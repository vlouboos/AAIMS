// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AdminDashboardPage.h"

#include "../managements/AccountManager.h"
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

    const long long total = account::all().size();
    const long long adminCount = account::get_admins().size();
    const long long teacherCount = account::get_teachers().size();
    const long long studentCount = account::get_working_students().size();

    auto *cardTotal = new StatCard("总账号数", total, ":/assets/users.svg", QColor(0x6366f1));
    auto *cardAdmin = new StatCard("管理员", adminCount, ":/assets/shield.svg", QColor(0xf59e0b));
    auto *cardTeacher = new StatCard("教师", teacherCount, ":/assets/briefcase.svg", QColor(0x10b981));
    auto *cardStudent = new StatCard("在校学生", studentCount, ":/assets/graduation-cap.svg", QColor(0x3b82f6));
    auto *cardClasses = new StatCard("班级数量", studentCount, ":/assets/classes.svg", QColor(0x3b82f6));

    statsGrid->addWidget(cardTotal, 0, 0);
    statsGrid->addWidget(cardAdmin, 0, 1);
    statsGrid->addWidget(cardTeacher, 0, 2);
    statsGrid->addWidget(cardStudent, 0, 3);
    statsGrid->addWidget(cardClasses, 1, 0);

    distributionContainer = new QFrame(this);
    distributionContainer->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 20px;");

    distributionLayout = new QVBoxLayout(distributionContainer);
    distributionLayout->setSpacing(15);

    titleLabel = new QLabel("账号角色分布概览", distributionContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1e293b;");

    distributionLayout->addWidget(titleLabel);

    distRing = new DistributionRing(distributionContainer);

    const QList<DistributionItem> data = {
        {"在校学生", account::get_working_students().size(), QColor(0x3b82f6)},
        {"教师", account::get_teachers().size(), QColor(0x10b981)},
        {"管理员", account::get_admins().size(), QColor(0xf59e0b)}
    };
    distRing->setData(data);

    centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(distRing);
    centerLayout->addStretch();

    distributionLayout->addLayout(centerLayout);

    distributionLayout->addWidget(distRing);

    mainLayout->addLayout(statsGrid);
    mainLayout->addWidget(distributionContainer);

    mainLayout->addStretch();
}
