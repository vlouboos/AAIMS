// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TEACHERDASHBOARDPAGE_H
#define AAIMS_TEACHERDASHBOARDPAGE_H

#include <QWidget>

#include "components/DistributionRing.h"
#include "components/StatCard.h"

class TeacherDashboardPage : public QWidget {
    Q_OBJECT

public:
    explicit TeacherDashboardPage(QWidget *parent = nullptr);

    void update() const;

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
    StatCard *cardStarted;
    StatCard *cardQualifying;
    StatCard *cardEnded;
};

#endif //AAIMS_TEACHERDASHBOARDPAGE_H
