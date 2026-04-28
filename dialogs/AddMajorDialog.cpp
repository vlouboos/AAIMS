// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddMajorDialog.h"

#include <QCompleter>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "../managements/ClassManager.h"

AddMajorDialog::AddMajorDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("添加专业");
    setFixedSize(450, 280);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    formLayout = new QFormLayout(this);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(15);

    editName = new QLineEdit(this);
    editName->setPlaceholderText("如：软件工程");

    deptLayout = new QHBoxLayout(this);

    deptCompleter = new QCompleter(this);
    deptCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    deptCompleter->setFilterMode(Qt::MatchContains);
    deptCompleter->setCompletionMode(QCompleter::InlineCompletion);

    deptCombo = new QComboBox(this);
    deptCombo->setEditable(true);
    deptCombo->addItems(aaims::manager::classes::get_departments());
    deptCombo->setInsertPolicy(QComboBox::NoInsert);
    deptCombo->setCompleter(deptCompleter);

    btnAddDept = new QPushButton("+", this);
    btnAddDept->setStyleSheet("padding: 0; margin: 0;");
    btnAddDept->setObjectName("AddElement");
    btnAddDept->setFixedSize(24, 24);

    deptLayout->addWidget(deptCombo);
    deptLayout->addWidget(btnAddDept);

    formLayout->addRow("专业名字:", editName);
    formLayout->addRow("所属院系:", deptLayout);

    btnConfirm = new QPushButton(this);
    btnConfirm->setText("确认添加");
    btnConfirm->setObjectName("AddElement");

    mainLayout->addLayout(formLayout);

    applyStyles();

    connect(btnConfirm, &QPushButton::clicked, this, [this] {
        if (deptCombo->currentText().trimmed().isEmpty()) {
            QMessageBox::warning(this, "输入错误", "院系不能为空！", QMessageBox::Ok);
            return;
        }
        if (!aaims::manager::classes::get_departments().contains(deptCombo->currentText().trimmed())) {
            QMessageBox::warning(this, "输入错误", "请选择院系！", QMessageBox::Ok);
            return;
        }
        for (const auto &major: aaims::manager::classes::get_majors()) {
            if (major->name == editName->text().trimmed()) {
                QMessageBox::warning(this, "添加失败", "已存在同名专业", QMessageBox::Ok);
                return;
            }
        }
        auto major = std::make_shared<aaims::model::Major>();
        major->name = editName->text().trimmed();
        major->department = deptCombo->currentText().trimmed();
        aaims::manager::classes::addMajor(major);
    });
}
