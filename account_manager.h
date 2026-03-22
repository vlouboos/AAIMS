// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ACCOUNT_MANAGER_H
#define AAIMS_ACCOUNT_MANAGER_H

#include "data_structures.h"
#include "file_manager.h"
#include <map>
#include <mutex>
#include <shared_mutex>

namespace account_manager {
    static inline std::map<std::string, aaims::model::Account> accounts;
    static inline std::shared_mutex mtx;
    static inline std::string current_path = "data/accounts.json";

    static bool init(const std::string &path = "data/accounts.json") {
        std::unique_lock lock(mtx);
        current_path = path;
        return file_manager::load(current_path, accounts);
    }

    static bool save() {
        std::shared_lock lock(mtx);
        return file_manager::save(current_path, accounts);
    }

    static bool add(const aaims::model::Account &acc) {
        std::unique_lock lock(mtx);
        if (accounts.contains(acc.id)) return false;
        accounts[acc.id] = acc;
        return true;
    }

    static aaims::model::Account* find(std::string id) {
        std::ranges::transform(id, id.begin(),
                               [](const unsigned char c) { return std::tolower(c); });
        std::shared_lock lock(mtx);
        if (const auto it = accounts.find(id); it != accounts.end()) {
            return std::addressof(it->second);
        }
        return nullptr;
    }
}
#endif // AAIMS_ACCOUNT_MANAGER_H
