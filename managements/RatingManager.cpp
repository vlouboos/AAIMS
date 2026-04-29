// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "RatingManager.h"

#include <QCoreApplication>

#include "../utils/AsyncJsonIO.h"

namespace {
    QHash<QUuid, std::shared_ptr<StudentRating>> ratingMap;
}

namespace aaims::manager::rating {
    void init() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/ratings.json";
        io::load(path, [](const QJsonObject &data) {
            for (const auto &key : data.keys()) {
                QUuid uuid = QUuid::fromString(key);
                ratingMap[uuid] = std::make_shared<StudentRating>(StudentRating::fromJson(uuid, data[key].toObject()));
            }
        });
    }

    QHash<QUuid, std::shared_ptr<StudentRating>> & get_ratings() {
        return ratingMap;
    }
}
