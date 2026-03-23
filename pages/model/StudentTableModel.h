// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ACCOUNTTABLEMODEL_H
#define AAIMS_ACCOUNTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "../../utils/DataStructures.h"
#include "../../managements/AccountManager.h"

class StudentTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Avatar = 0,
        Actions,
        Name,
        ID,
        Status,
    };

    int rowCount([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override {
        return accounts.size();
    }

    int columnCount([[maybe_unused]] const QModelIndex &parent = QModelIndex()) const override {
        return 4;
    }

    QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= accounts.size())
            return QVariant();

        auto *acc = accounts[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case 1: return acc->name;
                case 2: return acc->username;
                case 3: return acc->is_admin() ? "管理员" : (acc->is_teacher() ? "教师" : "学生");
                case 4: return acc->is_graduated() ? "Inactive" : "Active";
                default: return QVariant();
            }
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return QVariant();
    }

    QVariant headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            QStringList headers = {"操作", "姓名", "学号", "状态"};
            return headers[section];
        }
        return QVariant();
    }

    void refresh() {
        beginResetModel();
        accounts = aaims::manager::account::get_all_ptrs();
        endResetModel();
    }

private:
    QVector<Account *> accounts;
};

#endif //AAIMS_ACCOUNTTABLEMODEL_H
