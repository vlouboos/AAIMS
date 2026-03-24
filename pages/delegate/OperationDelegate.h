// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GUI_TEACHEROPERATIONDELEGATE_H
#define AAIMS_GUI_TEACHEROPERATIONDELEGATE_H

#include <QElapsedTimer>
#include <QStyledItemDelegate>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QMouseEvent>

class OperationDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit OperationDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

signals:
    void openEdit(const QModelIndex &index);

    void confirmDelete(const QModelIndex &index);

private:
    QSvgRenderer *renderers[2] = {};
    QElapsedTimer clickTimer;
    QModelIndex prevIndex;

    static void drawIconContainer(QPainter *painter, const QRect &rect, const QColor &color, QSvgRenderer *renderer);
};

#endif // AAIMS_GUI_TEACHEROPERATIONDELEGATE_H
