// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "ClassManager.h"

#include <qcoreapplication.h>
#include <QFutureWatcher>

#include "../utils/AsyncJsonIO.h"
#include "../utils/DataStructures.h"

namespace {
    QList<QString> departments;
    QHash<QUuid, aaims::model::Classes *> all_classes;
}

namespace aaims::manager::classes {
    QPair<unsigned long long, unsigned long long> addDepartment(const QVector<QString> &dep) {
        unsigned long long succeed = 0, fail = 0;
        for (const auto &department: dep) {
            if (departments.contains(department, Qt::CaseInsensitive)) {
                fail++;
                continue;
            }
            departments.append(department);
            succeed++;
        }
        return {succeed, fail};
    }

    QList<QString> get_departments() {
        return departments;
    }

    bool saveDepartments() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/departments.json";
        QJsonObject root;
        QJsonArray array;
        for (auto it = departments.begin(); it != departments.end(); ++it) {
            array.append(*it);
        }
        root["departments"] = array;
        return io::save(path, root);
    }

    void init() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/departments.json";
        const auto &future = io::loadAsync(path, [](const QJsonObject &json) {
            if (json.contains("departments")) {
                QJsonArray departments = json["departments"].toArray();
                QVector<QString> department_list;
                for (const auto &department: departments) {
                    department_list.append(department.toString());
                }
                addDepartment(department_list);
            }
        });
        const auto watcher = new QFutureWatcher<void>(); // NOLINT
        QObject::connect(watcher, &QFutureWatcher<void>::finished, [watcher] {
            watcher->deleteLater();
        });
        watcher->setFuture(future);
    }

    QHash<QUuid, model::Classes *> get_classes() {
        return all_classes;
    }

    void removeClass(const QUuid &uuid) {
        all_classes.remove(uuid);
    }
}
