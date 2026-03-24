// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherOperationDelegate.h"

TeacherOperationDelegate::TeacherOperationDelegate(QObject *parent) : QStyledItemDelegate(parent) {
    renderers[0] = new QSvgRenderer(QString(":/assets/edit.svg"), this);
    renderers[1] = new QSvgRenderer(QString(":/assets/delete.svg"), this);
}

void TeacherOperationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
    const int x = option.rect.x() + (option.rect.width() - 60) / 2;
    const int y = option.rect.y() + (option.rect.height() - 24) / 2;

    drawIconContainer(painter, QRect(x, y, 24, 24), QColor(0x107C10), renderers[0]);
    drawIconContainer(painter, QRect(x + 36, y, 24, 24), QColor(0xD83B01), renderers[1]);
}

void TeacherOperationDelegate::drawIconContainer(QPainter *painter, const QRect &rect, const QColor &color,
                                                 QSvgRenderer *renderer) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QColor bgColor = color;
    bgColor.setAlpha(40);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 3, 3);

    constexpr int iconSize = 24;
    const QRect iconRect(
        rect.x() + (rect.width() - iconSize) / 2,
        rect.y() + (rect.height() - iconSize) / 2,
        iconSize, iconSize
    );
    renderer->render(painter, iconRect);

    painter->restore();
}

bool TeacherOperationDelegate::editorEvent(QEvent *event, [[maybe_unused]] QAbstractItemModel *model,
                                           const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonPress) {
        const int x = option.rect.x() + (option.rect.width() - 60) / 2;
        const int y = option.rect.y() + (option.rect.height() - 24) / 2;
        if (const QMouseEvent *me = dynamic_cast<QMouseEvent *>(event); QRect(x, y, 24, 24).contains(me->pos())) emit
            detailClicked(index);
        else if (QRect(x + 36, y, 24, 24).contains(me->pos())) emit deleteClicked(index);

        return true;
    }
    return false;
}
