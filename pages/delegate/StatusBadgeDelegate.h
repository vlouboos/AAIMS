// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STATUSBADGEDELEGATE_H
#define AAIMS_STATUSBADGEDELEGATE_H

#pragma once
#include <QStyledItemDelegate>
#include <QPainter>

class StatusBadgeDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};


#endif //AAIMS_STATUSBADGEDELEGATE_H
