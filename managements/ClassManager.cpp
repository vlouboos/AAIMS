// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "ClassManager.h"

namespace {
    QList<QString> departments;
}

namespace aaims::manager::classes {
    QPair<unsigned long long, unsigned long long> addDepartment(const QVector<QString> &d) {
        unsigned long long succeed = 0, fail = 0;
        for (const auto &department: d) {
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
}
