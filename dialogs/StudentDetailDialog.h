// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STUDENTDETAILDIALOG_H
#define AAIMS_STUDENTDETAILDIALOG_H

#include <QComboBox>
#include <QCompleter>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class StudentDetailDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit StudentDetailDialog(aaims::model::StudentAccount *account, QWidget *parent = nullptr);

private slots:
    void onSaveButtonClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *tableLayout;
    QLineEdit *editId;
    QLineEdit *editName;
    QHBoxLayout *classLayout;
    QCompleter *completer;
    QComboBox *comboClass;
    QPushButton *btnAdd;
    QComboBox *comboStatus;
    QLineEdit *editRoom;
    QLineEdit *editPhoneNumber;
    QVBoxLayout *coursesLayout;
    QLabel *coursesLabel;
    QHBoxLayout *coursesEditLayout;
    QScrollArea *courses;
    QListWidget *courseList;
    QVBoxLayout *courseBtnLayout;
    QPushButton *btnAddCourse;
    QPushButton *btnRemoveCourse;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    aaims::model::StudentAccount *account;
    QList<QUuid> workingCourses;
};

#endif //AAIMS_STUDENTDETAILDIALOG_H
