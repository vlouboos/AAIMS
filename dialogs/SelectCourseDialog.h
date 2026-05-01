// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_SELECTCOURSEDIALOG_H
#define AAIMS_SELECTCOURSEDIALOG_H

#include <QListWidgetItem>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class SelectCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit SelectCourseDialog(const QList<QUuid> &availableCourses, QWidget *parent = nullptr);

private slots:
    void onConfirmClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QScrollArea *scrollArea;
    QWidget *coursesWidget;
    QVBoxLayout *coursesLayout;
    QPushButton *confirmBtn;
    QPushButton *cancelBtn;
    QHBoxLayout *btnLayout;
    QList<QUuid> availableCourses;
    QHash<QUuid, QPushButton *> courseCheckBoxes;

    void populateCourses();
};

#endif //AAIMS_SELECTCOURSEDIALOG_H
