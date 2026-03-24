// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherDetailDialog.h"
#include <QPushButton>

TeacherDetailDialog::TeacherDetailDialog(Mode mode, aaims::model::TeacherAccount *teacher, QWidget *parent)
    : QDialog(parent), ui(new Ui::TeacherDetailDialog), currentMode(mode), currentTeacher(teacher) {
    ui->setupUi(this);

    loadTeacherData();
    setupUiByMode();

    connect(ui->btnSave, &QPushButton::clicked, this, &TeacherDetailDialog::onSaveClicked);
    connect(ui->btnEdit, &QPushButton::clicked, this, &TeacherDetailDialog::onSwitchToEdit);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

TeacherDetailDialog::~TeacherDetailDialog() {
    delete ui;
}

void TeacherDetailDialog::loadTeacherData() {
    if (!currentTeacher) return;
    ui->editName->setText(currentTeacher->name);
    ui->editId->setText(currentTeacher->username);
    ui->editDept->setText(currentTeacher->department);
    // 暴力填充其余字段...
}

void TeacherDetailDialog::setupUiByMode() {
    bool isEdit = (currentMode == Mode::Edit);

    // 基础信息只读控制
    ui->editName->setReadOnly(!isEdit);
    ui->editId->setReadOnly(!isEdit);
    ui->editDept->setReadOnly(!isEdit);

    // 按钮显隐
    ui->btnSave->setVisible(isEdit);
    ui->btnEdit->setVisible(!isEdit);

    this->setWindowTitle(isEdit ? "编辑教师信息" : "教师详细信息");
}

void TeacherDetailDialog::onSwitchToEdit() {
    currentMode = Mode::Edit;
    setupUiByMode();
}

void TeacherDetailDialog::onSaveClicked() {
    if (!currentTeacher) return;

    // 暴力同步 UI 数据到对象
    currentTeacher->name = ui->editName->text();
    currentTeacher->department = ui->editDept->text();

    // 触发你之前的异步存盘
    // aaims::manager::account::save();

    accept();
}
