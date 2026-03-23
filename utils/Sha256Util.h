// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_SHA256UTIL_H
#define AAIMS_SHA256UTIL_H

#include <QString>
#include <QCryptographicHash>

class Sha256Util {
public:
    static QString hash(const QString &password) {
        if (password.isEmpty()) return "";
        const QByteArray data = password.toUtf8();
        const QByteArray hashed = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
        return hashed.toHex();
    }

};

#endif //AAIMS_SHA256UTIL_H