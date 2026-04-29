// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GRADEEDITDIALOG_H
#define AAIMS_GRADEEDITDIALOG_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QTabWidget>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class GradeEditDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit GradeEditDialog(const QUuid &courseUuid, QWidget *parent = nullptr);

private slots:
    void onSaveButtonClicked();

    void onEditStudentGrades(int row);

private:
    void setupUI();

    void loadStudents();

    void updateStudentGrades();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *infoLayout;
    QLabel *courseIdLabel;
    QLabel *courseNameLabel;

    QTabWidget *tabWidget;
    QWidget *singleEditPage;
    QVBoxLayout *singleLayout;
    QTableWidget *gradesTable;

    QHBoxLayout *btnLayout;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    QUuid courseUuid;
    QList<QUuid> studentUuids; // List of students in this course
};

#endif //AAIMS_GRADEEDITDIALOG_H
