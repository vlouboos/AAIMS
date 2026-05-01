// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TIMETABLEWIDGET_H
#define AAIMS_TIMETABLEWIDGET_H

#pragma once
#include <QBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include "../../utils/DataStructures.h"

struct CourseColor {
    QColor bg;
    QColor border;
    QColor text;
};

class TimetableCanvas : public QWidget {
    Q_OBJECT

public:
    explicit TimetableCanvas(QWidget *parent = nullptr);

    void setCourses(const QList<QUuid> &courses);

    void setCurrentWeek(int week);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int CELL_HEIGHT = 120;
    static constexpr int CELL_WIDTH = 120;
    static constexpr int TIME_COLUMN_WIDTH = 80;
    static constexpr int HEADER_HEIGHT = 48;
    static constexpr int TOTAL_PERIODS = 15;
    int currentWeek = 0;
    QList<QUuid> allCourses;
    QVector<CourseColor> colors;

    [[nodiscard]] long long getColorIndex(const QString &courseId) const;
};

class TimetableWidget : public QWidget {
    Q_OBJECT

public:
    enum UserType {
        TEACHER,
        STUDENT
    };

    void reload();

    void setUserType(const UserType &type);

    explicit TimetableWidget(QWidget *parent = nullptr);

private slots:
    void onPrevWeek();

    void onNextWeek();

    void onQualify();

private:
    static constexpr int TOTAL_WEEKS = 20;
    QVBoxLayout *mainLayout;
    QWidget *topBar;
    QHBoxLayout *topLayout;
    QLabel *labelSemester;
    QPushButton *btnPrev;
    QLabel *labelWeek;
    QPushButton *btnNext;
    QPushButton *btnQualify;
    TimetableCanvas *canvas;
    QScrollArea *scrollArea;
    int currentWeek = 0;
    QList<QUuid> courses, qualifyingCourses;
    QString semester;
    UserType userType;
};


#endif //AAIMS_TIMETABLEWIDGET_H
