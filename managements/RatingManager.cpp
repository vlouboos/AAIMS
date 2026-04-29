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
    
    bool save() {
        const QString path = QCoreApplication::applicationDirPath() + "/data/ratings.json";
        QJsonObject root;
        for (const auto &[uuid, rating] : ratingMap.asKeyValueRange()) {
            QJsonObject ratingObj;
            QJsonObject ratingsArray;
            
            for (const auto &[courseUuid, ratingDetail] : rating->ratings.asKeyValueRange()) {
                QJsonObject detailObj;
                detailObj["performance"] = ratingDetail.performance;
                detailObj["score"] = ratingDetail.score;
                detailObj["finalScore"] = ratingDetail.finalScore;
                ratingsArray[courseUuid.toString(QUuid::WithoutBraces)] = detailObj;
            }
            
            ratingObj["ratings"] = ratingsArray;
            root[uuid.toString(QUuid::WithoutBraces)] = ratingObj;
        }
        
        return io::save(path, root);
    }
}