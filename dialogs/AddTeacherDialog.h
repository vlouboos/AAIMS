// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADDTEACHERDIALOG_H
#define AAIMS_ADDTEACHERDIALOG_H

#include <QLineEdit>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>

class AddTeacherDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddTeacherDialog(QWidget *parent = nullptr);
    ~AddTeacherDialog() override = default;

private:
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    QWidget *singleAddPage;
    QFormLayout *singleLayout;
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *deptEdit;
    QComboBox * genderCombo;
    QLineEdit *phoneNumberEdit;
    QPushButton *btnConfirmSingle;
    QWidget *batchAddPage;
    QVBoxLayout *batchLayout;
    QLabel *tipLabel;
    QLabel *fileStatusLabel;
    QPushButton *btnSelectFile;
    QPushButton *btnConfirmBatch;
    QString selectedFilePath;

    QPair<unsigned long long, unsigned long long> importFromCsv() const;
};

#endif // AAIMS_ADDTEACHERDIALOG_H