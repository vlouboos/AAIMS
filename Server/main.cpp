// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <iostream>
#include <memory>

#include "account_manager.h"
#include "config_manager.h"
#include "network_manager.h"

int main() {
    std::cout << "Reading config..." << std::endl;
    config_manager::init();
    account_manager::init();
    network_manager::start();
}
