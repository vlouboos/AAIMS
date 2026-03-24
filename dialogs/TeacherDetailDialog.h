// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H
#define AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H

#include <QBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>

#include "../utils/DataStructures.h"

class TeacherDetailDialog : public QDialog {
    Q_OBJECT

public:
    explicit TeacherDetailDialog(const aaims::model::TeacherAccount *account, QWidget *parent = nullptr);

    void setTeacherData(const QString &id, const QString &name, const QString &dept);

signals:
    void saveRequested(const QString &id, const QString &name, const QString &dept);

private slots:
    void onSaveButtonClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QLineEdit *editId;
    QLineEdit *editName;
    QLineEdit *editDept;
    QLineEdit *editPhoneNumber;
    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;
};

#endif // AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H
