// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ACCOUNT_MANAGER_H
#define AAIMS_ACCOUNT_MANAGER_H

#include "../utils/DataStructures.h"
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

    QFuture<bool> saveAsync();

    bool save();

    bool isEmpty();

    Account *tryLogin(const QString &username, const QString &password);

    void onLoaded(const std::function<void()> &callback);

    Account *findByUUID(const QUuid &uuid);

    Account *findByUsername(const QString &username);

    Account *findByName(const QString &name);

    QHash<QUuid, std::shared_ptr<Account> > all();

    QHash<QUuid, Account *> get_admins();

    QHash<QUuid, TeacherAccount *> get_teachers();

    QHash<QUuid, StudentAccount *> get_students();

    QHash<QUuid, StudentAccount *> get_working_students();

    QHash<QUuid, StudentAccount *> get_graduated_students();

    QHash<QUuid, StudentAccount *> get_suspended_students();

    QString add(const std::shared_ptr<Account> &account);

    bool isLoading();

    QList<Account *> get_all_ptrs();

    void remove(const Account *account);
}

#endif // AAIMS_ACCOUNT_MANAGER_H
