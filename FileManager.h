// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_FILE_MANAGER_H
#define AAIMS_FILE_MANAGER_H

#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace aaims::manager::file {
    namespace fs = std::filesystem;

    template<typename T>
    bool load(const std::string &path, T &out_data) {
        if (!fs::exists(path)) return false;
        try {
            std::ifstream file(path);
            if (!file.is_open()) return false;
            nlohmann::json j;
            file >> j;
            out_data = j.get<T>();
            return true;
        } catch (...) { return false; }
    }

    template<typename T>
    bool save(const std::string &path, const T &data) {
        try {
            const fs::path p(path);
            if (p.has_parent_path()) fs::create_directories(p.parent_path());
            std::ofstream file(p, std::ios::out | std::ios::trunc);
            if (!file.is_open()) return false;
            file << nlohmann::json(data).dump(4);
            return true;
        } catch (...) { return false; }
    }
}
#endif // AAIMS_FILE_MANAGER_H
