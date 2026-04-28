// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSMANAGER_H
#define AAIMS_CLASSMANAGER_H

#include <qabstractitemmodel.h>
#include <QObject>

#include "../utils/DataStructures.h"

namespace aaims::manager::classes {
    void init();

    QPair<unsigned long long, unsigned long long> addDepartment(const QVector<QString> &dep);

    QList<QString> get_departments();

    bool saveDepartments();

    QHash<QUuid, std::shared_ptr<model::Class> > get_classes();

    void removeClass(const QUuid &uuid);

    bool saveClasses();

    QVector<model::Class *> get_all_ptr();

    QHash<QUuid, std::shared_ptr<model::Major> > get_majors();

    QString add(const std::shared_ptr<model::Class> &cls);

    void addMajor(const std::shared_ptr<model::Major> &major);

    bool saveMajors();
}

#endif //AAIMS_CLASSMANAGER_H
