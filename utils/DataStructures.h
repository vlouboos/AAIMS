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
        static constexpr auto EMPTY_UUID = QUuid();

        struct Course {
            constexpr static int ACCEPTING = 0b1;
            constexpr static int QUALIFYING = 0b10;
            constexpr static int STARTED = 0b100;

            struct LessonTime {
                int dayOfWeek;
                int startSection;
                int duration;

                ~LessonTime() = default;

                static LessonTime fromJson(const QJsonObject &json) {
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
            QList<LessonTime> times;
            uint8_t status;

            ~Course() = default;

            static Course fromJson(const QUuid &uuid, const QJsonObject &json) {
                Course course;
                course.uuid = uuid;
                course.name = json.value("name").toString();
                course.teacher = json.value("teacher").toString();
                course.status = json.value("status").toInt();
                QList<LessonTime> times;
                for (const auto &t: json.value("times").toArray()) {
                    times.emplace_back(LessonTime::fromJson(t.toObject()));
                }
                return course;
            }

            [[nodiscard]] QJsonObject toJson() const {
                QJsonArray t;
                for (const auto &x: this->times) { t.append(x.toJson()); }
                return {
                    {"name", name},
                    {"teacher", teacher},
                    {"status", status},
                    {"times", t}
                };
            }
        };

        struct RatingDetail {
            QUuid course_uuid;
            double performance = 0.0;
            double score = 0.0;
        };

        struct StudentRating {
            QUuid student_id;
            QList<RatingDetail> ratings;
        };

        struct CourseStatus {
            QUuid uuid;
            int retake = 0;
        };

        struct Classes {
            QUuid uuid;
            QString grade;
            QString name;
            QUuid master;
            QString department;
            QList<QUuid> lessons;
            QList<QUuid> students;

            virtual ~Classes() = default;

            [[nodiscard]] bool isEmpty() const {
                return students.isEmpty();
            }

            static Classes fromJson(const QUuid &uuid, const QJsonObject &json) {
                Classes cls;
                cls.uuid = uuid;
                cls.grade = json.value("grade").toString();
                cls.name = json.value("name").toString();
                cls.master = QUuid::fromString(json.value("master").toString());
                cls.department = json.value("department").toString();
                for (const auto &x: json.value("lessons").toArray()) {
                    cls.lessons.append(QUuid::fromString(x.toString()));
                }
                for (const auto &x: json.value("students").toArray()) {
                    cls.students.append(QUuid::fromString(x.toString()));
                }
                return cls;
            }

            [[nodiscard]] virtual QJsonObject toJson() const {
                QJsonArray lessons_array;
                for (const auto &lesson: lessons) {
                    lessons_array.append(lesson.toString(QUuid::WithoutBraces));
                }
                QJsonArray students_array;
                for (const auto &student: students) {
                    students_array.append(student.toString(QUuid::WithoutBraces));
                }
                return {
                    {"grade", grade},
                    {"name", name},
                    {"master", master.toString(QUuid::WithoutBraces)},
                    {"department", department},
                    {"lessons", lessons_array},
                    {"students", students_array}
                };
            }
        };

        struct Account {
            constexpr static int ADMIN = 0b1;
            constexpr static int TEACHER = 0b10;
            constexpr static int GRADUATED = 0b100;
            constexpr static int MASTER = 0b1000;
            constexpr static int SUSPENDED = 0b10000;
            constexpr static int CLASS_MASTER = 0b100000;

            QUuid uuid = EMPTY_UUID;
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

            [[nodiscard]] bool is_class_master() const { return (status & CLASS_MASTER) != 0; }

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
            QString phoneNumber;

        protected:
            PersonAccount() = default;
        };

        struct TeacherAccount : PersonAccount {
            QList<QUuid> teachingClasses;
            QString department;
            QUuid managingClass = EMPTY_UUID;

            [[nodiscard]] bool is_occupied() const { return !teachingClasses.isEmpty() || managingClass != EMPTY_UUID; }

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
                    lessonData.append(uuid.toString(QUuid::WithoutBraces));
                }
                return {
                    {"username", username},
                    {"name", name},
                    {"password", password},
                    {"female", female},
                    {"status", status},
                    {"department", department},
                    {"phoneNumber", phoneNumber},
                    {"managingClass", managingClass.toString(QUuid::WithoutBraces)},
                    {"lessons", lessonData}
                };
            }
        };

        struct StudentAccount : PersonAccount {
            QUuid currentClass;
            QString dormitory;
            QList<CourseStatus> lessons;

            [[nodiscard]] static StudentAccount fromJson(const QUuid &uuid, const QJsonObject &json) {
                StudentAccount student;
                student.uuid = uuid;
                student.username = json.value("username").toString();
                student.name = json.value("name").toString();
                student.password = json.value("password").toString();
                student.female = json.value("female").toBool();
                student.status = static_cast<uint8_t>(json.value("status").toInt());
                student.currentClass = QUuid::fromString(json.value("currentClass").toString());
                student.dormitory = json.value("dormitory").toString();
                student.phoneNumber = json.value("phoneNumber").toString();
                for (const auto &x: json.value("lessons").toArray()) {
                    if (x.isObject()) {
                        QJsonObject obj = x.toObject();
                        CourseStatus lesson(QUuid::fromString(obj.value("uuid").toString()),
                                            obj.value("retake").toInt());
                        student.lessons.append(lesson);
                    }
                }
                return student;
            }

            [[nodiscard]] QJsonObject toJson() const override {
                QJsonArray lessonData;
                for (const auto &[uuid, retake]: lessons) {
                    const QJsonObject &lesson = {{"uuid", uuid.toString(QUuid::WithoutBraces)}, {"retake", retake}};
                    lessonData.append(lesson);
                }
                return {
                    {"username", username},
                    {"name", name},
                    {"password", password},
                    {"female", female},
                    {"status", status},
                    {"currentClass", currentClass.toString(QUuid::WithoutBraces)},
                    {"dormitory", dormitory},
                    {"phoneNumber", phoneNumber},
                    {"lessons", lessonData}
                };
            }
        };
    }
}
#endif // AAIMS_DATA_STRUCTURES_H
