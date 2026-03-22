// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ACCOUNT_MANAGER_H
#define AAIMS_ACCOUNT_MANAGER_H

#include "DataStructures.h"
#include "FileManager.h"
#include <map>
#include <mutex>
#include <qwidget.h>
#include <shared_mutex>

namespace aaims::manager::account {
    static inline std::map<std::string, model::Account> accounts;
    static inline std::shared_mutex mtx;
    static inline std::string current_path = "data/accounts.json";

    static bool init() {
        std::unique_lock lock(mtx);
        return file::load(current_path, accounts);
    }

    static bool save() {
        std::shared_lock lock(mtx);
        return file::save(current_path, accounts);
    }

    static bool add(const model::Account &acc) {
        std::unique_lock lock(mtx);
        if (accounts.contains(acc.id)) return false;
        accounts[acc.id] = acc;
        return true;
    }

    static model::Account *find(std::string id) {
        std::ranges::transform(id, id.begin(),
                               [](const unsigned char c) { return std::tolower(c); });
        std::shared_lock lock(mtx);
        if (const auto it = accounts.find(id); it != accounts.end()) {
            return std::addressof(it->second);
        }
        return nullptr;
    }
}

using namespace aaims::model;

class AccountManager {
public:
    inline static Account *logged = nullptr;

    static Account *tryLogin(const std::string &username, const std::string &password);
};

#endif // AAIMS_ACCOUNT_MANAGER_H
