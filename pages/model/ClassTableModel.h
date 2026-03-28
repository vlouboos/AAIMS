// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSTABLEMODEL_H
#define AAIMS_CLASSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QPointer>

#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"
#include "../../utils/DataStructures.h"

class ClassTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Name,
        Grade,
        Department,
        MemberCount,
        Master,
        Actions
    };

    explicit ClassTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setClasss(const QList<QUuid> &newData) {
        beginResetModel();
        classes = newData;
        endResetModel();
    }

    [[nodiscard]] int rowCount([[maybe_unused]] const QModelIndex &parent) const override {
        return classes.size(); // NOLINT
    }

    [[nodiscard]] int columnCount([[maybe_unused]] const QModelIndex &parent) const override {
        return 5;
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= classes.size()) return {};

        Classes *cls = aaims::manager::classes::get_classes()[classes[index.row()]].get();

        if (!cls) return {};

        const TeacherAccount *master = aaims::manager::account::get_teachers()[cls->master];
        QString m = master ? master->name : "错误";

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Name: return cls->name;
                case Grade: return cls->grade;
                case Department: return cls->department;
                case MemberCount: return cls->students.size();
                case Master: return m;
                default: return {};
            }
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return {};
    }

    [[nodiscard]] QVariant
    headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            static const QStringList headers = {"班名", "年级", "所属院系", "人数", "班主任", "操作"};
            return headers[section];
        }
        return {};
    }

    QUuid getClass(const QModelIndex &index) {
        return classes[index.row()];
    }

private:
    QList<QUuid> classes;
};

#endif //AAIMS_CLASSTABLEMODEL_H
