// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherDashboardPage.h"

#include "../managements/AccountManager.h"

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

    const long long total = loggedTeacher->courses.size();

    cardTotal = new StatCard("总课程数", total, ":/assets/users.svg", QColor(0x6366f1));

    statsGrid->addWidget(cardTotal, 0, 0);

    distributionContainer = new QFrame(this);
    distributionContainer->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 20px;");

    distributionLayout = new QVBoxLayout(distributionContainer);
    distributionLayout->setSpacing(15);

    titleLabel = new QLabel("账号角色分布概览", distributionContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1e293b;");

    distributionLayout->addWidget(titleLabel);

    distRing = new DistributionRing(distributionContainer);

    const QList<DistributionItem> data = {
        {"选课中", 0, QColor(0x3b82f6)},
        {"已开课", 0, QColor(0x10b981)},
        {"已结束", 0, QColor(0xf59e0b)}
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
