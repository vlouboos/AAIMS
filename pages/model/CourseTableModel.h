// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_COURSETABLEMODEL_H
#define AAIMS_COURSETABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>

#include "../../managements/CourseManager.h"
#include "../../utils/DataStructures.h"

using namespace aaims::model;

class CourseTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Status
    };

    explicit CourseTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setCourses(const QList<QUuid> &newData) {
        beginResetModel();
        courses = newData;
        endResetModel();
    }

    [[nodiscard]] int rowCount([[maybe_unused]] const QModelIndex &parent) const override {
        return courses.size(); // NOLINT
    }

    [[nodiscard]] int columnCount([[maybe_unused]] const QModelIndex &parent) const override {
        return 6;
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= courses.size()) return {};

        const Course *course = aaims::manager::course::get_courses()[courses[index.row()]].get();

        if (!course) return {};

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                default: return {};
            }
        }

        if (role == Qt::ForegroundRole && index.column() == Status) {
            return course->is_accepting() || course->is_qualifying() || course->is_ended() ? QColor(0x2563eb) : QColor(0x64748b);
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return {};
    }

    QUuid getCourse(const QModelIndex &index) {
        return courses[index.row()];
    }

private:
    QList<QUuid> courses;
};

#endif //AAIMS_COURSETABLEMODEL_H
