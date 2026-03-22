// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ACCOUNT_MANAGER_H
#define AAIMS_ACCOUNT_MANAGER_H

#include "DataStructures.h"
#include <QApplication>
#include <QFuture>

using namespace aaims::model;

namespace aaims::manager::account {
    class InternalManager : public QObject {
        Q_OBJECT

    public:
        static InternalManager *instance() {
            static InternalManager inst;
            return &inst;
        }

    signals:
        void loaded();
    };

    inline Account *logged = nullptr;

    void init();

    QFuture<bool> save();

    bool isEmpty();

    Account *tryLogin(const QString &username, const QString &password);

    void onLoaded(const std::function<void()> &callback);

    Account *findByUUID(const QUuid &uuid);

    Account *findByUsername(const QString &username);

    Account *findByName(const QString &name);

    void add(const Account &account);

    bool isLoading();
}

#endif // AAIMS_ACCOUNT_MANAGER_H
