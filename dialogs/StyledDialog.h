// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STYLEDDIALOG_H
#define AAIMS_STYLEDDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QFrame>

class StyledDialog : public QDialog {
    Q_OBJECT
public:
    explicit StyledDialog(QWidget *parent = nullptr);

protected:
    void applyStyles();
};

#endif //AAIMS_STYLEDDIALOG_H
