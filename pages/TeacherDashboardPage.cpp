// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherDashboardPage.h"

#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"

using namespace aaims::manager;

TeacherDashboardPage::TeacherDashboardPage(QWidget *parent) : QWidget(parent) {
    const auto *const loggedTeacher = dynamic_cast<TeacherAccount *>(account::logged);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(24);

    welcomeLabel = new QLabel("欢迎回来, " + loggedTeacher->name, this);
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #1e293b;");
    mainLayout->addWidget(welcomeLabel);

    statsGrid = new QGridLayout();
    statsGrid->setSpacing(16);

    cardTotal = new StatCard("总课程数", 0, ":/assets/users.svg", QColor(0x6366f1));
    cardQualifying = new StatCard("待筛选课程", 0, ":/assets/classes.svg", QColor(0xf59e0b));
    cardStarted = new StatCard("已开课程数", 0, ":/assets/classes.svg", QColor(0x10b981));
    cardEnded = new StatCard("已结束课程", 0, ":/assets/classes.svg", QColor(0x3b82f6));

    statsGrid->addWidget(cardTotal, 0, 0);
    statsGrid->addWidget(cardQualifying, 0, 1);
    statsGrid->addWidget(cardStarted, 0, 2);
    statsGrid->addWidget(cardEnded, 0, 3);

    distributionContainer = new QFrame(this);
    distributionContainer->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 20px;");

    distributionLayout = new QVBoxLayout(distributionContainer);
    distributionLayout->setSpacing(15);

    titleLabel = new QLabel("课程分布概览", distributionContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1e293b;");

    distributionLayout->addWidget(titleLabel);

    distRing = new DistributionRing("个人课程数", distributionContainer);

    centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(distRing);
    centerLayout->addStretch();

    distributionLayout->addLayout(centerLayout);

    distributionLayout->addWidget(distRing);

    mainLayout->addLayout(statsGrid);
    mainLayout->addWidget(distributionContainer);

    mainLayout->addStretch();

    update();
}

void TeacherDashboardPage::update() const {
    const TeacherAccount *teacher = account::get_teachers()[account::logged->uuid];
    QList<QUuid> courses = teacher->courses;
    const long long total = courses.count();
    const long long qualifying = courses.removeIf([](const QUuid &courseId) {
        const auto &course = course::get_courses()[courseId];
        return course->is_qualifying();
    });
    const long long started = courses.removeIf([](const QUuid &courseId) {
        const auto &course = course::get_courses()[courseId];
        return course->is_started();
    });
    const long long ended = courses.removeIf([](const QUuid &courseId) {
        const auto &course = course::get_courses()[courseId];
        return course->is_ended();
    });
    cardTotal->setValue(QString::number(total));
    cardQualifying->setValue(QString::number(qualifying));
    cardStarted->setValue(QString::number(started));
    cardEnded->setValue(QString::number(ended));

    const QList<DistributionItem> data = {
        {"筛选中", qualifying, QColor(0x3b82f6)},
        {"已开课", started, QColor(0x10b981)},
        {"已结束", ended, QColor(0xf59e0b)}
    };
    distRing->setData(data);
}
