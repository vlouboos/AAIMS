// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_RATINGMANAGER_H
#define AAIMS_RATINGMANAGER_H

#include "../utils/DataStructures.h"

using namespace aaims::model;

namespace aaims::manager::rating {
    void init();

    QHash<QUuid, std::shared_ptr<StudentRating>> &get_ratings();
}

#endif //AAIMS_RATINGMANAGER_H
