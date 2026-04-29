// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GRADEDELEGATE_H
#define AAIMS_GRADEDELEGATE_H
#include <QStyledItemDelegate>


class GradeDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit GradeDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

signals:
    void edit(const QModelIndex &index);
};


#endif //AAIMS_GRADEDELEGATE_H
