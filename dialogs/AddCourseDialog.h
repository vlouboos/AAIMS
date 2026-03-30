// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDCOURSEDIALOG_H
#define AAIMS_ADDCOURSEDIALOG_H

#pragma once
#include <QLineEdit>

#include "StyledDialog.h"
#include "../pages/components/TimeSlot.h"
#include "../utils/DataStructures.h"

class AddCourseDialog : public StyledDialog {
    explicit AddCourseDialog(const Course &initialData, QWidget *parent = nullptr);

    [[nodiscard]] Course getCourseData() const;

private slots:
    void onAddSlotClicked();

    void onRemoveSlotRequested(TimeSlot *widget);

private:
    QLineEdit *editId;
    QLineEdit *editName;
    QLineEdit *editInstructor;
    QComboBox *comboCredits;
    QComboBox *comboWeekStart;
    QComboBox *comboWeekEnd;
    QVBoxLayout *timeSlotsLayout;
    QList<TimeSlot *> slotWidgets;
    Course data;

    void populateData();

    bool validateForm();
};

#endif //AAIMS_ADDCOURSEDIALOG_H
