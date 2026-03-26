// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "ClassManager.h"

#include <qcoreapplication.h>

#include "../utils/AsyncJsonIO.h"
#include "../utils/DataStructures.h"

namespace {
    QList<QString> departments;
    QHash<QUuid, std::shared_ptr<aaims::model::Classes> > all_classes;
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
        QString path = QCoreApplication::applicationDirPath() + "/data/departments.json";
        io::load(path, [](const QJsonObject &json) {
            if (json.contains("departments")) {
                QJsonArray departments = json["departments"].toArray();
                QVector<QString> department_list;
                for (const auto &department: departments) {
                    department_list.append(department.toString());
                }
                addDepartment(department_list);
            }
        });
        path = QCoreApplication::applicationDirPath() + "/data/classes.json";
        io::load(path, [](const QJsonObject &json) {
            for (const auto &key: json.keys()) {
                QUuid uuid = QUuid::fromString(key);
                all_classes[uuid] = std::make_shared<model::Classes>(
                    model::Classes::fromJson(uuid, json.value(key).toObject()));
            }
        });
    }

    QHash<QUuid, std::shared_ptr<model::Classes> > get_classes() {
        return all_classes;
    }

    QString add(const std::shared_ptr<model::Classes> &cls) {
        if (!cls) return "内部错误";
        QUuid uuid;
        do {
            uuid = QUuid::createUuid();
        } while (all_classes.contains(uuid));
        all_classes[uuid] = cls;
        return "";
    }

    void removeClass(const QUuid &uuid) {
        all_classes.remove(uuid);
    }

    bool saveClasses() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/classes.json";
        QJsonObject root;
        for (auto it = all_classes.begin(); it != all_classes.end(); ++it) {
            QUuid uuid = it.key();
            root.insert(uuid.toString(QUuid::WithoutBraces), it.value()->toJson());
        }
        return io::save(path, root);
    }

    QVector<model::Classes *> get_all_ptr() {
        QVector<model::Classes *> classes;
        classes.reserve(all_classes.size());
        for (auto it = all_classes.begin(); it != all_classes.end(); ++it) {
            classes.emplace_back(it->get());
        }
        return classes;
    }
}
