// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AuthDelegate.h"

#include <QMouseEvent>
#include <QPainter>

AuthDelegate::AuthDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

void AuthDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
    constexpr int btnW = 70, btnH = 20;
    const QRect btnRect(option.rect.center().x() - btnW / 2,
                        option.rect.center().y() - btnH / 2,
                        btnW, btnH);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0x2563eb));
    painter->drawRoundedRect(btnRect, 6, 6);
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setWeight(QFont::DemiBold);
    painter->setFont(font);
    painter->drawText(btnRect, Qt::AlignCenter, "重置密码");
    painter->restore();
}

bool AuthDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                               const QStyleOptionViewItem &option, const QModelIndex &index) {
    constexpr int btnW = 70, btnH = 20;
    const QRect btnRect(option.rect.center().x() - btnW / 2,
                        option.rect.center().y() - btnH / 2,
                        btnW, btnH);
    if (event->type() == QEvent::MouseButtonPress) {
        if (const auto *const mouseEvent = dynamic_cast<QMouseEvent *>(event); btnRect.contains(mouseEvent->pos())) {
            emit confirmReset(index);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
