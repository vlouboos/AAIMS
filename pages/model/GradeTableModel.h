// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GRADETABLEMODEL_H
#define AAIMS_GRADETABLEMODEL_H

#include <QColor>

#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"
#include "../../managements/CourseManager.h"
#include "../../utils/DataStructures.h"

class GradeTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Number,
        Name,
        Size,
        Status,
        Actions
    };

    explicit GradeTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
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
        return 5;
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= courses.size()) return {};

        const Course *course = aaims::manager::course::get_courses()[courses[index.row()]].get();

        if (!course) return {};

        unsigned long long size = course->students.size();
        for (const auto x: course->classes) {
            const Class *const cls = aaims::manager::classes::get_classes()[x].get();
            size += cls->students.size();
        }
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Number: return course->id;
                case Name: return course->name;
                case Size: return QString("%1(%2名重修/自选+%3个班级)").arg(size).arg(course->students.size())
                            .arg(course->classes.size());
                case Status: return course->is_accepting()
                                        ? "选课中"
                                        : course->is_qualifying()
                                              ? "筛选中"
                                              : course->is_started()
                                                    ? "上课中"
                                                    : "结课";
                default: return {};
            }
        }

        if (role == Qt::ForegroundRole && index.column() == Actions) {
            return course->is_accepting() || course->is_qualifying() || course->is_ended()
                       ? QColor(0x2563eb)
                       : QColor(0x64748b);
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return {};
    }

    [[nodiscard]] QVariant
    headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            static const QStringList headers = {"课程编号", "课程名", "人数", "状态", "操作"};
            return headers[section];
        }
        return {};
    }

    QUuid getCourse(const QModelIndex &index) {
        return courses[index.row()];
    }

private:
    QList<QUuid> courses;
};

#endif //AAIMS_GRADETABLEMODEL_H
