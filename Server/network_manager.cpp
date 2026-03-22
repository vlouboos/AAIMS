// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <App.h>

#include "config_manager.h"
#include "connection.h"

std::unordered_map<uWS::WebSocket<false, true, int> *, connection> connections;

class network_manager {
    static std::unordered_map<uWS::WebSocket<false, true, int> *, connection> connections;

public:
    static void start() {
        uWS::App().ws<connection>("/aaims", {
                                      .open = [](auto *ws) {
                                          const connection *c = new(ws->getUserData()) connection();
                                          std::clog << "Connection " << *c << std::endl;
                                      },
                                      .message = [](auto *ws, std::string_view message, uWS::OpCode op_code) {
                                          if (op_code == uWS::OpCode::TEXT) {
                                              nlohmann::json j = nlohmann::json::parse(message);
                                              if (!j.contains("type")) return
                                              ws->getUserData()->handleMessage(j.value("type", ""), j.value("payload", nlohmann::json()));
                                          }
                                      },
                                  })
                .listen(config_manager::get().host, config_manager::get().port, [](const auto *listen_socket) {
                    if (listen_socket) {
                        std::clog << "Listening on " << config_manager::get().host << ":" << config_manager::get().port
                                << "/aaims" << std::endl;
                    } else {
                        std::cerr << "Failed to listen on " << config_manager::get().host << ":" <<
                                config_manager::get().port << ". Maybe port is occupied!" << std::endl;
                    }
                }).run();
    }
};
