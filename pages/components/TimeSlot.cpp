// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TimeSlot.h"

#include <QLabel>

TimeSlot::TimeSlot(const Course::LessonTime &l, QWidget *parent) : QWidget(parent) {
    layout = new QHBoxLayout(this);

    comboDay = new QComboBox(this);
    comboDay->addItems(Course::LessonTime::DAY_OF_WEEK_TABLE);

    comboStart = new QComboBox(this);
    for (int i = 0; i < 15; ++i) {
        comboStart->addItem(QString("第 %1 节 (%2)").arg(i + 1).arg(Course::LessonTime::TIME_TABLE[i]), i);
    }
    comboStart->setCurrentIndex(l.startTime);

    duration = new QSpinBox(this);
    duration->setRange(1, 5);
    duration->setSuffix(" 节课");
    duration->setValue(l.duration);

    weekStart = new QSpinBox(this);
    weekStart->setRange(1, 20);
    weekStart->setValue(l.weekStart);

    weekEnd = new QSpinBox(this);
    weekEnd->setRange(1, 20);
    weekEnd->setValue(l.weekEnd);

    btnRemove = new QPushButton("X", this);
    btnRemove->setFixedWidth(30);

    layout->addWidget(comboDay);
    layout->addWidget(new QLabel("从", this));
    layout->addWidget(comboStart);
    layout->addWidget(new QLabel("开始上", this));
    layout->addWidget(duration);
    layout->addWidget(new QLabel(" | 第", this));
    layout->addWidget(weekStart);
    layout->addWidget(new QLabel("-", this));
    layout->addWidget(weekEnd);
    layout->addWidget(new QLabel("周", this));
    layout->addWidget(btnRemove);

    connect(btnRemove, &QPushButton::clicked, this, [this] {
        emit removeRequested();
    });
}

Course::LessonTime TimeSlot::toData() const {
    Course::LessonTime s{};
    s.dayOfWeek = comboDay->currentIndex();
    s.startTime = comboStart->currentIndex();
    s.weekStart = weekStart->value();
    s.weekEnd = weekEnd->value();
    s.duration = duration->value();
    return s;
}
