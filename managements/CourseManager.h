// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_COURSEMANAGER_H
#define AAIMS_COURSEMANAGER_H

#include "../utils/DataStructures.h"

using namespace aaims::model;

namespace aaims::manager::course {
    void init();

    QString add(const std::shared_ptr<Course> &course);

    void remove(const Course *course);

    QHash<QUuid, std::shared_ptr<Course>> get_courses();

    bool save();
}

#endif //AAIMS_COURSEMANAGER_H
