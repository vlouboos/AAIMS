// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSDETAILDIALOG_H
#define AAIMS_CLASSDETAILDIALOG_H

#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QFormLayout>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class ClassDetailDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit ClassDetailDialog(aaims::model::Class *cls, QWidget *parent = nullptr);

private slots:
    void onSaveButtonClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *tableLayout;
    QLineEdit *editName;
    QLineEdit *editGrade;
    QHBoxLayout *majorLayout;
    QCompleter *completerMajor;
    QComboBox *comboMajor;
    QPushButton *btnAddMajor;
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
    QPushButton *btnExportGrades;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    aaims::model::Class *cls;
    QList<QUuid> workingCourses;
};

#endif //AAIMS_CLASSDETAILDIALOG_H
