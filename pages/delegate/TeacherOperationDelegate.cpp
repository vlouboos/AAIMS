// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherOperationDelegate.h"

TeacherOperationDelegate::TeacherOperationDelegate(QObject *parent) : QStyledItemDelegate(parent) {
    renderers[0] = new QSvgRenderer(QString(":/assets/detail.svg"), this);
    renderers[1] = new QSvgRenderer(QString(":/assets/edit.svg"), this);
    renderers[2] = new QSvgRenderer(QString(":/assets/delete.svg"), this);
}

void TeacherOperationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
    constexpr int s = 48;
    constexpr int g = 12;
    constexpr int totalW = s * 3 + g * 2;

    const int x = option.rect.x() + (option.rect.width() - totalW) / 2;
    const int y = option.rect.y() + (option.rect.height() - s) / 2;

    drawIconContainer(painter, QRect(x, y, s, s), QColor(0x0078D4), renderers[0]);
    drawIconContainer(painter, QRect(x + s + g, y, s, s), QColor(0x107C10), renderers[1]);
    drawIconContainer(painter, QRect(x + (s + g) * 2, y, s, s), QColor(0xD83B01), renderers[2]);
}

void TeacherOperationDelegate::drawIconContainer(QPainter *painter, const QRect &rect, const QColor &color, QSvgRenderer *renderer) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QColor bgColor = color;
    bgColor.setAlpha(40);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 12, 12);

    constexpr int iconSize = 24;
    const QRect iconRect(
        rect.x() + (rect.width() - iconSize) / 2,
        rect.y() + (rect.height() - iconSize) / 2,
        iconSize, iconSize
    );
    renderer->render(painter, iconRect);

    painter->restore();
}

bool TeacherOperationDelegate::editorEvent(QEvent *event, [[maybe_unused]] QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonPress) {
        const QMouseEvent *me = dynamic_cast<QMouseEvent*>(event);
        const int s = 16, g = 8;
        const int x = option.rect.x() + (option.rect.width() - (s * 3 + g * 2)) / 2;
        const int y = option.rect.y() + (option.rect.height() - s) / 2;

        if (QRect(x, y, s, s).contains(me->pos())) emit detailClicked(index);
        else if (QRect(x + s + g, y, s, s).contains(me->pos())) emit editClicked(index);
        else if (QRect(x + (s + g) * 2, y, s, s).contains(me->pos())) emit deleteClicked(index);

        return true;
    }
    return false;
}