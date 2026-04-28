// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDMAJORDIALOG_H
#define AAIMS_ADDMAJORDIALOG_H

#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>

#include "StyledDialog.h"


class AddMajorDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddMajorDialog(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QLineEdit *editName;
    QHBoxLayout *deptLayout;
    QCompleter *deptCompleter;
    QComboBox *deptCombo;
    QPushButton *btnAddDept;
    QPushButton *btnConfirm;
};


#endif //AAIMS_ADDMAJORDIALOG_H