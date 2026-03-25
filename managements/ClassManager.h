// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSMANAGER_H
#define AAIMS_CLASSMANAGER_H

#include <qabstractitemmodel.h>
#include <QObject>

namespace aaims::manager::classes {
    class InternalManager : public QObject {
        Q_OBJECT

    public:
        static InternalManager *instance() {
            static InternalManager inst;
            return &inst;
        }
    };

    QPair<unsigned long long, unsigned long long> addDepartment(const QVector<QString> &dep);

    QList<QString> get_departments();

    bool saveDepartments();

    void init();
}

#endif //AAIMS_CLASSMANAGER_H
