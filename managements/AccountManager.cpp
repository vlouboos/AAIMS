// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <QApplication>
#include <QFutureWatcher>

#include "AccountManager.h"

#include "ClassManager.h"
#include "CourseManager.h"
#include "../utils/AsyncJsonIO.h"

using namespace aaims::io;
using namespace aaims::model;

namespace {
    QHash<QUuid, std::shared_ptr<Account> > accounts;
    QHash<QString, QUuid> accounts_by_username;
    QHash<QString, QUuid> accounts_by_name;
    QUuid master;
    QHash<QUuid, Account *> admins;
    QHash<QUuid, TeacherAccount *> teachers;
    QHash<QUuid, StudentAccount *> students;
    QHash<QUuid, StudentAccount *> workingStudents;
    QHash<QUuid, StudentAccount *> graduatedStudents;
    QHash<QUuid, StudentAccount *> suspendedStudents;
    bool loading = true;
}

namespace aaims::manager::account {
    void init() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/accounts.json";
        const auto &future = loadAsync(path, [](const QJsonObject &json) {
            for (const auto &key: json.keys()) {
                const QUuid uuid = QUuid::fromString(key);
                QJsonObject accountData = json.value(key).toObject();
                QList<CourseStatus> lessons;
                for (QJsonArray lessonData = accountData.value("lessons").toArray(); const auto &lesson: lessonData) {
                    if (lesson.isObject()) {
                        const auto &lessonJson = lesson.toObject();
                        lessons.emplace_back(QUuid::fromString(lessonJson.value("uuid").toString()),
                                             lessonJson.value("retake").toInt());
                    }
                }
                const int status = accountData.value("status").toInt();
                std::shared_ptr<Account> acc;
                if (status & Account::ADMIN || status & Account::MASTER) {
                    acc = std::make_shared<Account>(Account::fromJson(uuid, accountData));
                } else if (status & Account::TEACHER) {
                    const auto teacher = std::make_shared<TeacherAccount>(TeacherAccount::fromJson(uuid, accountData));
                    acc = teacher;
                    teachers[uuid] = teacher.get();
                } else {
                    const auto student = std::make_shared<StudentAccount>(StudentAccount::fromJson(uuid, accountData));
                    acc = student;
                    if (student->is_graduated()) graduatedStudents[uuid] = student.get();
                    else if (student->is_suspended()) suspendedStudents[uuid] = student.get();
                    else workingStudents[uuid] = student.get();
                    students[uuid] = student.get();
                }
                accounts[uuid] = acc;
                accounts_by_username[acc->username.toLower()] = uuid;
                accounts_by_name[acc->name] = uuid;
                if (acc->is_master()) {
                    master = uuid;
                }
                if (acc->is_admin()) {
                    admins[uuid] = acc.get();
                }
            }
            for (const auto &teacher: teachers) {
                for (const auto &courseUuid: teacher->courses) {
                    for (const auto &course = course::get_courses()[courseUuid]; const auto &[weekStart, weekEnd, dayOfWeek, startTime, duration, location]: course->times) {
                        int mask = 0;
                        for (int i = weekStart; i <= weekEnd; ++i) {
                            mask |= 1 << (i - 1);
                        }
                        for (int i = 0; i < duration; i++) {
                            teacher->occupied[dayOfWeek - 1][startTime + i] |= mask;
                        }
                    }
                }
            }
        });
        auto *watcher = new QFutureWatcher<void>(InternalManager::instance()); // NOLINT
        QObject::connect(watcher, &QFutureWatcher<void>::finished, [watcher] {
            qDebug() << "Loaded" << accounts.size() << "accounts.";
            watcher->deleteLater();
            loading = false;
        });

