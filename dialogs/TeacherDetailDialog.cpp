// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherDetailDialog.h"

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProgressDialog>
#include <QValidator>
#include <QLineEdit>

#include "AddDepartmentDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"

TeacherDetailDialog::TeacherDetailDialog(TeacherAccount *account,
                                         QWidget *parent) : StyledDialog(parent), account(account) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setFixedSize(450, 320);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    headerLabel = new QLabel("编辑教师信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QFormLayout();
    tableLayout->setSpacing(15);

    editId = new QLineEdit(this);
    editId->setStyleSheet("color: #a0a0a9;");
    editId->setEnabled(false);
    editId->setText(account->username);

    editName = new QLineEdit(this);
    editName->setText(account->name);

    deptLayout = new QHBoxLayout();

    completer = new QCompleter(aaims::manager::classes::get_departments());
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    comboDept = new QComboBox(this);
    comboDept->addItems(aaims::manager::classes::get_departments());
    comboDept->setEditable(true);
    comboDept->setPlaceholderText("请选择学院");
    if (aaims::manager::classes::get_departments().contains(account->department)) {
        comboDept->setCurrentText(account->department);
    }
    comboDept->setInsertPolicy(QComboBox::NoInsert);
    comboDept->setCompleter(completer);

    btnAdd = new QPushButton("+", this);
    btnAdd->setStyleSheet("padding: 0; margin: 0;");
    btnAdd->setObjectName("AddElement");
    btnAdd->setFixedSize(24, 24);

    deptLayout->addWidget(comboDept);
    deptLayout->addWidget(btnAdd);

    editPhoneNumber = new QLineEdit(this);
    editPhoneNumber->setText(account->phoneNumber);
    editPhoneNumber->setValidator(new QRegularExpressionValidator(QRegularExpression("^1[3-9]\\d{9}$"), this));

    tableLayout->addRow("用户名", editId);
    tableLayout->addRow("姓名", editName);
    tableLayout->addRow("院系", deptLayout);
    tableLayout->addRow("手机号码:", editPhoneNumber);

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

    connect(btnAdd, &QPushButton::clicked, [this] {
        if (AddDepartmentDialog dialog; dialog.exec() == Accepted) {
            comboDept->clear();
            comboDept->addItems(aaims::manager::classes::get_departments());
        }
    });
    connect(btnSave, &QPushButton::clicked, this, &TeacherDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void TeacherDetailDialog::onSaveButtonClicked() {
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名不能为空！");
        return;
    }
    if (!aaims::manager::classes::get_departments().contains(comboDept->currentText())) {
        QMessageBox::warning(this, "输入错误", "请选择院系！");
        return;
    }
    static const QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    if (editPhoneNumber->text().trimmed().length() != 11 || !phoneRegex.match(editPhoneNumber->text().trimmed()).
        isValid()) {
        QMessageBox::warning(this, "输入错误", "无效的中国大陆手机号！");
        return;
    }
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    account->name = editName->text().trimmed();
    account->department = comboDept->currentText();
    account->phoneNumber = editPhoneNumber->text().trimmed();
    const auto future = aaims::manager::account::saveAsync();
    auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "保存完成", QString("保存教师成功！"));
        accept();
    });
    watcher->setFuture(future);
}
