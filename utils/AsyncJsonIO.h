// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ASYNCJSONIO_H
#define AAIMS_ASYNCJSONIO_H

#include <QtConcurrentRun>
#include <QJsonObject>
#include <QDir>
#include <QSaveFile>

namespace aaims::io {
    static QFuture<void> loadAsync(const QString &filePath, const std::function<void(const QJsonObject &)> &consumer) {
        return QtConcurrent::run([filePath, consumer] {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly)) return;

            if (const QJsonDocument doc = QJsonDocument::fromJson(file.readAll()); doc.isObject()) {
                consumer(doc.object());
            }
        });
    }

    static QFuture<bool> saveAsync(const QString &filePath, const QJsonObject &data) {
        return QtConcurrent::run([filePath, data]() -> bool {
            if (!QFileInfo::exists(filePath) && !QDir().mkpath(QFileInfo(filePath).absolutePath())) return false;
            const QJsonDocument doc(data);
            const QByteArray bytes = doc.toJson();
            if (QSaveFile file(filePath); file.open(QIODevice::WriteOnly)) {
                file.write(bytes);
                return file.commit();
            }
            return false;
        });
    }
}

#endif //AAIMS_ASYNCJSONIO_H
