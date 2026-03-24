// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H
#define AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H

#include <QDialog>
#include "../utils/DataStructures.h"

namespace Ui {
    class TeacherDetailDialog;
}

class TeacherDetailDialog : public QDialog {
    Q_OBJECT

public:
    enum class Mode { Detail, Edit };

    explicit TeacherDetailDialog(Mode mode, aaims::model::TeacherAccount *teacher = nullptr, QWidget *parent = nullptr);

    ~TeacherDetailDialog() override;

private slots:
    void onSaveClicked();

    void onSwitchToEdit();

private:
    Ui::TeacherDetailDialog *ui;
    Mode currentMode;
    aaims::model::TeacherAccount *currentTeacher;

    void setupUiByMode();

    void loadTeacherData();
};

#endif // AAIMS_GUI_TEACHER_TEACHERDETAILDIALOG_H
