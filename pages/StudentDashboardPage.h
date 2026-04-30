// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STUDENTDASHBOARDPAGE_H
#define AAIMS_STUDENTDASHBOARDPAGE_H

#include <QPushButton>  // For ad button

#include "components/DistributionRing.h"
#include "components/StatCard.h"

class StudentDashboardPage : public QWidget {
    Q_OBJECT

public:
    explicit StudentDashboardPage(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QLabel *welcomeLabel;
    QGridLayout *statsGrid;
    DistributionRing *distRing;
    QFrame *distributionContainer;
    QVBoxLayout *distributionLayout;
    QLabel *titleLabel;
    QHBoxLayout *centerLayout;
    StatCard *cardTotal;
    QFrame * adBanner;
    QHBoxLayout *adLayout;
    QLabel *adTextLabel;
    QPushButton *adButton;
};


#endif //AAIMS_STUDENTDASHBOARDPAGE_H