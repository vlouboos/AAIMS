// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <iostream>

#include "account_manager.h"

int main() {
    std::cout << "Loading accounts..." << std::endl;
    account_manager::init();
    int a = 10;
}
