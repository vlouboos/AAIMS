// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TimetableWidget.h"
#include <QPainter>
#include <QHBoxLayout>

TimetableCanvas::TimetableCanvas(QWidget *parent) : QWidget(parent) {
    constexpr int totalWidth = TIME_COLUMN_WIDTH + 7 * CELL_WIDTH;
    constexpr int totalHeight = HEADER_HEIGHT + TOTAL_PERIODS * CELL_HEIGHT;
    setMinimumSize(totalWidth, totalHeight);

    colors = {
        {QColor(59, 130, 246, 38), QColor(59, 130, 246), QColor(59, 130, 246)},
        {QColor(16, 185, 129, 38), QColor(16, 185, 129), QColor(16, 185, 129)},
        {QColor(245, 158, 11, 38), QColor(245, 158, 11), QColor(245, 158, 11)},
        {QColor(239, 68, 68, 38),  QColor(239, 68, 68),  QColor(239, 68, 68)}
        // ...
    };
}

void TimetableCanvas::setCourses(const QList<aaims::model::Course>& courses) {
    allCourses = courses;
    update();
}

void TimetableCanvas::setCurrentWeek(const int week) {
    currentWeek = week;
    update();
}

int TimetableCanvas::getColorIndex(const QString& courseId) const {
    return qAbs(static_cast<int>(qHash(courseId))) % colors.size();
}

void TimetableCanvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    QPen borderPen(QColor(0xe5e7eb), 1);
    painter.setPen(borderPen);

    for (int i = 0; i <= 7; ++i) {
        int x = TIME_COLUMN_WIDTH + i * CELL_WIDTH;
        painter.drawLine(x, 0, x, height());
    }
    for (int i = 0; i <= TOTAL_PERIODS; ++i) {
        int y = HEADER_HEIGHT + i * CELL_HEIGHT;
        painter.drawLine(0, y, width(), y);
    }

    painter.fillRect(0, 0, width(), HEADER_HEIGHT, QColor(0xf9fafb));
    painter.setPen(QColor(0x6b7280));
    painter.setFont(QFont("Microsoft YaHei", 9));

    painter.drawText(QRect(0, 0, TIME_COLUMN_WIDTH, HEADER_HEIGHT), Qt::AlignCenter, "节次");

    painter.setPen(QColor(0x111827));
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    for (int i = 0; i < 7; ++i) {
        QRect rect(TIME_COLUMN_WIDTH + i * CELL_WIDTH, 0, CELL_WIDTH, HEADER_HEIGHT);
        painter.drawText(rect, Qt::AlignCenter, aaims::model::Course::LessonTime::DAY_OF_WEEK_TABLE[i]);
    }

    painter.fillRect(0, HEADER_HEIGHT, TIME_COLUMN_WIDTH, height() - HEADER_HEIGHT, QColor(0xf9fafb));
    for (int i = 0; i < TOTAL_PERIODS; ++i) {
        int y = HEADER_HEIGHT + i * CELL_HEIGHT;

        painter.setPen(QColor(0x111827));
        painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
        painter.drawText(QRect(0, y + 45, TIME_COLUMN_WIDTH, 20), Qt::AlignCenter, QString("第%1节").arg(i + 1));

        painter.setPen(QColor(0x9ca3af));
        painter.setFont(QFont("Microsoft YaHei", 8));
        painter.drawText(QRect(0, y + 65, TIME_COLUMN_WIDTH, 20), Qt::AlignCenter, aaims::model::Course::LessonTime::TIME_TABLE[i]);
    }

    for (const auto& course : allCourses) {
        auto [bg, border, text] = colors[getColorIndex(course.id)];

        for (const auto&[weekStart, weekEnd, dayOfWeek, startTime, duration, location] : course.times) {
            if (currentWeek == 0 || (currentWeek >= weekStart && currentWeek <= weekEnd)) {
                int x = TIME_COLUMN_WIDTH + dayOfWeek * CELL_WIDTH;
                int y = HEADER_HEIGHT + startTime * CELL_HEIGHT;
                int w = CELL_WIDTH;
                int h = duration * CELL_HEIGHT;

                QRect blockRect(x + 1, y + 1, w - 2, h - 2);

                painter.fillRect(blockRect, bg);

                painter.fillRect(x + 1, y + 1, 3, h - 2, border);

                painter.setPen(text);
                painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));

                painter.drawText(QRect(x + 8, y + 4, w - 12, h - 8), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, course.name);
                painter.drawText(QRect(x + 8, y + 14, w - 12, h - 8), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, location);
                painter.drawText(QRect(x + 8, y + 24, w - 12, h - 8), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, QString("第%1-%2周").arg(weekStart, weekEnd));
            }
        }
    }
}

TimetableWidget::TimetableWidget(QWidget *parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    topBar = new QWidget(this);
    topBar->setStyleSheet("background-color: white; border-bottom: 1px solid #e5e7eb;");
    topLayout = new QHBoxLayout(topBar);

    btnPrev = new QPushButton("<", this);
    labelWeek = new QLabel("全部", this);
    btnNext = new QPushButton(">", this);

    btnPrev->setFixedSize(32, 32);
    btnNext->setFixedSize(32, 32);
    labelWeek->setMinimumWidth(80);
    labelWeek->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(btnPrev);
    topLayout->addWidget(labelWeek);
    topLayout->addWidget(btnNext);
    topLayout->addStretch();

    scrollArea = new QScrollArea(this);
    canvas = new TimetableCanvas(this);

    scrollArea->setWidget(canvas);
    scrollArea->setWidgetResizable(false);
    scrollArea->setFrameShape(QFrame::NoFrame);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(scrollArea);

    connect(btnPrev, &QPushButton::clicked, this, &TimetableWidget::onPrevWeek);
    connect(btnNext, &QPushButton::clicked, this, &TimetableWidget::onNextWeek);
}

void TimetableWidget::loadCourses(const QList<aaims::model::Course>& courses) const {
    canvas->setCourses(courses);
}

void TimetableWidget::onPrevWeek() {
    if (currentWeek > 0) {
        currentWeek--;
        if (currentWeek == 0) labelWeek->setText("全部");
        else labelWeek->setText(QString("第 %1 周").arg(currentWeek));
        canvas->setCurrentWeek(currentWeek);
        btnNext->setEnabled(true);
    }
    btnPrev->setEnabled(currentWeek > 0);
}

void TimetableWidget::onNextWeek() {
    if (currentWeek < TOTAL_WEEKS) {
        currentWeek++;
        labelWeek->setText(QString("第 %1 周").arg(currentWeek));
        canvas->setCurrentWeek(currentWeek);
        btnPrev->setEnabled(true);
    }
    btnNext->setEnabled(currentWeek < TOTAL_WEEKS);
}