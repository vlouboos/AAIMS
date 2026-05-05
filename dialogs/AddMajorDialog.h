// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDMAJORDIALOG_H
#define AAIMS_ADDMAJORDIALOG_H

#pragma once
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

#include "StyledDialog.h"

class AddMajorDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddMajorDialog(QWidget *parent = nullptr);

    QPair<unsigned long long, unsigned long long> importFromCsv() const;

private:
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    QWidget *singleAddPage;
    QFormLayout *singleLayout;
    QLineEdit *nameEdit;
    QHBoxLayout *deptLayout;
    QCompleter *completer;
    QComboBox *deptCombo;
    QPushButton *btnAddDept;
    QPushButton *btnConfirmSingle;
    QWidget *batchAddPage;
    QVBoxLayout *batchLayout;
    QLabel *tipLabel;
    QLabel *fileStatusLabel;
    QPushButton *btnSelectFile;
    QPushButton *btnConfirmBatch;
    QString selectedFilePath;
};

#endif //AAIMS_ADDMAJORDIALOG_H
