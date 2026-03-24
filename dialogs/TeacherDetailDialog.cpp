// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherDetailDialog.h"

#include <QPushButton>
#include <QHBoxLayout>

TeacherDetailDialog::TeacherDetailDialog(const aaims::model::TeacherAccount *account, QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(450, 400);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    headerLabel = new QLabel("编辑教师信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");
    mainLayout->addWidget(headerLabel);

    btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    editId = new QLineEdit("用户名", this);
    editId->setEnabled(false);
    editId->setText(account->username);

    editName = new QLineEdit("姓名", this);
    editName->setText(account->name);

    editDept = new QLineEdit("所属院系", this);
    editDept->setText(account->department);

    editPhoneNumber = new QLineEdit("电话号码", this);
    editPhoneNumber->setText(account->phoneNumber);

    btnLayout->addWidget(editId);
    btnLayout->addWidget(editName);
    btnLayout->addWidget(editDept);
    btnLayout->addLayout(btnLayout);

    btnCancel = new QPushButton("取消", this);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnSave->setObjectName("AddElement");

    btnSave = new QPushButton("保存修改", this);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnSave->setObjectName("AddElement");

    mainLayout->addLayout(btnLayout);

    connect(btnSave, &QPushButton::clicked, this, &TeacherDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void TeacherDetailDialog::onSaveButtonClicked() {
    emit saveRequested(editId->text(), editName->text(), editDept->text());
    accept();
}
