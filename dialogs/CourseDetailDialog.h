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
    explicit CourseDetailDialog(Course* course = nullptr, QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *tableLayout;
    QLineEdit *editId;
    QLineEdit *editName;
    QHBoxLayout *majorLayout;
    QCompleter *completerMajor;
    QComboBox *comboMajor;
    QPushButton *btnAddMajor;
    QHBoxLayout *teacherLayout;
    QComboBox *comboTeacher;
    QCompleter *teacherCompleter;
    QPushButton *btnAddTeacher;
    QComboBox *comboCredits;
    QGroupBox *timeGroup;
    QVBoxLayout *timeSlotsLayout;
    QVBoxLayout *groupLayout;
    QPushButton *btnAddSlot;
    QList<TimeSlot *> slotWidgets;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
};



#endif //AAIMS_COURSEDETAILDIALOG_H
