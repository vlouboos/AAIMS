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
            constexpr static int ENDED = 0b1000;

            // Assignment rule for distributing courses to students
            struct AssignmentRule {
                constexpr static int DEPARTMENT = 0b1;
                constexpr static int CLASS = 0b10;
                constexpr static int GENDER = 0b100;
                constexpr static int GRADE = 0b1000;
                constexpr static int MAJOR = 0b10000;
                constexpr static int SINGLE = 0b100000;

                [[nodiscard]] bool specific_department() const {
                    return (status & DEPARTMENT) != 0;
                }

                [[nodiscard]] bool specific_class() const {
                    return (status & CLASS) != 0;
                }

                [[nodiscard]] bool specific_gender() const {
                    return (status & GENDER) != 0;
                }

                [[nodiscard]] bool specific_grade() const {
                    return (status & GRADE) != 0;
                }

                [[nodiscard]] bool specific_major() const {
                    return (status & MAJOR) != 0;
                }

                [[nodiscard]] bool specific_single() const {
                    return (status & SINGLE) != 0;
                }

                uint8_t status = 0;
                QString targetDepartment;
                QUuid targetClass;
                bool isFemale = false;
                QList<QUuid> specificStudents;
            };

            struct LessonTime {
                inline static const QStringList TIME_TABLE = {
                    "8:00", "8:45", "9:55", "10:40", "11:25", "12:40", "13:25", "14:30", "15:15", "16:25", "17:10",
                    "17:55", "19:30", "20:15", "21:00"
                };
                inline static const QStringList DAY_OF_WEEK_TABLE = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
                int weekStart;
                int weekEnd;
                int dayOfWeek;
                int startTime;
                int duration;
                QString location;

                ~LessonTime() = default;

                static LessonTime fromJson(const QJsonObject &json) {
                    return {
                        json.value("weekStart").toInt(), json.value("weekEnd").toInt(), json.value("day").toInt(),
                        json.value("start").toInt(), json.value("duration").toInt(), json.value("location").toString()
                    };
                }

                [[nodiscard]] QJsonObject toJson() const {
                    return {
                        {"weekStart", weekStart},
                        {"weekEnd", weekEnd},
                        {"day", dayOfWeek},
                        {"start", startTime},
                        {"duration", duration},
                        {"location", location}
                    };
                }
            };

            QUuid uuid;
            QString id;
            QString name;
            QUuid teacher;
            int credit = 0;
            uint8_t status = 0;
            QString semester; // TODO: Make it work
            QList<LessonTime> times;
            QList<QUuid> students;
            QList<QUuid> classes;
            AssignmentRule assignmentRule; // Rule for determining which students can select this course

            ~Course() = default;

            [[nodiscard]] bool is_accepting() const { return (status & ACCEPTING) != 0; }

            [[nodiscard]] bool is_qualifying() const { return (status & QUALIFYING) != 0; }

            [[nodiscard]] bool is_started() const { return (status & STARTED) != 0; }

            [[nodiscard]] bool is_ended() const { return (status & ENDED) != 0; }

            static Course fromJson(const QUuid &uuid, const QJsonObject &json) {
                Course course;
                course.uuid = uuid;
                course.id = json.value("id").toString();
                course.name = json.value("name").toString();
                course.teacher = QUuid::fromString(json.value("teacher").toString());
                course.credit = json.value("credit").toInt();
                course.status = json.value("status").toInt();
                QList<LessonTime> times;
                for (const auto &t: json.value("times").toArray()) {
                    times.emplace_back(LessonTime::fromJson(t.toObject()));
                }

                // Parse assignment rule only if course is in ACCEPTING status
                if (course.is_accepting()) {
                    const auto &ruleObj = json.value("assignmentRule").toObject();
                    course.assignmentRule.status = ruleObj.value("status").toInt();
                    if (course.assignmentRule.specific_department())
                        course.assignmentRule.targetDepartment = ruleObj.value("targetDepartment").toString();
                    if (course.assignmentRule.specific_class())
                        course.assignmentRule.targetClass = QUuid::fromString(
                            ruleObj.value("targetClass").toString());
                    if (course.assignmentRule.specific_gender())
                        course.assignmentRule.isFemale = ruleObj.value("isFemale").toBool();
                    // TODO: Add "major" as an element of classes
                    if (course.assignmentRule.specific_single()) {
                        for (const auto &sid: ruleObj.value("specificStudents").toArray()) {
                            course.assignmentRule.specificStudents.append(QUuid::fromString(sid.toString()));
                        }
                    }
                }

                return course;
            }

            [[nodiscard]] QJsonObject toJson() const {
                QJsonArray t;
                for (const auto &x: this->times) { t.append(x.toJson()); }

                QJsonObject result{
                    {"id", id},
                    {"name", name},
                    {"teacher", teacher.toString(QUuid::WithoutBraces)},
                    {"credit", credit},
                    {"status", status},
                    {"times", t}
                };

                // Only store assignment rule if course is in ACCEPTING status
                if (status & ACCEPTING) {
                    QJsonObject ruleObj;
                    ruleObj["status"] = assignmentRule.status;
                    if (assignmentRule.specific_department())
                        ruleObj["targetDepartment"] = assignmentRule.targetDepartment;
                    if (assignmentRule.specific_class())
                        ruleObj["targetClass"] = assignmentRule.targetClass.toString(
                            QUuid::WithoutBraces);
                    if (assignmentRule.specific_gender()) ruleObj["isFemale"] = assignmentRule.isFemale;
                    if (assignmentRule.specific_single()) {
                        QJsonArray specificStudents;
                        for (const auto &sid: assignmentRule.specificStudents) {
                            specificStudents.append(sid.toString(QUuid::WithoutBraces));
                        }
                        ruleObj["specificStudents"] = specificStudents;
                    }
                    result["assignmentRule"] = ruleObj;
                }

                return result;
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

        struct Class {
            QUuid uuid;
            QString grade;
            QString name;
            QUuid master;
            QString department;
            QList<QUuid> courses;
            QList<QUuid> students;

            virtual ~Class() = default;

            [[nodiscard]] bool isEmpty() const {
                return students.isEmpty();
            }

            static Class fromJson(const QUuid &uuid, const QJsonObject &json) {
                Class cls;
                cls.uuid = uuid;
                cls.grade = json.value("grade").toString();
                cls.name = json.value("name").toString();
                cls.master = QUuid::fromString(json.value("master").toString());
                cls.department = json.value("department").toString();
                for (const auto &x: json.value("courses").toArray()) {
                    cls.courses.append(QUuid::fromString(x.toString()));
                }
                for (const auto &x: json.value("students").toArray()) {
                    cls.students.append(QUuid::fromString(x.toString()));
                }
                return cls;
            }

            [[nodiscard]] virtual QJsonObject toJson() const {
                QJsonArray courses_array;
                for (const auto &course: courses) {
                    courses_array.append(course.toString(QUuid::WithoutBraces));
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
                    {"courses", courses_array},
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
            QList<QUuid> courses;
            QString department;
            QUuid managingClass = EMPTY_UUID;
            int occupied[7][15]{};

            [[nodiscard]] bool is_occupied() const { return !courses.isEmpty() || managingClass != EMPTY_UUID; }

            [[nodiscard]] bool is_occupied(const QList<Course::LessonTime> &times) {
                return std::ranges::all_of(times, [this](const auto &data) {
                    int mask = 0;
                    for (int i = data.weekStart; i <= data.weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    for (int i = 0; i < data.duration; i++) {
                        if (occupied[data.dayOfWeek - 1][data.startTime + i] & mask) {
                            return true;
                        }
                    }
                    return false;
                });
            }

            void addCourse(const Course *const &course) {
                courses.append(course->uuid);
                for (const auto &[weekStart, weekEnd, dayOfWeek, startTime, duration, location]: course->times) {
                    int mask = 0;
                    for (int i = weekStart; i <= weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    for (int i = 0; i < duration; i++) {
                        occupied[dayOfWeek - 1][startTime + i] |= mask;
                    }
                }
            }

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
                    t.courses.append(QUuid::fromString(x.toString()));
                }
                return t;
            }

            [[nodiscard]] QJsonObject toJson() const override {
                QJsonArray lessonData;
                for (const auto &uuid: courses) {
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
