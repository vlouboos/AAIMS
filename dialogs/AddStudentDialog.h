// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDSTUDENTDIALOG_H
#define AAIMS_ADDSTUDENTDIALOG_H

#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>

#include "StyledDialog.h"

class AddStudentDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AddStudentDialog(QWidget *parent = nullptr);

    ~AddStudentDialog() override = default;

private:
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    QWidget *singleAddPage;
    QFormLayout *singleLayout;
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QComboBox *genderCombo;
    QHBoxLayout *classLayout;
    QCompleter *completer;
    QComboBox *classCombo;
    QPushButton *btnAdd;
    QLineEdit *roomEdit;
    QLineEdit *phoneNumberEdit;
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

#endif //AAIMS_ADDSTUDENTDIALOG_H
