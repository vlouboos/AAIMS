// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_COURSETABLEMODEL_H
#define AAIMS_COURSETABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>

#include "../../managements/CourseManager.h"
#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"
#include "../../utils/DataStructures.h"

using namespace aaims::model;

class CourseTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Number,
        Name,
        Teacher,
        Size,
        Credit,
        Status,
        Actions
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
        return 7;
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, const int role) const override {
        if (!index.isValid() || index.row() >= courses.size()) return {};

        const Course *course = aaims::manager::course::get_courses()[courses[index.row()]].get();

        if (!course) return {};

        const TeacherAccount *const t = aaims::manager::account::get_teachers()[course->teacher];
        unsigned long long size = course->students.size();
        for (const auto x: course->classes) {
            const Class *const cls = aaims::manager::classes::get_classes()[x].get();
            size += cls->students.size();
        }
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Number: return course->id;
                case Name: return course->name;
                case Teacher: return QString("%1(%2)").arg(t->name, t->department);
                case Size: return QString("%1(%2名重修/自选+%3个班级)").arg(size).arg(course->students.size())
                            .arg(course->classes.size());
                case Credit: return course->credit;
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
            static const QStringList headers = {"课程编号", "课程名", "教师", "人数", "学分", "状态", "操作"};
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

#endif //AAIMS_COURSETABLEMODEL_H
