// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H
#define AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H

#include <QLineEdit>
#include <QLabel>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class TeacherDetailDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit TeacherDetailDialog(aaims::model::TeacherAccount *account, QWidget *parent = nullptr);

signals:
    void saveRequested(const QString &id, const QString &name, const QString &dept);

private slots:
    void onSaveButtonClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QVBoxLayout *tableLayout;
    QHBoxLayout *idLayout;
    QLabel *idLabel;
    QLineEdit *editId;
    QHBoxLayout *nameLayout;
    QLabel *nameLabel;
    QLineEdit *editName;
    QHBoxLayout *deptLayout;
    QLabel *deptLabel;
    QLineEdit *editDept;
    QHBoxLayout *phoneNumberLayout;
    QLabel *phoneNumberLabel;
    QLineEdit *editPhoneNumber;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    aaims::model::TeacherAccount *account;
};

#endif // AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H
