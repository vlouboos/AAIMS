// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADMINDASHBOARDPAGE_H
#define AAIMS_ADMINDASHBOARDPAGE_H

#include <QWidget>

#include "components/DistributionRing.h"
#include "components/StatCard.h"

class AdminDashboardPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminDashboardPage(QWidget *parent = nullptr);

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
    StatCard *cardAdmin;
    StatCard *cardTeacher;
    StatCard *cardStudent;
    StatCard *cardDepartments;
    StatCard *cardClasses;
    StatCard *cardMajors;
};


#endif //AAIMS_ADMINDASHBOARDPAGE_H
