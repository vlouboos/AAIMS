// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSMEMBERTABLEMODEL_H
#define AAIMS_CLASSMEMBERTABLEMODEL_H

#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"
#include "../../utils/DataStructures.h"

class ClassMemberTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Id,
        Name,
        Dormitory,
        PhoneNumber,
        Courses
    };

    explicit ClassMemberTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setClasses(const QList<QUuid> &newData) {
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

        StudentAccount *student = aaims::manager::account::get_students()[classes[index.row()]];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Id: return student->username;
                case Name: return student->name;
                case Dormitory: return student->dormitory;
                case PhoneNumber: return student->phoneNumber;
                case Courses: return student->lessons.size();
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
            static const QStringList headers = {"学号", "姓名", "宿舍", "手机号", "自选/重修课程数"};
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

#endif //AAIMS_CLASSMEMBERTABLEMODEL_H
