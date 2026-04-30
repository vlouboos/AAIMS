// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "PasswordResetDialog.h"

#include <QFutureWatcher>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QtConcurrentRun>

#include "../managements/AccountManager.h"
#include "../utils/DataStructures.h"
#include "../utils/Sha256Util.h"

PasswordResetDialog::PasswordResetDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("修改密码");
    resize(400, 300);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    titleLabel = new QLabel("修改密码", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #0f172a;");

    formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignLeft);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Current password field
    currentPasswordEdit = new QLineEdit(this);
    currentPasswordEdit->setPlaceholderText("请输入当前密码");
    currentPasswordEdit->setEchoMode(QLineEdit::Password);

    // New password field
    newPasswordEdit = new QLineEdit(this);
    newPasswordEdit->setPlaceholderText("请输入新密码");
    newPasswordEdit->setEchoMode(QLineEdit::Password);

    // Confirm password field
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("请再次输入新密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    passwordStatus = new QLabel("", this);
    passwordStatus->setStyleSheet("color: #ef4444; font-size: 12px;");

    formLayout->addRow("当前密码:", currentPasswordEdit);
    formLayout->addRow("新密码:", newPasswordEdit);
    formLayout->addRow("确认密码:", confirmPasswordEdit);
    formLayout->addRow("", passwordStatus);

    // Buttons
    btnLayout = new QHBoxLayout();
    resetBtn = new QPushButton("修改密码", this);
    resetBtn->setObjectName("AddElement");
    resetBtn->setEnabled(false); // Initially disabled

    cancelBtn = new QPushButton("取消", this);
    cancelBtn->setObjectName("AddElement");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addStretch();
    btnLayout->addWidget(resetBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);

    // Connect signals
    connect(currentPasswordEdit, &QLineEdit::textChanged, this, &PasswordResetDialog::onCurrentPasswordChanged);
    connect(newPasswordEdit, &QLineEdit::textChanged, this, &PasswordResetDialog::onNewPasswordChanged);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &PasswordResetDialog::onConfirmPasswordChanged);
    connect(resetBtn, &QPushButton::clicked, this, &PasswordResetDialog::onResetPasswordClicked);
    updateButtonState();

    applyStyles();
}

void PasswordResetDialog::onCurrentPasswordChanged() const {
    updateButtonState();
}

void PasswordResetDialog::onNewPasswordChanged() const {
    const QString password = newPasswordEdit->text();

    // Clear previous status
    passwordStatus->clear();

    if (!password.isEmpty()) {
        // Validate password strength
        if (password.length() < 6) {
            passwordStatus->setText("密码长度至少为6位");
        } else if (password.length() > 26) {
            passwordStatus->setText("密码长度不能超过26位");
        } else {
            // Additional checks can be added here
            updateButtonState();
        }
    } else {
        updateButtonState();
    }
}

void PasswordResetDialog::onConfirmPasswordChanged() const {
    const QString newPassword = newPasswordEdit->text();

    if (const QString confirmPassword = confirmPasswordEdit->text(); !confirmPassword.isEmpty() && !newPassword.isEmpty() && newPassword != confirmPassword) {
        passwordStatus->setText("两次输入的新密码不一致");
    } else if (!confirmPassword.isEmpty() && newPassword.isEmpty()) {
        passwordStatus->setText("请输入新密码");
    } else {
        if (!newPassword.isEmpty() && newPassword == confirmPassword) {
            if (newPassword.length() < 6) {
                passwordStatus->setText("密码长度至少为6位");
            } else if (newPassword.length() > 26) {
                passwordStatus->setText("密码长度不能超过26位");
            } else {
                passwordStatus->clear();
            }
        }
    }

    updateButtonState();
}

void PasswordResetDialog::updateButtonState() const {
    const QString currentPassword = currentPasswordEdit->text();
    const QString newPassword = newPasswordEdit->text();
    const QString confirmPassword = confirmPasswordEdit->text();
    const bool isValid = !currentPassword.isEmpty() &&
                         !newPassword.isEmpty() && newPassword.length() >= 6 && newPassword.length() <= 26 &&
                         !confirmPassword.isEmpty() && newPassword == confirmPassword;

    resetBtn->setEnabled(isValid);
}

void PasswordResetDialog::onResetPasswordClicked() {
    QString currentPassword = currentPasswordEdit->text();
    QString newPassword = newPasswordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();

    // Validation is already handled by updateButtonState(), but let's double-check
    if (currentPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写所有必填字段");
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "警告", "两次输入的新密码不一致");
        return;
    }

    if (newPassword.length() < 6) {
        QMessageBox::warning(this, "警告", "新密码长度至少为6位");
        return;
    }

    Account *account = aaims::manager::account::logged;
    if (Sha256Util::hash(currentPassword) != account->password) {
        QMessageBox::warning(this, "错误", "原密码输入错误");
        return;
    }
    auto *pd = new QProgressDialog("正在修改...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    const auto future = QtConcurrent::run([this, account] {
        account->password = Sha256Util::hash(newPasswordEdit->text());
        return aaims::manager::account::save();
    });
    const auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, pd, watcher] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "成功", "密码修改成功");
        accept();
    });
    watcher->setFuture(future);
}
