// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddCourseDialog.h"

#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>

AddCourseDialog::AddCourseDialog(QWidget *parent) : StyledDialog(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();
    editId = new QLineEdit(this);
    editName = new QLineEdit(this);
    editInstructor = new QLineEdit(this);

    comboCredits = new QComboBox(this);
    for (int i = 1; i <= 6; ++i) comboCredits->addItem(QString::number(i) + " 学分", i);

    QHBoxLayout *weekLayout = new QHBoxLayout();
    comboWeekStart = new QComboBox(this);
    comboWeekEnd = new QComboBox(this);
    for (int i = 1; i <= 20; ++i) {
        comboWeekStart->addItem(QString::number(i), i);
        comboWeekEnd->addItem(QString::number(i), i);
    }
    weekLayout->addWidget(new QLabel("第"));
    weekLayout->addWidget(comboWeekStart);
    weekLayout->addWidget(new QLabel("周 至 第"));
    weekLayout->addWidget(comboWeekEnd);
    weekLayout->addWidget(new QLabel("周"));
    weekLayout->addStretch();

    formLayout->addRow("课程编号:", editId);
    formLayout->addRow("课程名称:", editName);
    formLayout->addRow("授课教师:", editInstructor);
    formLayout->addRow("学分:", comboCredits);
    formLayout->addRow("周次范围:", weekLayout);

    mainLayout->addLayout(formLayout);

    // 2. 动态时间段区域 (上课时间段)
    QGroupBox *timeGroup = new QGroupBox("上课时间", this);
    QVBoxLayout *groupLayout = new QVBoxLayout(timeGroup);

    QPushButton *btnAddSlot = new QPushButton("+ 添加时段", this);
    connect(btnAddSlot, &QPushButton::clicked, this, &AddCourseDialog::onAddSlotClicked);
    groupLayout->addWidget(btnAddSlot, 0, Qt::AlignRight);

    timeSlotsLayout = new QVBoxLayout();
    groupLayout->addLayout(timeSlotsLayout);
    mainLayout->addWidget(timeGroup);

    // 3. 底部按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnCancel = new QPushButton("取消", this);
    QPushButton *btnSave = new QPushButton("添加", this);

    // Qt 默认行为绑定
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);
    mainLayout->addLayout(btnLayout);

    setWindowTitle("添加课程");
    resize(500, 600);

    applyStyles();
}

void AddCourseDialog::onAddSlotClicked() {
    auto w = new TimeSlot(Course::LessonTime(), this);
    //connect(w, &TimeSlot::removeRequested, this, &AddCourseDialog::onRemoveSlotRequested);
    timeSlotsLayout->addWidget(w);
    slotWidgets.append(w);
}

void AddCourseDialog::onRemoveSlotRequested(TimeSlot *widget) {
    if (slotWidgets.size() <= 1) {
        QMessageBox::warning(this, "提示", "至少需要保留一个上课时间段！");
        return;
    }

    slotWidgets.removeOne(widget);
    timeSlotsLayout->removeWidget(widget);
    widget->deleteLater();
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
    if (comboWeekStart->currentData().toInt() > comboWeekEnd->currentData().toInt()) {
        QMessageBox::warning(this, "错误", "起始周不能大于结束周");
        return false;
    }

    for (const auto *w: slotWidgets) {
        const auto data = w->getData();
        if (data.startTime + data.duration > 15) {
            QMessageBox::warning(this, "时间冲突", "排课时间超出了当天的最大课节范围！");
            return false;
        }

        if (data.weekStart > data.weekEnd) {
            QMessageBox::warning(this, "周次错误", "起始周不能晚于结束周");
            return false;
        }
    }

    return true;
}
