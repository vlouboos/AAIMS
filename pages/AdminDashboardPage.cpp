// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AdminDashboardPage.h"

#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../utils/DataStructures.h"
#include "components/StatCard.h"

using namespace aaims::manager;

AdminDashboardPage::AdminDashboardPage(QWidget *parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(24);

    welcomeLabel = new QLabel("欢迎回来, " + account::logged->name, this);
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #1e293b;");
    mainLayout->addWidget(welcomeLabel);

    statsGrid = new QGridLayout();
    statsGrid->setSpacing(16);

    cardTotal = new StatCard("总账号数", 0, ":/assets/users.svg", QColor(0x6366f1));
    cardAdmin = new StatCard("管理员", 0, ":/assets/shield.svg", QColor(0xf59e0b));
    cardTeacher = new StatCard("教师", 0, ":/assets/briefcase.svg", QColor(0x10b981));
    cardStudent = new StatCard("在校学生", 0, ":/assets/graduation-cap.svg", QColor(0x3b82f6));
    cardDepartments = new StatCard("院系数量", 0, ":/assets/briefcase.svg", QColor(0xf59e0b));
    cardClasses = new StatCard("班级数量", 0, ":/assets/classes.svg", QColor(0x10b981));
    cardMajors = new StatCard("专业数量", 0, ":/assets/graduation-cap.svg", QColor(0x3b82f6));

    statsGrid->addWidget(cardTotal, 0, 0);
    statsGrid->addWidget(cardAdmin, 0, 1);
    statsGrid->addWidget(cardTeacher, 0, 2);
    statsGrid->addWidget(cardStudent, 0, 3);
    statsGrid->addWidget(cardDepartments, 1, 0);
    statsGrid->addWidget(cardClasses, 1, 1);
    statsGrid->addWidget(cardMajors, 1, 2);

    distributionContainer = new QFrame(this);
    distributionContainer->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 20px;");

    distributionLayout = new QVBoxLayout(distributionContainer);
    distributionLayout->setSpacing(15);

    titleLabel = new QLabel("账号角色分布概览", distributionContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1e293b;");

    distributionLayout->addWidget(titleLabel);

    distRing = new DistributionRing("个人账号数", distributionContainer);

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

void AdminDashboardPage::update() const {
    const long long total = account::all().size();
    const long long adminCount = account::get_admins().size();
    const long long teacherCount = account::get_teachers().size();
    const long long studentCount = account::get_working_students().size();
    const long long departmentsCount = classes::get_departments().size();
    const long long classesCount = classes::get_departments().size();
    const long long majorsCount = classes::get_majors().size();
    cardTotal->setValue(QString::number(total));
    cardAdmin->setValue(QString::number(adminCount));
    cardTeacher->setValue(QString::number(teacherCount));
    cardStudent->setValue(QString::number(studentCount));
    cardDepartments->setValue(QString::number(departmentsCount));
    cardClasses->setValue(QString::number(classesCount));
    cardMajors->setValue(QString::number(majorsCount));
    const QList<DistributionItem> data = {
        {"在校学生", studentCount, QColor(0x3b82f6)},
        {"教师", teacherCount, QColor(0x10b981)},
        {"管理员", adminCount, QColor(0xf59e0b)}
    };
    distRing->setData(data);
}
