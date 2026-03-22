// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ASYNCJSONIO_H
#define AAIMS_ASYNCJSONIO_H

#include <QtConcurrent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QSaveFile>

namespace aaims::io {
    static QFuture<void> loadAsync(const QString &filePath, std::function<void(const QJsonObject &)> consumer) {
        return QtConcurrent::run([filePath, consumer] {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly)) return;

            if (const QJsonDocument doc = QJsonDocument::fromJson(file.readAll()); doc.isObject()) {
                consumer(doc.object());
            }
        });
    }

    template<typename MapType, typename Serializer>
    static QFuture<bool> saveAsync(const QString &filePath, MapType map, Serializer serializeFunc) {
        return QtConcurrent::run([filePath, map, serializeFunc]() -> bool {
            const QJsonArray arr = serializeFunc(map);

            const QJsonDocument doc(arr);

            if (QSaveFile file(filePath); file.open(QIODevice::WriteOnly)) {
                file.write(doc.toJson());
                return file.commit();
            }
            return false;
        });
    }
}

#endif //AAIMS_ASYNCJSONIO_H
