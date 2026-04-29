// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_QUALIFYCOURSEDIALOG_H
#define AAIMS_QUALIFYCOURSEDIALOG_H

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class QualifyCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit QualifyCourseDialog(QList<aaims::model::Course *> qualifyingCourses, QWidget *parent = nullptr);
};


#endif //AAIMS_QUALIFYCOURSEDIALOG_H
