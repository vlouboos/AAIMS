// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "CourseManager.h"
#include "../utils/AsyncJsonIO.h"

#include <QApplication>

namespace {
    QHash<QUuid, std::shared_ptr<Course>> courses;
}

namespace aaims::manager::course {
    void init() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/courses.json";
        io::load(path, [](const QJsonObject &json) {
            for (const auto &key : json.keys()) {
                QUuid uuid = QUuid::fromString(key);
                courses[uuid] = std::make_shared<Course>(Course::fromJson(uuid, json.value(key).toObject()));
            }
        });
    }

    QString add(const std::shared_ptr<Course> &course) {
        if (!course.get()) return "内部错误";
        QUuid uuid;
        do {
            uuid = QUuid::createUuid();
        } while (courses.contains(uuid) || uuid == EMPTY_UUID);
        course->uuid = uuid;
        courses[uuid] = course;
        return "";
    }

    void remove(const Course *course) {
        courses.remove(course->uuid);
    }
}
