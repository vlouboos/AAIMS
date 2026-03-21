// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_CONFIG_MANAGER_H
#define ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_CONFIG_MANAGER_H

#include "data_structures.h"
#include "file_manager.h"
#include <shared_mutex>
#include <iostream>
#include <mutex>

namespace config_manager {
    inline aaims::model::ServerConfig config;
    inline std::shared_mutex mtx;
    inline std::string current_path = "data/config.json";

    inline void init() {
        std::unique_lock lock(mtx);
        if (!file_manager::load(current_path, config)) {
            file_manager::save(current_path, config);
            std::clog << "[Config] Generated default config.\n";
        }
    }

    inline aaims::model::ServerConfig get() {
        std::shared_lock lock(mtx);
        return config;
    }
}
#endif // ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_CONFIG_MANAGER_H
