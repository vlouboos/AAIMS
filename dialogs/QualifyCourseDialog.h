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
#include <QCheckBox>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class QualifyCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit QualifyCourseDialog(QList<QUuid> qualifyingCourses, QWidget *parent = nullptr);

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
    QPushButton *btnRemoveSelected;
    QPushButton *btnFinalize;
    QPushButton *btnCancel;
    
    QHash<QUuid, QCheckBox*> studentCheckBoxes;  // Track checkboxes for students

    QList<QUuid> qualifyingCourses;
    QUuid currentCourseUuid = aaims::model::EMPTY_UUID;

    void populateCourses();

    void updateCourseDetails() const;

    void loadStudentsForCurrentCourse();

    void removeSelectedStudents();

private slots:
    void onCourseSelected(QListWidgetItem *current, QListWidgetItem *previous);

    void onRemoveSelectedClicked();
};

#endif //AAIMS_QUALIFYCOURSEDIALOG_H