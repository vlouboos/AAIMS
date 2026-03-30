// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADMINDASHBOARDPAGE_H
#define AAIMS_ADMINDASHBOARDPAGE_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "components/DistributionRing.h"

class AdminDashboardPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminDashboardPage(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QLabel *welcomeLabel;
    QGridLayout *statsGrid;
    DistributionRing * distRing;
    QFrame * distributionContainer;
    QVBoxLayout * distributionLayout;
    QLabel * titleLabel;
    QHBoxLayout * centerLayout;
};


#endif //AAIMS_ADMINDASHBOARDPAGE_H
