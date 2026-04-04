// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TIMESLOT_H
#define AAIMS_TIMESLOT_H

#pragma once
#include <QComboBox>
#include <QHBoxLayout>
#include <QTimeEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTime>

#include "../../utils/DataStructures.h"

using namespace aaims::model;

class TimeSlot : public QWidget {
    Q_OBJECT

public:
    explicit TimeSlot(const Course::LessonTime &l, QWidget *parent = nullptr);

    [[nodiscard]] Course::LessonTime toData() const;

signals:
    void removeRequested();

private:
    QHBoxLayout *layout;
    QComboBox *comboDay;
    QComboBox *comboStart;
    QSpinBox *duration;
    QSpinBox *weekStart;
    QSpinBox *weekEnd;
    QPushButton *btnRemove;
};

#endif //AAIMS_TIMESLOT_H
