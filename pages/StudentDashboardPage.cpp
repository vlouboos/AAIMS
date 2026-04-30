// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StudentDashboardPage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include "../managements/AccountManager.h"
#include "../utils/DataStructures.h"

StudentDashboardPage::StudentDashboardPage(QWidget *parent) : QWidget(parent) {
    const auto *const loggedStudent = dynamic_cast<StudentAccount *>(aaims::manager::account::logged);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(24);

    welcomeLabel = new QLabel("欢迎回来, " + loggedStudent->name, this);
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #1e293b;");
    mainLayout->addWidget(welcomeLabel);

    statsGrid = new QGridLayout();
    statsGrid->setSpacing(16);

    cardTotal = new StatCard("总课程数", 0, ":/assets/classes.svg", QColor(0x6366f1));

    statsGrid->addWidget(cardTotal, 0, 0);

    adBanner = new QFrame(this);
    adBanner->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3b82f6, stop:1 #60a5fa); "
        "border-radius: 12px; height: 100px;");
    adBanner->setFixedHeight(100);

    adLayout = new QHBoxLayout(adBanner);
    adLayout->setContentsMargins(20, 10, 20, 10);

    adTextLabel = new QLabel(adBanner);
    adTextLabel->setText("🎓 提升您的学习体验！探索我们的高级学习资源平台");
    adTextLabel->setStyleSheet("color: white; font-size: 16px; font-weight: 500;");
    adTextLabel->setWordWrap(true);

    adButton = new QPushButton("了解更多", adBanner);
    adButton->setObjectName("LoginBtn");
    adButton->setStyleSheet(
        "background: black; color: white; border: 2px solid black; border-radius: 8px; padding: 0;");
    adButton->setFixedSize(100, 40);

    adLayout->addWidget(adTextLabel);
    adLayout->addWidget(adButton);

    distributionContainer = new QFrame(this);
    distributionContainer->setStyleSheet(
        "background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 20px;");

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
    mainLayout->addWidget(adBanner); // 添加广告横幅
    mainLayout->addWidget(distributionContainer);

    mainLayout->addStretch();

    connect(adButton, &QPushButton::clicked, this, [this] {
        QMessageBox::information(this, "了解更多", "从前有座山，山里有座庙，庙里有个老和尚，老和尚在给小和尚讲故事：从前有座山，山里有座庙，庙里有个老和尚，老和尚在给小和尚讲故...从前有座山，山里有座庙，庙里有个老和尚，老和尚在给小和尚讲...从前有座山，山里有座庙，庙里有个老和尚，老和尚在给小和尚...从前有座山，山里有座庙，庙里有个老和尚，老和尚在给小和...从前有座山，山里有座庙，庙里有个老和尚，老和尚在给小...从前有座山，山里有座庙，庙里有个老和尚，老和尚在给...从前有座山，山里有座庙，庙里有个老和尚，老和尚在...从前有座山，山里有座庙，庙里有个老和尚，老和尚...从前有座山，山里有座庙，庙里有个老和尚，老和...从前有座山，山里有座庙，庙里有个老和尚，老...从前有座山，山里有座庙，庙里有个老和尚...从前有座山，山里有座庙，庙里有个老和...从前有座山，山里有座庙，庙里有个老...从前有座山，山里有座庙，庙里有个...从前有座山，山里有座庙，庙里有...从前有座山，山里有座庙，庙里...从前有座山，山里有座庙，庙...从前有座山，山里有座庙...从前有座山，山里有座...从前有座山，山里有...从前有座山，山里...从前有座山，山...从前有座山...从前有座...从前有...从前...从...");
    });
}
