// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <App.h>
#include <iostream>
#include "config_manager.h"

int main() {
    std::cout << "Reading config..." << std::endl;
    config_manager::init();
    uWS::App().ws<int>("/aaims", {})
    .listen(config_manager::get().host, config_manager::get().port, [&](const auto *listen_socket) {
        if (listen_socket) {
            std::clog << "Listening on " << config_manager::get().host << ":" << config_manager::get().port << std::endl;
        } else {
            std::cerr << "Failed to listen on " << config_manager::get().host << ":" << config_manager::get().port << ". Maybe port is occupied!" << std::endl;
        }
    }).run();
}
