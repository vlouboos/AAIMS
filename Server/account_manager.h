// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_ACCOUNT_MANAGER_H
#define ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_ACCOUNT_MANAGER_H

#include "data_structures.h"
#include "file_manager.h"
#include <map>
#include <mutex>
#include <shared_mutex>
#include <optional>

namespace account_manager {
    inline std::map<std::string, aaims::model::Account> accounts;
    inline std::shared_mutex mtx;
    inline std::string current_path = "data/accounts.json";

    inline bool init(const std::string &path = "data/accounts.json") {
        std::unique_lock lock(mtx);
        current_path = path;
        return file_manager::load(current_path, accounts);
    }

    inline bool save() {
        std::shared_lock lock(mtx);
        return file_manager::save(current_path, accounts);
    }

    inline bool add(const aaims::model::Account &acc) {
        std::unique_lock lock(mtx);
        if (accounts.contains(acc.id)) return false;
        accounts[acc.id] = acc;
        return true;
    }

    inline std::optional<aaims::model::Account> find(const std::string &id) {
        std::shared_lock lock(mtx);
        if (const auto it = accounts.find(id); it != accounts.end()) {
            return it->second;
        }
        return std::nullopt;
    }
}
#endif // ACADEMICAFFAIRSINFORMATIONMANAGEMENTSYSTEM_ACCOUNT_MANAGER_H