        watcher->setFuture(future);
    }

    [[nodiscard]] QFuture<bool> saveAsync() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/accounts.json";
        QJsonObject root;
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            root.insert(it.key().toString(QUuid::WithoutBraces), it.value()->toJson());
        }
        const auto &future = io::saveAsync(path, root);
        auto *watcher = new QFutureWatcher<void>(InternalManager::instance()); // NOLINT
        QObject::connect(watcher, &QFutureWatcher<void>::finished, [watcher] {
            qDebug() << "Saved" << accounts.size() << "accounts.";
            watcher->deleteLater();
        });
        watcher->setFuture(future);
        return future;
    }

    bool save() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/accounts.json";
        QJsonObject root;
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            root.insert(it.key().toString(QUuid::WithoutBraces), it.value()->toJson());
        }
        return io::save(path, root);
    }

    [[nodiscard]] QString add(const std::shared_ptr<Account> &account) {
        if (!account.get()) return "内部错误";
        QUuid uuid;
        do {
            uuid = QUuid::createUuid();
        } while (accounts.contains(uuid) || uuid == EMPTY_UUID);
        account->uuid = uuid;

        if (account->is_master()) {
            master = uuid;
        } else if (account->is_admin()) {
            if (std::ranges::any_of(admins.values(), [account](const Account *a) {
                return account->username.toLower() == a->username.toLower();
            }))
                return "工号已存在！";
            admins[uuid] = account.get();
        } else if (account->is_teacher()) {
            if (std::ranges::any_of(teachers.values(), [account](const TeacherAccount *a) {
                return account->username.toLower() == a->username.toLower();
            }))
                return "工号已存在！";
            teachers[uuid] = dynamic_cast<TeacherAccount *>(account.get());
        } else {
            if (std::ranges::any_of(students.values(), [account](const StudentAccount *a) {
                return account->username.toLower() == a->username.toLower();
            }))
                return "学号已存在！";
            students[uuid] = dynamic_cast<StudentAccount *>(account.get());
            if (account->is_graduated()) {
                graduatedStudents[uuid] = students[uuid];
            } else if (account->is_suspended()) {
                suspendedStudents[uuid] = students[uuid];
            } else {
                workingStudents[uuid] = students[uuid];
            }
        }

        accounts[uuid] = account;
        accounts_by_username[account->username.toLower()] = uuid;
        accounts_by_name[account->name] = uuid;
        return "";
    }

    Account *tryLogin(const QString &username, const QString &password) {
        if (Account *account = findByUsername(username); account && account->password == password)
            return account;
        return nullptr;
    }

    bool isEmpty() {
        return accounts.isEmpty();
    }

    Account *findByUUID(const QUuid &uuid) {
        if (accounts.contains(uuid)) {
            return accounts[uuid].get();
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

    QHash<QUuid, std::shared_ptr<Account> > all() {
        return accounts;
    }

    QHash<QUuid, Account *> get_admins() {
        return admins;
    }

    QHash<QUuid, TeacherAccount *> get_teachers() {
        return teachers;
    }

    QHash<QUuid, StudentAccount *> get_students() {
        return students;
    }

    QHash<QUuid, StudentAccount *> get_working_students() {
        return workingStudents;
    }

    QHash<QUuid, StudentAccount *> get_graduated_students() {
        return graduatedStudents;
    }

    QHash<QUuid, StudentAccount *> get_suspended_students() {
        return suspendedStudents;
    }

    bool isLoading() {
        return loading;
    }

    QList<Account *> get_all_ptrs() {
        QList<Account *> ptrs;
        ptrs.reserve(accounts.size());
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            ptrs.append(it.value().get());
        }
        return ptrs;
    }

    void remove(const Account *account) {
        if (account->is_teacher()) {
            teachers.remove(account->uuid);
        } else if (account->is_admin()) {
            admins.remove(account->uuid);
        } else {
            students.remove(account->uuid);
            if (account->is_graduated()) {
                graduatedStudents.remove(account->uuid);
            } else if (account->is_suspended()) {
                suspendedStudents.remove(account->uuid);
            } else {
                workingStudents.remove(account->uuid);
            }
        }
        accounts.remove(account->uuid);
    }
}
