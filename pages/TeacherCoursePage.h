// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TeacherCoursePage_H
#define AAIMS_TeacherCoursePage_H

#include "components/TimetableWidget.h"

class TeacherCoursePage : public QWidget {
    Q_OBJECT

public:
    explicit TeacherCoursePage(QWidget *parent = nullptr);

    void reload() const;

private:
    QVBoxLayout *mainLayout;
    TimetableWidget *timetable;
};

#endif //AAIMS_TeacherCoursePage_H
