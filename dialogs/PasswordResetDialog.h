// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_PASSWORDRESETDIALOG_H
#define AAIMS_PASSWORDRESETDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "StyledDialog.h"

class PasswordResetDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit PasswordResetDialog(QWidget *parent = nullptr);

private slots:
    void onCurrentPasswordChanged();

    void onNewPasswordChanged();

    void onConfirmPasswordChanged();

    void onResetPasswordClicked();

private:
    void setupUI();

    void updateButtonState();

private:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QFormLayout *formLayout;
    QLineEdit *currentPasswordEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *passwordStatus;
    QHBoxLayout *btnLayout;
    QPushButton *resetBtn;
    QPushButton *cancelBtn;
};

#endif //AAIMS_PASSWORDRESETDIALOG_H
