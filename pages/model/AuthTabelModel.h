// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_AUTHTABELMODEL_H
#define AAIMS_AUTHTABELMODEL_H

#include "../../managements/AccountManager.h"
#include "../../utils/DataStructures.h"

class AuthTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Id,
        Name,
        Role,
        Banned,
        Actions
    };

    explicit AuthTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setAccounts(const QList<QUuid> &newData) {
        beginResetModel();
        accounts = newData;
        endResetModel();
    }

    [[nodiscard]] int rowCount([[maybe_unused]] const QModelIndex &parent) const override {
        return accounts.size(); // NOLINT
    }

    [[nodiscard]] int columnCount([[maybe_unused]] const QModelIndex &parent) const override {
        return 5;
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= accounts.size()) return {};

        Account *account = aaims::manager::account::all()[accounts[index.row()]].get();

        if (!account) return {};

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Id: return account->username;
                case Name: return account->name;
                case Role: return account->is_master() ? "主管理员" : account->is_admin() ? "管理员" : account->is_teacher() ? "教师" : "学生";
                case Banned: return "否"; // Not planned
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
            static const QStringList headers = {"用户名", "姓名", "身份", "是否封禁", "操作"};
            return headers[section];
        }
        return {};
    }

    QUuid getAccount(const QModelIndex &index) {
        return accounts[index.row()];
    }

private:
    QList<QUuid> accounts;
};

#endif //AAIMS_AUTHTABELMODEL_H
