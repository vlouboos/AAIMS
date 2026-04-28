// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_COURSEDETAILDIALOG_H
#define AAIMS_COURSEDETAILDIALOG_H

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>

#include "StyledDialog.h"
#include "../pages/components/TimeSlot.h"
#include "../utils/DataStructures.h"

class CourseDetailDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit CourseDetailDialog(Course *course = nullptr, QWidget *parent = nullptr);

    void onAddSlotClicked();

    void removeSlot(TimeSlot *slot);

    bool validateForm();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *formLayout;
    QComboBox *comboSemester;
    QLineEdit *editId;
    QLineEdit *editName;
    QHBoxLayout *teacherLayout;
    QComboBox *comboTeacher;
    QCompleter *teacherCompleter;
    QPushButton *btnAddTeacher;
    QComboBox *comboCredits;
    QComboBox *comboStatus;
    QPushButton *onlineCheck;
    QGroupBox *timeGroup;
    QVBoxLayout *timeSlotsLayout;
    QVBoxLayout *groupLayout;
    QPushButton *btnAddSlot;
    QList<TimeSlot *> slotWidgets;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    Course *course;
};


#endif //AAIMS_COURSEDETAILDIALOG_H
