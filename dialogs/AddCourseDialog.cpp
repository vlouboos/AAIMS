// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddCourseDialog.h"

#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>

AddCourseDialog::AddCourseDialog(QWidget *parent) : StyledDialog(parent) {
    mainLayout = new QVBoxLayout(this);

    formLayout = new QFormLayout();

    editId = new QLineEdit(this);
    editName = new QLineEdit(this);
    editInstructor = new QLineEdit(this);

    comboCredits = new QComboBox(this);
    for (int i = 1; i <= 4; i++) comboCredits->addItem(QString::number(i) + " 学分", i);

    formLayout->addRow("课程编号:", editId);
    formLayout->addRow("课程名称:", editName);
    formLayout->addRow("授课教师:", editInstructor);
    formLayout->addRow("学分:", comboCredits);

    timeGroup = new QGroupBox("上课时间", this);
    groupLayout = new QVBoxLayout(timeGroup);

    btnAddSlot = new QPushButton("+ 添加时段", this);
    connect(btnAddSlot, &QPushButton::clicked, this, &AddCourseDialog::onAddSlotClicked);
    groupLayout->addWidget(btnAddSlot, 0, Qt::AlignRight);

    timeSlotsLayout = new QVBoxLayout();
    groupLayout->addLayout(timeSlotsLayout);

    btnLayout = new QHBoxLayout();
    btnCancel = new QPushButton("取消", this);
    btnSave = new QPushButton("添加", this);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(timeGroup);
    mainLayout->addLayout(btnLayout);

    setWindowTitle("添加课程");
    resize(500, 400);

    const auto w = new TimeSlot(Course::LessonTime(), this);
    // Strange, IDK why it keeps stupid clangD.
    connect(w, &TimeSlot::removeRequested, this, [this, w] { removeSlot(w); });
    connect(btnSave, &QPushButton::clicked, this, [this] {
        if (validateForm()) {
            accept();
        }
    });
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    timeSlotsLayout->addWidget(w);
    slotWidgets.append(w);
    applyStyles();
}

void AddCourseDialog::onAddSlotClicked() {
    const auto w = new TimeSlot(Course::LessonTime(), this);
    connect(w, &TimeSlot::removeRequested, this, [this, w] { removeSlot(w); });
    timeSlotsLayout->addWidget(w);
    slotWidgets.append(w);
}

void AddCourseDialog::removeSlot(TimeSlot *slot) {
    if (slotWidgets.size() <= 1) {
        QMessageBox::warning(this, "提示", "至少需要保留一个上课时间段！");
        return;
    }

    slotWidgets.removeOne(slot);
    timeSlotsLayout->removeWidget(slot);
    slot->deleteLater();
}

bool AddCourseDialog::validateForm() {
    if (editId->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "课程编号不能为空");
        return false;
    }
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "课程名称不能为空");
        return false;
    }

    // [0-6][0-14]
    int occupied[7][15];
    return std::ranges::all_of(slotWidgets, [this, &occupied](const auto w) {
        const auto data = w->toData();
        if (data.weekEnd < data.weekStart) {
            QMessageBox::warning(this, "时间错误", "结束周不能早于起始周！");
            return false;
        }
        if (data.startTime + data.duration > 15) {
            QMessageBox::warning(this, "时间溢出", "排课时间超出了当天的最大课节范围！");
            return false;
        }
        int mask = 0;
        for (int i = data.weekStart; i <= data.weekEnd; ++i) {
            mask |= 1 << (i - 1);
        }
        for (int i = 0; i < data.duration; i++) {
            if (occupied[data.dayOfWeek - 1][data.startTime + i] & mask) {
                QMessageBox::warning(this, "时间冲突", "时间安排存在冲突！");
                return false;
            }
            occupied[data.dayOfWeek - 1][data.startTime + i] |= mask;
        }
        return true;
    });
}
