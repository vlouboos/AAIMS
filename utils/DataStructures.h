// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_DATA_STRUCTURES_H
#define AAIMS_DATA_STRUCTURES_H

#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

namespace aaims {
    inline bool restart = true;

    namespace model {
        struct Lesson {
            struct ClassTime {
                int dayOfWeek;
                int startSection;
                int duration;

                static ClassTime fromJson(const QJsonObject &json) {
                    return {json.value("day").toInt(), json.value("start").toInt(), json.value("duration").toInt()};
                }

                [[nodiscard]] QJsonObject toJson() const {
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

            [[nodiscard]] QJsonObject toJson() const {
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
            constexpr static int SUSPENDED = 0b10000;
            constexpr static int CLASS_MASTER = 0b100000;

            QUuid uuid;
            QString username;
            QString name;
            QString password;
            bool female = false;
            uint8_t status = 0;

            [[nodiscard]] bool is_admin() const { return (status & ADMIN) != 0; }

            [[nodiscard]] bool is_teacher() const { return (status & TEACHER) != 0; }

            [[nodiscard]] bool is_graduated() const { return (status & GRADUATED) != 0; }

            [[nodiscard]] bool is_master() const { return (status & MASTER) != 0; }

            [[nodiscard]] bool is_suspended() const { return (status & SUSPENDED) != 0; }

            static Account fromJson(const QUuid &uuid, const QJsonObject &json) {
                Account acc;
                acc.uuid = uuid;
                acc.username = json.value("username").toString();
                acc.name = json.value("name").toString();
                acc.password = json.value("password").toString();
                acc.female = json.value("female").toBool();
                acc.status = json.value("status").toInt();
                return acc;
            }

            [[nodiscard]] virtual QJsonObject toJson() const {
                return {
                    {"username", username},
                    {"name", name},
                    {"password", password},
                    {"female", female},
                    {"status", status}
                };
            }

            virtual ~Account() = default;
        };

        struct PersonAccount : Account {
            QString department;
            QString phoneNumber;

        protected:
            PersonAccount() = default;
        };

        struct TeacherAccount : PersonAccount {
            QList<QUuid> teachingClasses;
            QUuid managingClass = QUuid::fromString("00000000-0000-0000-0000-000000000000");

            [[nodiscard]] bool is_teaching() const { return !teachingClasses.isEmpty(); }

            [[nodiscard]] static TeacherAccount fromJson(const QUuid &uuid, const QJsonObject &json) {
                TeacherAccount t;
                t.uuid = uuid;
                t.username = json.value("username").toString();
                t.name = json.value("name").toString();
                t.password = json.value("password").toString();
                t.female = json.value("female").toBool();
                t.status = static_cast<uint8_t>(json.value("status").toInt());
                t.department = json.value("department").toString();
                t.phoneNumber = json.value("phoneNumber").toString();
                if (t.status & CLASS_MASTER) {
                    t.managingClass = QUuid::fromString(json.value("managingClass").toString());
                }
                for (const QJsonArray lessons = json.value("lessons").toArray(); const auto &x: lessons) {
                    t.teachingClasses.append(QUuid::fromString(x.toString()));
                }
                return t;
            }

            [[nodiscard]] QJsonObject toJson() const override {
                QJsonArray lessonData;
                for (const auto &uuid: teachingClasses) {
                    lessonData.append(uuid.toString());
                }
                return {
                    {"username", username},
                    {"name", name},
                    {"password", password},
                    {"female", female},
                    {"status", status},
                    {"department", department},
                    {"phoneNumber", phoneNumber},
                    {"lessons", lessonData}
                };
            }
        };

        struct StudentAccount : PersonAccount {
            QString dormitoryNumber;
            QList<LessonStatus> lessons;

            [[nodiscard]] static StudentAccount fromJson(const QUuid &uuid, const QJsonObject &json) {
                StudentAccount student;
                student.uuid = uuid;
                student.username = json.value("username").toString();
                student.name = json.value("name").toString();
                student.password = json.value("password").toString();
                student.female = json.value("female").toBool();
                student.status = static_cast<uint8_t>(json.value("status").toInt());
                student.department = json.value("department").toString();
                student.phoneNumber = json.value("phoneNumber").toString();
                for (const auto &x: json.value("lessons").toArray()) {
                    if (x.isObject()) {
                        QJsonObject obj = x.toObject();
                        LessonStatus lesson(QUuid::fromString(obj.value("uuid").toString()),
                                            obj.value("retake").toInt());
                        student.lessons.append(lesson);
                    }
                }
                return student;
            }

            [[nodiscard]] QJsonObject toJson() const override {
                QJsonArray lessonData;
                for (const auto &[uuid, retake]: lessons) {
                    const QJsonObject &lesson = {{"uuid", uuid.toString()}, {"retake", retake}};
                    lessonData.append(lesson);
                }
                return {
                    {"username", username},
                    {"name", name},
                    {"password", password},
                    {"female", female},
                    {"status", status},
                    {"department", department},
                    {"phoneNumber", phoneNumber},
                    {"lessons", lessonData}
                };
            }
        };
    }
}
#endif // AAIMS_DATA_STRUCTURES_H
