// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StudentDetailDialog.h"

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QValidator>
#include <QPushButton>
#include <qtconcurrentrun.h>

#include "AddClassDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"

StudentDetailDialog::StudentDetailDialog(StudentAccount *account, QWidget *parent) : StyledDialog(parent),
    account(account) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setFixedSize(450, 350);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    headerLabel = new QLabel("编辑学生信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QFormLayout();
    tableLayout->setSpacing(15);

    editId = new QLineEdit(this);
    editId->setStyleSheet("color: #a0a0a9;");
    editId->setEnabled(false);
    editId->setText(account->username);

    editName = new QLineEdit(this);
    editName->setText(account->name);

    classLayout = new QHBoxLayout();

    completer = new QCompleter(aaims::manager::classes::get_departments());
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    comboClass = new QComboBox(this);
    for (const auto &x: aaims::manager::classes::get_all_ptr()) {
        comboClass->addItem(x->grade + x->name, x->uuid);
    }
    comboClass->setEditable(true);
    comboClass->setPlaceholderText("请选择班级");
    if (aaims::manager::classes::get_classes().contains(account->currentClass)) {
        const auto cls = aaims::manager::classes::get_classes()[account->currentClass];
        comboClass->setCurrentText(cls->grade + cls->name);
    }
    comboClass->setInsertPolicy(QComboBox::NoInsert);
    comboClass->setCompleter(completer);

    btnAdd = new QPushButton("+", this);
    btnAdd->setStyleSheet("padding: 0; margin: 0;");
    btnAdd->setObjectName("AddElement");
    btnAdd->setFixedSize(24, 24);

    classLayout->addWidget(comboClass);
    classLayout->addWidget(btnAdd);

    comboStatus = new QComboBox(this);
    comboStatus->setPlaceholderText("请选择状态");
    comboStatus->addItems({"在校", "毕业", "休学"});
    if (account->is_graduated()) comboStatus->setCurrentIndex(1);
    else if (account->is_graduated()) comboStatus->setCurrentIndex(2);
    else comboStatus->setCurrentIndex(0);

    editRoom = new QLineEdit(this);
    editRoom->setText(account->dormitory);

    editPhoneNumber = new QLineEdit(this);
    editPhoneNumber->setText(account->phoneNumber);
    editPhoneNumber->setValidator(new QRegularExpressionValidator(QRegularExpression("^1[3-9]\\d{9}$"), this));

    tableLayout->addRow("用户名:", editId);
    tableLayout->addRow("姓名:", editName);
    tableLayout->addRow("班级:", classLayout);
    tableLayout->addRow("状态:", comboStatus);
    tableLayout->addRow("宿舍:", editRoom);
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
        if (AddClassDialog dialog(this); dialog.exec() == Accepted) {
            comboClass->clear();
            comboClass->addItems(aaims::manager::classes::get_departments());
        }
    });
    connect(btnSave, &QPushButton::clicked, this, &StudentDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void StudentDetailDialog::onSaveButtonClicked() {
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名不能为空！");
        return;
    }
    const auto &clsUuid = comboClass->currentData().value<QUuid>();
    if (const auto *cls = aaims::manager::classes::get_classes()[clsUuid].get(); !cls) {
        QMessageBox::warning(this, "输入错误", "请选择班级！");
        return;
    }
    static const QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    if (editPhoneNumber->text().trimmed().length() != 11 || !phoneRegex.match(
            editPhoneNumber->text().trimmed()).isValid()) {
        QMessageBox::warning(this, "输入错误", "无效的中国大陆手机号！");
        return;
    }
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    account->name = editName->text().trimmed();
    account->currentClass = clsUuid;
    account->phoneNumber = editPhoneNumber->text().trimmed();
    switch (comboStatus->currentIndex()) {
        case 0: {
            if (account->is_suspended()) aaims::manager::account::get_suspended_students().remove(account->uuid);
            else if (account->is_graduated()) aaims::manager::account::get_graduated_students().remove(account->uuid);
            else break;
            aaims::manager::account::get_working_students()[account->uuid] = account;
        }
        case 1: {
            if (account->is_suspended()) aaims::manager::account::get_suspended_students().remove(account->uuid);
            else if (!account->is_graduated()) aaims::manager::account::get_working_students().remove(account->uuid);
            else break;
            aaims::manager::account::get_graduated_students()[account->uuid] = account;
        }
        case 2: {
            if (account->is_graduated()) aaims::manager::account::get_graduated_students().remove(account->uuid);
            else if (!account->is_suspended()) aaims::manager::account::get_working_students().remove(account->uuid);
            else break;
            aaims::manager::account::get_suspended_students()[account->uuid] = account;
        }
        default: break;
    }
    const auto future = QtConcurrent::run([] { return aaims::manager::account::save(); });
    auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "保存完成", QString("保存班级成功！"));
        accept();
    });
    watcher->setFuture(future);
}
