// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_SINGLEGRADETABLEMODEL_H
#define AAIMS_SINGLEGRADETABLEMODEL_H

#include "../../managements/RatingManager.h"

#include <QColor>

#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"
#include "../../utils/DataStructures.h"

class SingleGradeTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Id,
        Name,
        Performance,
        Score,
        TotalScore,
        Actions
    };

    explicit SingleGradeTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
    }

    void setStudents(const QList<QUuid> &newData, const QUuid &c) {
        beginResetModel();
        students = newData;
        course = c;
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

        const StudentAccount *student = aaims::manager::account::get_students()[students[index.row()]];

        if (!student) return {};

        const auto rating = aaims::manager::rating::get_ratings()[student->uuid];

        double performance = 0, score = 0, totalScore = 0;
        if (rating) {
            if (rating->ratings.contains(course)) {
                const auto r = rating->ratings[course];
                performance = r.performance;
                score = r.score;
                totalScore = r.finalScore;
            }
        }
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Id: return student->username;
                case Name: return student->name;
                case Performance: return performance;
                case Score: return score;
                case TotalScore: return totalScore;
                default: return {};
            }
        }

        if (role == Qt::ForegroundRole && index.column() == Actions) {
            return student->is_suspended() ? QColor(0x2563eb) : QColor(0x64748b);
        }

        if (role == Qt::TextAlignmentRole) {
            return static_cast<QVariant>(Qt::AlignVCenter | Qt::AlignLeft);
        }

        return {};
    }

    [[nodiscard]] QVariant
    headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            static const QStringList headers = {"学号", "姓名", "平时分", "期末分", "总成绩", "操作"};
            return headers[section];
        }
        return {};
    }

    QUuid getStudent(const QModelIndex &index) {
        return students[index.row()];
    }

private:
    QList<QUuid> students;
    QUuid course;
};

#endif //AAIMS_SINGLEGRADETABLEMODEL_H
