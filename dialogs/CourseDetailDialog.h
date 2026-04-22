// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_COURSEDETAILDIALOG_H
#define AAIMS_COURSEDETAILDIALOG_H

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>

#include "StyledDialog.h"

class CourseDetailDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit CourseDetailDialog(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *tableLayout;
    QLineEdit *editName;
    QLineEdit *editGrade;
    QHBoxLayout *deptLayout;
    QCompleter *completerDept;
    QComboBox *comboDept;
    QPushButton *btnAddDept;
    QHBoxLayout *masterLayout;
    QCompleter *completerMaster;
    QComboBox *comboMaster;
    QPushButton *btnAddTeacher;
    QVBoxLayout *coursesLayout;
    QLabel *coursesLabel;
    QHBoxLayout *coursesEditLayout;
    QScrollArea *courses;
    QListWidget *courseList;
    QVBoxLayout * courseBtnLayout;
    QPushButton *btnAddCourse;
    QPushButton *btnRemoveCourse;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
};



#endif //AAIMS_COURSEDETAILDIALOG_H
