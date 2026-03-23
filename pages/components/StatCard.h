// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STATCARD_H
#define AAIMS_STATCARD_H

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QHBoxLayout>


class StatCard : public QFrame {
    Q_OBJECT

public:
    StatCard(const QString &title, long long value, const QString &iconPath, const QColor &themeColor,
             QWidget *parent = nullptr);

private:
    QGraphicsDropShadowEffect *shadow;
    QHBoxLayout *mainLayout;
    QWidget * iconContainer;
    QLabel * iconLabel;
    QVBoxLayout * iconLayout;
    QWidget * textContainer;
    QVBoxLayout * textLayout;
    QLabel *titleLabel;
    QLabel *valueLabel;
};


#endif //AAIMS_STATCARD_H
