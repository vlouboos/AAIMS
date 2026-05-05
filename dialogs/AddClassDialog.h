// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDCLASSDIALOG_H
#define AAIMS_ADDCLASSDIALOG_H

#pragma once
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

#include "StyledDialog.h"

class AddClassDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddClassDialog(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    QWidget *singleAddPage;
    QFormLayout *singleLayout;
    QLineEdit *gradeEdit;
    QLineEdit *nameEdit;
    QHBoxLayout *majorLayout;
    QCompleter *majorCompleter;
    QComboBox *majorCombo;
    QPushButton *btnAddMajor;
    QHBoxLayout *masterLayout;
    QCompleter *masterCompleter;
    QComboBox *masterCombo;
    QPushButton *btnAddTeacher;
    QPushButton *btnConfirmSingle;
    QWidget *batchAddPage;
    QVBoxLayout *batchLayout;
    QLabel *tipLabel;
    QLabel *fileStatusLabel;
    QPushButton *btnSelectFile;
    QPushButton *btnConfirmBatch;
    QString selectedFilePath;

    [[nodiscard]] QPair<unsigned long long, unsigned long long> importFromCsv() const;
};

#endif //AAIMS_ADDCLASSDIALOG_H
