// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_DATA_STRUCTURES_H
#define AAIMS_DATA_STRUCTURES_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace aaims::model {
    struct LessonStatus {
        std::string uuid;
        bool retake = false;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LessonStatus, uuid, retake)

    struct Account {
        constexpr static int ADMIN = 0b1;
        constexpr static int TEACHER = 0b10;
        constexpr static int GRADUATED = 0b100;

        std::string id;
        std::string username;
        std::string name;
        std::string password;
        uint8_t status = 0;
        std::vector<LessonStatus> lessons;

        [[nodiscard]] bool is_admin() const { return (status & ADMIN) != 0; }

        [[nodiscard]] bool is_teacher() const { return (status & TEACHER) != 0; }

        [[nodiscard]] bool is_graduated() const { return (status & GRADUATED) != 0; }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Account, id, username, password, status, lessons)

    struct Lesson {
        std::string uuid;
        std::string name;
        std::string teacher;
        std::string time;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Lesson, uuid, name, teacher, time)

    struct RatingDetail {
        std::string lesson_uuid;
        double performance = 0.0;
        double score = 0.0;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RatingDetail, lesson_uuid, performance, score)

    struct StudentRating {
        std::string student_id;
        std::vector<RatingDetail> ratings;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(StudentRating, student_id, ratings)
}
#endif // AAIMS_DATA_STRUCTURES_H
