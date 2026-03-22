// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_DATA_STRUCTURES_H
#define AAIMS_DATA_STRUCTURES_H

#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

#include "AccountLoginDialog.h"

namespace aaims::model {
    struct LessonStatus {
        QUuid uuid;
        int retake = 0;
    };

    struct Class {
        QUuid uuid;
        QString name;
        QList<QUuid> lessons;
    };

    struct Account {
        constexpr static int ADMIN = 0b1;
        constexpr static int TEACHER = 0b10;
        constexpr static int GRADUATED = 0b100;
        constexpr static int MASTER = 0b1000;

        QUuid uuid;
        QString username;
        QString name;
        QString password;
        bool female;
        uint8_t status = 0;
        QList<LessonStatus> lessons;

        [[nodiscard]] bool is_admin() const { return (status & ADMIN) != 0; }

        [[nodiscard]] bool is_teacher() const { return (status & TEACHER) != 0; }

        [[nodiscard]] bool is_graduated() const { return (status & GRADUATED) != 0; }

        [[nodiscard]] bool is_master() const { return (status & MASTER) != 0; }
    };

    struct Lesson {
        struct ClassTime {
            int dayOfWeek;
            int startSection;
            int duration;

            static ClassTime fromJson(const QJsonObject &json) {
                return {json.value("day").toInt(), json.value("start").toInt(), json.value("duration").toInt()};
            }

            QJsonObject toJson() const {
                return {
                    {"day", dayOfWeek},
                    {"start", startSection},
                    {"duration", duration}
                };
            }
        };

        QUuid uuid;
        QString name;
        QString teacher;
        QList<ClassTime> times;

        static Lesson fromJson(const QUuid &uuid, const QJsonObject &json) {
            const QString name = json.value("name").toString();
            const QString teacher = json.value("teacher").toString();
            QList<ClassTime> times;
            for (const auto &t: json.value("times").toArray()) {
                times.emplace_back(ClassTime::fromJson(t.toObject()));
            }
            return {uuid, name, teacher, times};
        }

        QJsonObject toJson() const {
            QJsonArray t;
            for (const auto &x: this->times) { t.append(x.toJson()); }
            return {
                {"name", name},
                {"teacher", teacher},
                {"times", t}
            };
        }
    };

    struct RatingDetail {
        QUuid lesson_uuid;
        double performance = 0.0;
        double score = 0.0;
    };

    struct StudentRating {
        QUuid student_id;
        QList<RatingDetail> ratings;
    };
}
#endif // AAIMS_DATA_STRUCTURES_H
