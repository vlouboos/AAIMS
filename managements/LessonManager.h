// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_LESSON_MANAGER_H
#define AAIMS_LESSON_MANAGER_H

#include "../utils/DataStructures.h"
#include <map>
#include <mutex>
#include <shared_mutex>
#include <optional>

namespace aaims::manager::lesson {
    inline std::map<std::string, model::Lesson> lessons;
    inline std::shared_mutex mtx;
    inline std::string current_path = "data/lessons.json";

    inline bool init(const std::string &path = "data/lessons.json") {
        std::unique_lock lock(mtx);
        current_path = path;
        return file::load(current_path, lessons);
    }

    inline bool save() {
        std::shared_lock lock(mtx);
        return file::save(current_path, lessons);
    }

    inline bool add(const model::Lesson &lesson) {
        std::unique_lock lock(mtx);
        if (lessons.contains(lesson.uuid)) return false;
        lessons[lesson.uuid] = lesson;
        return true;
    }

    inline std::optional<model::Lesson> find(const std::string &uuid) {
        std::shared_lock lock(mtx);
        if (const auto it = lessons.find(uuid); it != lessons.end()) {
            return it->second;
        }
        return std::nullopt;
    }
}
#endif // AAIMS_LESSON_MANAGER_H
