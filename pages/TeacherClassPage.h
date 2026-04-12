// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TEACHERCLASSPAGE_H
#define AAIMS_TEACHERCLASSPAGE_H

#include <QBoxLayout>

#include "components/TimetableWidget.h"

class TeacherClassPage : public QWidget {
    Q_OBJECT

public:
    explicit TeacherClassPage(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    TimetableWidget * timetable;
};

#endif //AAIMS_TEACHERCLASSPAGE_H
