// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TEACHERTABLEMODEL_H
#define AAIMS_TEACHERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QVector>

#include "../../utils/DataStructures.h"

class TeacherTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Name = 0,
        Department,
        Lessons,
        Status,
        Actions,
        ColumnCount
    };

    explicit TeacherTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setTeachers(const QList<aaims::model::TeacherAccount *> &newData) {
        beginResetModel();
        teachers = newData;
        endResetModel();
    }

    int rowCount([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override {
        return teachers.size();
    }

    int columnCount([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override {
        return ColumnCount;
    }

    QVariant data(const QModelIndex &index, const int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= teachers.size()) return {};

        auto *t = teachers[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Name: return t->name;
                case Department: return t->department;
                case Lessons: return t->teachingClasses.isEmpty()
                                         ? "暂无授课"
                                         : QString("%1 门课程").arg(t->teachingClasses.size());
                case Status: return t->is_teaching() ? "授课中" : "空闲";
                default: return {};
            }
        }

        // 辅助：给状态列加点颜色提示
        if (role == Qt::ForegroundRole && index.column() == Status) {
            return t->is_teaching() ? QColor(0x2563eb) : QColor(0x64748b);
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return QVariant();
    }

    QVariant headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            static const QStringList headers = {"姓名", "院系", "授课情况", "状态", "操作"};
            return headers[section];
        }
        return QVariant();
    }

    aaims::model::TeacherAccount *getAccount(const QModelIndex &index) {
        return teachers[index.row()];
    }

private:
    QList<aaims::model::TeacherAccount *> teachers;
};


#endif //AAIMS_TEACHERTABLEMODEL_H
