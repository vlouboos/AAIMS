// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <QApplication>
#include <QFutureWatcher>

#include "AccountManager.h"
#include "../utils/AsyncJsonIO.h"

using namespace aaims::io;
using namespace aaims::model;

namespace {
    QHash<QUuid, Account> accounts;
    QHash<QString, QUuid> accounts_by_username;
    QHash<QString, QUuid> accounts_by_name;
    bool loading = true;
}

namespace aaims::manager::account {
    void init() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/accounts.json";
        const auto &future = loadAsync(path, [](const QJsonObject &json) {
            for (const auto &key: json.keys()) {
                const QUuid uuid = QUuid::fromString(key);
                QJsonObject accountData = json.value(key).toObject();
                QList<LessonStatus> lessons;
                for (QJsonArray lessonData = accountData.value("lessons").toArray(); const auto &lesson: lessonData) {
                    if (lesson.isObject()) {
                        const auto &lessonJson = lesson.toObject();
                        lessons.emplace_back(QUuid::fromString(lessonJson.value("uuid").toString()),
                                             lessonJson.value("retake").toInt());
                    }
                }
                Account account(uuid,
                                accountData.value("username").toString(),
                                accountData.value("name").toString(),
                                accountData.value("password").toString(),
                                accountData.value("female").toBool(),
                                accountData.value("status").toInt(),
                                lessons
                );
                accounts[uuid] = account;
                accounts_by_username[account.username.toLower()] = uuid;
                accounts_by_name[account.name] = uuid;
            }
        });
        auto *watcher = new QFutureWatcher<void>(InternalManager::instance()); // NOLINT
        QObject::connect(watcher, &QFutureWatcher<void>::finished, [watcher] {
            qDebug() << "Loaded" << accounts.size() << "accounts.";
            emit InternalManager::instance()->loaded();
            watcher->deleteLater();
            loading = false;
        });

        watcher->setFuture(future);
    }

    QFuture<bool> save() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/accounts.json";
        QJsonObject root;
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            root.insert(it.key().toString(), it->toJson());
        }
        const auto &future = saveAsync(path, root);
        auto *watcher = new QFutureWatcher<void>(InternalManager::instance()); // NOLINT
        QObject::connect(watcher, &QFutureWatcher<void>::finished, [watcher] {
            qDebug() << "Loaded" << accounts.size() << "accounts.";
            emit InternalManager::instance()->loaded();
            watcher->deleteLater();
        });
        watcher->setFuture(future);
        return future;
    }

    void add(const Account &account) {
        accounts[account.uuid] = account;
        accounts_by_username[account.username.toLower()] = account.uuid;
        accounts_by_name[account.name] = account.uuid;
    }

    Account *tryLogin(const QString &username, const QString &password) {
        if (Account *account = findByUsername(username); account && account->password == password)
            return account;
        return nullptr;
    }

    bool isEmpty() {
        return accounts.isEmpty();
    }

    void onLoaded(const std::function<void()> &callback) {
        QObject::connect(InternalManager::instance(), &InternalManager::loaded, callback);
    }

    Account *findByUUID(const QUuid &uuid) {
        if (accounts.contains(uuid)) {
            return std::addressof(accounts[uuid]);
        }
        return nullptr;
    }

    Account *findByUsername(const QString &username) {
        if (accounts_by_username.contains(username)) {
            return findByUUID(accounts_by_username[username.toLower()]);
        }
        return nullptr;
    }

    Account *findByName(const QString &name) {
        if (accounts_by_name.contains(name)) {
            return findByUUID(accounts_by_name[name]);
        }
        return nullptr;
    }

    bool isLoading() {
        return loading;
    }
}
