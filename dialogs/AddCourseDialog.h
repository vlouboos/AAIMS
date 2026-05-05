// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDCOURSEDIALOG_H
#define AAIMS_ADDCOURSEDIALOG_H

#pragma once
#include <QGroupBox>

#include "AddClassDialog.h"
#include "AssignmentRuleDialog.h"
#include "StyledDialog.h"
#include "../pages/components/TimeSlot.h"

class AddCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddCourseDialog(QWidget *parent = nullptr);

private slots:
    void onAddSlotClicked();

private:
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    QWidget *singleAddPage;
    QVBoxLayout *singleLayout;
    QFormLayout *formLayout;
    QComboBox *comboSemester;
    QLineEdit *editId;
    QLineEdit *editName;
    QHBoxLayout *teacherLayout;
    QComboBox *comboTeacher;
    QCompleter *teacherCompleter;
    QPushButton *btnAddTeacher;
    QComboBox *comboCredits;
    QPushButton *onlineCheck;
    QPushButton *btnSetAssignmentRule;
    QGroupBox *timeGroup;
    QVBoxLayout *timeSlotsLayout;
    QVBoxLayout *groupLayout;
    QPushButton *btnAddSlot;
    QList<TimeSlot *> slotWidgets;
    QHBoxLayout *btnLayout;
    QPushButton *btnCancel;
    QPushButton *btnSave;
    QWidget *batchAddPage;
    QVBoxLayout *batchLayout;
    QLabel *tipLabel;
    QLabel *fileStatusLabel;
    QPushButton *btnSelectFile;
    QPushButton *btnConfirmBatch;
    QString selectedFilePath;
    Course::AssignmentRule currentAssignmentRule;

    bool validateForm();

    void removeSlot(TimeSlot *slot);

    [[nodiscard]] QPair<unsigned long long, unsigned long long> importFromCsv() const;
};

#endif //AAIMS_ADDCOURSEDIALOG_H
