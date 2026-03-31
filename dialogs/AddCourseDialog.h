// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDCOURSEDIALOG_H
#define AAIMS_ADDCOURSEDIALOG_H

#pragma once
#include <qformlayout.h>
#include <qgroupbox.h>
#include <QLineEdit>

#include "StyledDialog.h"
#include "../pages/components/TimeSlot.h"

class AddCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddCourseDialog(QWidget *parent = nullptr);

private slots:
    void onAddSlotClicked();

    void onRemoveSlotRequested(TimeSlot *widget);

private:
    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QLineEdit *editId;
    QLineEdit *editName;
    QLineEdit *editInstructor;
    QComboBox *comboCredits;
    QHBoxLayout *weekLayout;
    QComboBox *comboWeekStart;
    QComboBox *comboWeekEnd;
    QGroupBox *timeGroup;
    QVBoxLayout *timeSlotsLayout;
    QVBoxLayout *groupLayout;
    QPushButton *btnAddSlot;
    QList<TimeSlot *> slotWidgets;
    QHBoxLayout *btnLayout;
    QPushButton *btnCancel;
    QPushButton *btnSave;

    bool validateForm();
};

#endif //AAIMS_ADDCOURSEDIALOG_H
