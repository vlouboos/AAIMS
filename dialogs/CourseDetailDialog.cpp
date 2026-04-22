// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "CourseDetailDialog.h"

CourseDetailDialog::CourseDetailDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("课程详情");
    resize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    mainLayout = new QVBoxLayout(this);
}
