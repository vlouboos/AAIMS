// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDDEPARTMENTDIALOG_H
#define AAIMS_ADDDEPARTMENTDIALOG_H

#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>
#include "StyledDialog.h"

class AddDepartmentDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddDepartmentDialog(QWidget *parent = nullptr);

    ~AddDepartmentDialog() override = default;

private:
    QVBoxLayout *mainLayout;
    QLabel *label;
    QPlainTextEdit *edit;
    QPushButton *btnConfirm;
};

#endif //AAIMS_ADDDEPARTMENTDIALOG_H
