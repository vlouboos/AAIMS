// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "ClassManager.h"

#include <qcoreapplication.h>

#include "AccountManager.h"
#include "CourseManager.h"
#include "../utils/AsyncJsonIO.h"
#include "../utils/DataStructures.h"

namespace {
    QList<QString> departments;
    QHash<QUuid, std::shared_ptr<Major> > majors;
    QHash<QUuid, std::shared_ptr<Class> > all_classes;
    QHash<QString, QUuid> majors_by_name;
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

    QList<QString> &get_departments() {
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
        qInfo() << "Loaded" << departments.size() << "departments.";
        path = QCoreApplication::applicationDirPath() + "/data/majors.json";
        io::load(path, [](const QJsonObject &json) {
            for (const auto &key: json.keys()) {
                QUuid uuid = QUuid::fromString(key);
                majors[uuid] = std::make_shared<Major>(Major::fromJson(uuid, json.value(key).toObject()));
            }
        });
        qInfo() << "Loaded" << majors.size() << "majors.";
        path = QCoreApplication::applicationDirPath() + "/data/classes.json";
        io::load(path, [](const QJsonObject &json) {
            for (const auto &key: json.keys()) {
                QUuid uuid = QUuid::fromString(key);
                all_classes[uuid] = std::make_shared<Class>(
                    Class::fromJson(uuid, json.value(key).toObject()));
            }
        });
        qInfo() << "Loaded" << all_classes.size() << "classes.";
    }

    QHash<QUuid, std::shared_ptr<Class> > &get_classes() {
        return all_classes;
    }

    QHash<QUuid, std::shared_ptr<Major> > &get_majors() {
        return majors;
    }

    QString add(const std::shared_ptr<Class> &cls) {
        if (!cls) return "内部错误";
        QUuid uuid;
        do {
            uuid = QUuid::createUuid();
        } while (all_classes.contains(uuid));
        all_classes[uuid] = cls;
        cls->uuid = uuid;
        return "";
    }

    QString addMajor(const std::shared_ptr<Major> &major) {
        if (!major) return "内部错误";
        QUuid uuid;
        do {
            uuid = QUuid::createUuid();
        } while (majors.contains(uuid));
        majors[uuid] = major;
        majors_by_name[major->name] = uuid;
        major->uuid = uuid;
        return "";
    }

    bool saveMajors() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/majors.json";
        QJsonObject root;
        for (auto it = majors.begin(); it != majors.end(); ++it) {
            QUuid uuid = it.key();
            if (!it.value().get()) continue;
            root.insert(uuid.toString(QUuid::WithoutBraces), it.value()->toJson());
        }
        return io::save(path, root);
    }

    Major *findMajorByUuid(const QUuid &uuid) {
        return majors.contains(uuid) ? majors[uuid].get() : nullptr;
    }

    Major *findMajorByName(const QString &name) {
        return majors_by_name.contains(name) ? findMajorByUuid(majors_by_name[name]) : nullptr;
    }

    void removeClass(const QUuid &uuid) {
        using namespace aaims::model;
        const Class *cls = all_classes[uuid].get();
        TeacherAccount *teacher = account::get_teachers()[cls->master];
        teacher->managingClass = EMPTY_UUID;
        teacher->status &= ~Account::CLASS_MASTER;
        for (const auto &course: cls->courses) {
            const auto c = course::get_courses()[course];
            c->classes.removeAll(uuid);
        }
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

    QVector<Class *> get_all_ptr() {
        QVector<Class *> classes;
        classes.reserve(all_classes.size());
        for (auto &cls: all_classes) {
            classes.emplace_back(cls.get());
        }
        return classes;
    }
}
