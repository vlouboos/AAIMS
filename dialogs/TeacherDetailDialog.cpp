// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherDetailDialog.h"

#include <QFutureWatcher>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProgressDialog>

#include "../managements/AccountManager.h"

TeacherDetailDialog::TeacherDetailDialog(TeacherAccount *account,
                                         QWidget *parent) : StyledDialog(parent), account(account) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setFixedSize(450, 320);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    headerLabel = new QLabel("编辑教师信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QVBoxLayout();
    tableLayout->setSpacing(15);

    idLayout = new QHBoxLayout();

    idLabel = new QLabel("用户名", this);

    editId = new QLineEdit(this);
    editId->setStyleSheet("color: #a0a0a9;");
    editId->setEnabled(false);
    editId->setText(account->username);

    idLayout->addWidget(idLabel);
    idLayout->addWidget(editId);

    nameLayout = new QHBoxLayout();

    nameLabel = new QLabel("姓名", this);

    editName = new QLineEdit(this);
    editName->setText(account->name);

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(editName);

    deptLayout = new QHBoxLayout();

    deptLabel = new QLabel("所属院系", this);

    editDept = new QLineEdit(this);
    editDept->setText(account->department);

    deptLayout->addWidget(deptLabel);
    deptLayout->addWidget(editDept);

    phoneNumberLayout = new QHBoxLayout();

    phoneNumberLabel = new QLabel("电话号码", this);

    editPhoneNumber = new QLineEdit(this);
    editPhoneNumber->setText(account->phoneNumber);

    phoneNumberLayout->addWidget(phoneNumberLabel);
    phoneNumberLayout->addWidget(editPhoneNumber);

    tableLayout->addLayout(idLayout);
    tableLayout->addLayout(nameLayout);
    tableLayout->addLayout(deptLayout);
    tableLayout->addLayout(phoneNumberLayout);

    btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    btnSave = new QPushButton("保存修改", this);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnSave->setObjectName("AddElement");

    btnCancel = new QPushButton("取消", this);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setObjectName("AddElement");

    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);

    mainLayout->addWidget(headerLabel);
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(btnLayout);
    applyStyles();

    connect(btnSave, &QPushButton::clicked, this, &TeacherDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void TeacherDetailDialog::onSaveButtonClicked() {
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名不能为空！");
        return;
    }
    if (editDept->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "院系不能为空！");
        return;
    }
    static const QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    if (!phoneRegex.match(editPhoneNumber->text().trimmed()).isValid()) {
        QMessageBox::warning(this, "输入错误", "无效的中国大陆手机号！");
        return;
    }
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    account->name = editName->text().trimmed();
    account->department = editDept->text().trimmed();
    account->phoneNumber = editPhoneNumber->text().trimmed();
    const auto future = aaims::manager::account::saveAsync();
    auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
        pd->close();
        watcher->deleteLater();
        QMessageBox::information(this, "保存完成", QString("保存教师成功！"));
        accept();
    });
    watcher->setFuture(future);
}
