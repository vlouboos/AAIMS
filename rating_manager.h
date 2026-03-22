// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_RATING_MANAGER_H
#define AAIMS_RATING_MANAGER_H

#include "data_structures.h"
#include "file_manager.h"
#include <map>
#include <vector>
#include <shared_mutex>
#include <filesystem>
#include <mutex>

namespace rating_manager {
    inline std::map<std::string, std::vector<aaims::model::StudentRating> > registry;
    inline std::shared_mutex mtx;
    inline std::string base_dir = "data/ratings/";

    inline void init_all(const std::string &dir = "data/ratings/") {
        std::unique_lock lock(mtx);
        base_dir = dir;
        namespace fs = std::filesystem;

        if (!fs::exists(base_dir)) {
            fs::create_directories(base_dir);
            return;
        }

        for (const auto &entry: fs::directory_iterator(base_dir)) {
            if (entry.path().extension() == ".json") {
                std::string name = entry.path().stem().string();
                if (std::vector<aaims::model::StudentRating> data; file_manager::load(entry.path().string(), data)) {
                    registry[name] = std::move(data);
                }
            }
        }
    }

    inline bool save_by_name(const std::string &name) {
        std::shared_lock lock(mtx);
        if (!registry.contains(name)) return false;
        return file_manager::save(base_dir + name + ".json", registry[name]);
    }

    inline bool add_record(const std::string &rating_name, const aaims::model::StudentRating &record) {
        std::unique_lock lock(mtx);
        registry[rating_name].push_back(record);
        return true;
    }
}
#endif // AAIMS_RATING_MANAGER_H
