// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StudentCoursePage.h"

#include <QHBoxLayout>

StudentCoursePage::StudentCoursePage(QWidget *parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);

    timetable = new TimetableWidget(this);
    timetable->setUserType(TimetableWidget::STUDENT);

    mainLayout->addWidget(timetable);
}

void StudentCoursePage::reload() const {
    timetable->reload();
}