// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_DATA_STRUCTURES_H
#define ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_DATA_STRUCTURES_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace aaims::model {
    struct ServerConfig {
        std::string host = "0.0.0.0";
        int port = 4399;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ServerConfig, host, port)

    struct LessonStatus {
        std::string uuid;
        bool retake = false;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LessonStatus, uuid, retake)

    struct Account {
        std::string id;
        std::string username;
        std::string password;
        uint32_t status = 0;
        std::vector<LessonStatus> lessons;
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
#endif // ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_DATA_STRUCTURES_H
