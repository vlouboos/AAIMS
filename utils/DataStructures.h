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
                QVector<QString> targetDepartments;
                QVector<QUuid> targetClasses;
                QVector<QString> targetGrades;
                QVector<QUuid> targetMajors;
                bool isFemale = false;
                QVector<QUuid> specificStudents;
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
                        json.value("weekStart").toInt(),
                        json.value("weekEnd").toInt(), json.value("day").toInt(),
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
            QString semester;
            bool online = false;
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
                course.semester = json.value("semester").toString();
                course.status = json.value("status").toInt();
                course.online = json.value("online").toBool();
                for (const auto &t: json.value("times").toArray()) {
                    course.times.emplace_back(LessonTime::fromJson(t.toObject()));
                }
                for (const auto &t: json.value("classes").toArray()) {
                    course.classes.emplace_back(QUuid::fromString(t.toString()));
                }
                for (const auto &t: json.value("students").toArray()) {
                    course.students.emplace_back(QUuid::fromString(t.toString()));
                }

                // Parse assignment rule only if course is in ACCEPTING status
                if (course.is_accepting()) {
                    const auto &ruleObj = json.value("assignmentRule").toObject();
                    course.assignmentRule.status = ruleObj.value("status").toInt();
                    if (course.assignmentRule.specific_department()) {
                        for (const auto &did: ruleObj.value("specificDepartments").toArray()) {
                            course.assignmentRule.targetDepartments.append(did.toString());
                        }
                    }
                    if (course.assignmentRule.specific_class()) {
                        for (const auto &cid: ruleObj.value("specificClasses").toArray()) {
                            course.assignmentRule.targetClasses.append(QUuid::fromString(cid.toString()));
                        }
                    }
                    if (course.assignmentRule.specific_grade()) {
                        for (const auto &gid: ruleObj.value("specificGrades").toArray()) {
                            course.assignmentRule.targetGrades.append(gid.toString());
                        }
                    }
                    if (course.assignmentRule.specific_gender())
                        course.assignmentRule.isFemale = ruleObj.value("isFemale").toBool();
                    if (course.assignmentRule.specific_major()) {
                        for (const auto &mid: ruleObj.value("specificMajors").toArray()) {
                            course.assignmentRule.targetMajors.append(QUuid::fromString(mid.toString()));
                        }
                    }
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
                QJsonArray c;
                for (const auto &x: this->classes) { c.append(x.toString(QUuid::WithoutBraces)); }
                QJsonArray s;
                for (const auto &x: this->students) { s.append(x.toString(QUuid::WithoutBraces)); }

                QJsonObject result{
                    {"id", id},
                    {"name", name},
                    {"teacher", teacher.toString(QUuid::WithoutBraces)},
                    {"credit", credit},
                    {"semester", semester},
                    {"status", status},
                    {"online", online},
                    {"times", t},
                    {"classes", c},
                    {"students", s}
                };

                // Only store assignment rule if course is in ACCEPTING status
                if (status & ACCEPTING) {
                    QJsonObject ruleObj;
                    ruleObj["status"] = assignmentRule.status;
                    if (assignmentRule.specific_department()) {
                        QJsonArray specificDepartments;
                        for (const auto &did: assignmentRule.targetDepartments) {
                            specificDepartments.append(did);
                        }
                        ruleObj["specificDepartments"] = specificDepartments;
                    }
                    if (assignmentRule.specific_class()) {
                        QJsonArray specificClasses;
                        for (const auto &sid: assignmentRule.targetClasses) {
                            specificClasses.append(sid.toString(QUuid::WithoutBraces));
                        }
                        ruleObj["specificClasses"] = specificClasses;
                    }
                    if (assignmentRule.specific_grade()) {
                        QJsonArray specificGrades;
                        for (const auto &gid: assignmentRule.targetGrades) {
                            specificGrades.append(gid);
                        }
                        ruleObj["specificGrades"] = specificGrades;
                    }
                    if (assignmentRule.specific_gender()) ruleObj["isFemale"] = assignmentRule.isFemale;
                    if (assignmentRule.specific_major()) {
                        QJsonArray specificMajors;
                        for (const auto &mid: assignmentRule.targetMajors) {
                            specificMajors.append(mid.toString());
                        }
                        ruleObj["specificMajors"] = specificMajors;
                    }
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


        struct StudentRating {
            struct RatingDetail {
                QUuid course_uuid;
                double performance = 0.0;
                double score = 0.0;
                double finalScore = 0.0;

                static RatingDetail fromJson(const QUuid &uuid, const QJsonObject &json) {
                    RatingDetail rating;
                    rating.course_uuid = uuid;
                    rating.performance = json.value("performance").toDouble();
                    rating.score = json.value("score").toDouble();
                    rating.finalScore = json.value("finalScore").toDouble();
                    return rating;
                }
            };

            QUuid student_id;
            QHash<QUuid, RatingDetail> ratings;

            static StudentRating fromJson(const QUuid &uuid, const QJsonObject &json) {
                StudentRating rating;
                rating.student_id = uuid;
                QHash<QUuid, RatingDetail> ratings;
                for (QJsonObject ratingJson = json.value("ratings").toObject(); const auto &key: ratingJson.keys()) {
                    QUuid ratingUuid = QUuid::fromString(key);
                    ratings[ratingUuid] = RatingDetail::fromJson(ratingUuid, ratingJson[key].toObject());
                }
                rating.ratings = ratings;
                return rating;
            }
        };

        struct CourseStatus {
            QUuid uuid;
            int retake = 0;
        };

        struct Major {
            QUuid uuid;
            QString name;
            QString department;

            static Major fromJson(const QUuid &uuid, const QJsonObject &json) {
                Major major;
                major.uuid = uuid;
                major.name = json.value("name").toString();
                major.department = json.value("department").toString();
                return major;
            }

            [[nodiscard]] QJsonObject toJson() const {
                return {
                    {"name", name},
                    {"department", department}
                };
            }
        };

        struct Class {
            QUuid uuid;
            QString grade;
            QString name;
            QUuid master;
            QUuid major;
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
                cls.major = QUuid::fromString(json.value("major").toString());
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
                    {"major", major.toString(QUuid::WithoutBraces)},
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
            QHash<QString, QVector<QVector<int> > > occupied;

            [[nodiscard]] bool is_occupied() const { return !courses.isEmpty() || managingClass != EMPTY_UUID; }

            [[nodiscard]] bool is_occupied(const QString &semester, const QList<Course::LessonTime> &times, const QList<Course::LessonTime> &existingTimes) {
                if (!occupied.contains(semester)) {
                    return false;
                }
                for (const auto &data : existingTimes) {
                    int mask = 0;
                    for (int i = data.weekStart; i <= data.weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    occupied[semester][data.dayOfWeek][data.startTime] &= ~mask;
                }
                const bool res =  std::ranges::any_of(times, [this, semester](const auto &data) {
                    int mask = 0;
                    for (int i = data.weekStart; i <= data.weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    for (int i = 0; i < data.duration; i++) {
                        if (occupied[semester][data.dayOfWeek][data.startTime + i] & mask) {
                            return true;
                        }
                    }
                    return false;
                });
                for (const auto &data : existingTimes) {
                    int mask = 0;
                    for (int i = data.weekStart; i <= data.weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    occupied[semester][data.dayOfWeek][data.startTime] |= mask;
                }
                return res;
            }

            void addCourse(const Course *course) {
                courses.append(course->uuid);
                if (!occupied.contains(course->semester)) {
                    occupied[course->semester].resize(7, QVector<int>(15, 0));
                }
                for (const auto &[weekStart, weekEnd, dayOfWeek, startTime, duration, location]: course->
                     times) {
                    int mask = 0;
                    for (int i = weekStart; i <= weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    for (int i = 0; i < duration; i++) {
                        qDebug() << "Add course" << course->uuid << "at" << dayOfWeek << startTime + i;
                        occupied[course->semester][dayOfWeek][startTime + i] |= mask;
                    }
                }
            }

            void removeCourse(const Course *course) {
                courses.removeOne(course->uuid);
                for (const auto &[weekStart, weekEnd, dayOfWeek, startTime, duration, location]: course->
                     times) {
                    int mask = 0;
                    for (int i = weekStart; i <= weekEnd; ++i) {
                        mask |= 1 << (i - 1);
                    }
                    for (int i = 0; i < duration; i++) {
                        occupied[course->semester][dayOfWeek][startTime + i] &= ~mask;
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
