// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STUDENTTABLEMODEL_H
#define AAIMS_STUDENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>

#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"
#include "../../utils/DataStructures.h"

class StudentTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Username,
        Name,
        Class,
        Dormitory,
        Status,
        Actions
    };

    explicit StudentTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setStudents(const QList<QUuid> &newData) {
        beginResetModel();
        students = newData;
        endResetModel();
    }

    [[nodiscard]] int rowCount([[maybe_unused]] const QModelIndex &parent) const override {
        return students.size(); // NOLINT
    }

    [[nodiscard]] int columnCount([[maybe_unused]] const QModelIndex &parent) const override {
        return 6;
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= students.size()) return {};

        StudentAccount *t = aaims::manager::account::get_students()[students[index.row()]];

        if (!t) return {};

        const auto cls = aaims::manager::classes::get_classes()[t->currentClass].get();

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Username: return t->username;
                case Name: return t->name;
                case Class: return cls->grade + cls->name;
                case Dormitory: return t->dormitory;
                case Status: return t->is_graduated() ? "已毕业" : t->is_suspended() ? "休学" : "在校";
                default: return {};
            }
        }

        if (role == Qt::ForegroundRole && index.column() == Status) {
            return t->is_graduated() || t->is_suspended() ? QColor(0x2563eb) : QColor(0x64748b);
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return {};
    }

    [[nodiscard]] QVariant
    headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            static const QStringList headers = {"学号", "姓名", "班级", "宿舍", "状态", "操作"};
            return headers[section];
        }
        return {};
    }

    QUuid getAccount(const QModelIndex &index) {
        return students[index.row()];
    }

private:
    QList<QUuid> students;
};

#endif //AAIMS_STUDENTTABLEMODEL_H
