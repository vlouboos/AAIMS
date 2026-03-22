// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CONNECTION_H
#define AAIMS_CONNECTION_H
#include "account_manager.h"

struct connection {
    uWS::WebSocket<false, true, connection> *ws;
    bool loggedIn = false;
    aaims::model::Account *account = nullptr;

    explicit connection(auto *ws): ws(ws) {}

    void send(const std::string &type, const nlohmann::json &payload) const {
        nlohmann::json j;
        j["type"] = type;
        j["payload"] = payload;
        ws->send(j.dump(), uWS::OpCode::TEXT);
    }

    void handleMessage(const std::string &type, const nlohmann::json &payload) {
        if (!loggedIn) {
            if (type != "C2SLogin") return;
            const std::string username = payload.value("username", "");
            // Search account first
            if (aaims::model::Account *acc = account_manager::find(username); !acc) {
                send("S2CAccount", {{"code", -1}});
            } else {
                if (const std::string password = payload.value("password", ""); acc->password != password) {
                    send("S2CAccount", {{"code", 1}});
                } else {
                    loggedIn = true;
                    account = acc;
                    send("S2CAccount", {{"code", 0}});
                }
            }
        } else {
        }
    }
};

inline std::ostream &operator<<(std::ostream &os, const connection &conn) {
    os << "Connection(#" << &conn << ")";
    return os;
}

#endif //AAIMS_CONNECTION_H
