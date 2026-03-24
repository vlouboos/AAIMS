// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GUI_TEACHEROPERATIONDELEGATE_H
#define AAIMS_GUI_TEACHEROPERATIONDELEGATE_H

#include <QStyledItemDelegate>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QMouseEvent>

class TeacherOperationDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit TeacherOperationDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

signals:
    void openEdit(const QModelIndex &index);

    void confirmDelete(const QModelIndex & index);

private:
    QSvgRenderer *renderers[2] = {};

    static void drawIconContainer(QPainter *painter, const QRect &rect, const QColor &color, QSvgRenderer *renderer);
};

#endif // AAIMS_GUI_TEACHEROPERATIONDELEGATE_H
