// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_QUALIFYCOURSEDIALOG_H
#define AAIMS_QUALIFYCOURSEDIALOG_H

#include <QSplitter>
#include <QListWidget>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class QualifyCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit QualifyCourseDialog(QList<aaims::model::Course *> qualifyingCourses, QWidget *parent = nullptr);

private:
    QHBoxLayout *mainLayout;
    QSplitter *splitter;
    QWidget *leftPanel;
    QWidget *rightPanel;
    QLabel *leftTitle;
    QVBoxLayout *leftPanelLayout;
    QListWidget *courseList;
    QVBoxLayout *rightPanelLayout;
    QWidget *detailsWidget;
    QVBoxLayout *detailsLayout;
    QHBoxLayout *infoLayout;
    QScrollArea *studentsArea;
    QWidget *studentsWidget;
    QVBoxLayout *studentsLayout;
    QLabel *studentsTitle;
    QLabel *selectedCourseLabel;
    QLabel *capacityLabel;
    QLabel *currentLabel;
    QLabel *exceededLabel;
    QLabel *capLabel;
    QLabel *curLabel;
    QLabel *excLabel;

    QHBoxLayout *buttonLayout;
    QPushButton *btnFinalize;
    QPushButton *btnCancel;

    QList<aaims::model::Course *> qualifyingCourses;
    aaims::model::Course *currentCourse = nullptr;

    void populateCourses();

    void updateCourseDetails() const;

    void loadStudentsForCurrentCourse();

    void removeStudentFromCourse(const QUuid &studentUuid);

private slots:
    void onCourseSelected(QListWidgetItem *current, QListWidgetItem *previous);

    void onRemoveStudentClicked(const QUuid &studentUuid);
};

#endif //AAIMS_QUALIFYCOURSEDIALOG_H
