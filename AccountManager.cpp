// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AccountManager.h"

using namespace aaims::model;

Account *AccountManager::tryLogin(const std::string &username, const std::string &password) {
    if (Account *account = aaims::manager::account::find(username); account && account->password == password)
        return account;
    return nullptr;
}
