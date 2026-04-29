// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "QualifyCourseDialog.h"

QualifyCourseDialog::QualifyCourseDialog(QWidget *parent) : StyledDialog(parent) {
     setWindowTitle("筛选课程");
     setFixedSize(400, 300);

     // TODO: Open one specific course first, then show capacity, current, exceeded, and one line contains student-info with remove btn.

     applyStyles();
}
