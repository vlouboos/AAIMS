// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StatusBadgeDelegate.h"

void StatusBadgeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
    QString status = index.data(Qt::DisplayRole).toString();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRect badgeRect = option.rect.adjusted(12, 8, -12, -8);

    QColor bgColor, textColor;
    if (status == "Active") {
        bgColor = QColor(0xdcfce7);
        textColor = QColor(0x166534);
    } else {
        bgColor = QColor(0xf1f5f9);
        textColor = QColor(0x475569);
    }

    painter->setBrush(bgColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(badgeRect, 12, 12);

    painter->setPen(textColor);
    QFont font = painter->font();
    font.setWeight(QFont::Bold);
    font.setPixelSize(11);
    painter->setFont(font);

    painter->drawText(badgeRect, Qt::AlignCenter, status);
    painter->restore();
}
