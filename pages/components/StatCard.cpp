// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StatCard.h"

StatCard::StatCard(const QString &title, const long long value, const QString &iconPath, const QColor &themeColor,
                   QWidget *parent) : QFrame(parent) {
    shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);
    setFixedHeight(100);

    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 0, 20, 0);
    mainLayout->setSpacing(15);

    const QString containerStyle = QString(
        "background-color: rgba(%1, %2, %3, 40); border-radius: 12px;"
    ).arg(themeColor.red()).arg(themeColor.green()).arg(themeColor.blue());
    iconContainer = new QWidget(this);
    iconContainer->setFixedSize(48, 48);
    iconContainer->setStyleSheet(containerStyle);

    iconLayout = new QVBoxLayout(iconContainer);

    const QIcon icon(iconPath);
    iconLabel = new QLabel(iconContainer);
    iconLabel->setPixmap(icon.pixmap(24, 24));

    iconLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    iconLayout->setContentsMargins(2, 2, 2, 2);

    textContainer = new QWidget(this);

    textLayout = new QVBoxLayout(textContainer);
    textLayout->setSpacing(2);
    textLayout->setContentsMargins(0, 20, 0, 20);

    titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("color: #64748b; font-size: 13px; font-weight: 500;");

    valueLabel = new QLabel(QString::number(value), this);
    valueLabel->setStyleSheet("color: #0f172a; font-size: 26px; font-weight: 700; font-family: 'Inter', sans-serif;");

    textLayout->addWidget(titleLabel);
    textLayout->addWidget(valueLabel);

    mainLayout->addWidget(iconContainer);
    mainLayout->addWidget(textContainer);
    mainLayout->addStretch();
}
