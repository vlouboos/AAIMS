// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_PASSWORDRESETDIALOG_H
#define AAIMS_PASSWORDRESETDIALOG_H

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

#include "StyledDialog.h"

class PasswordResetDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit PasswordResetDialog(QWidget *parent = nullptr);

private slots:
    void onCurrentPasswordChanged() const;

    void onNewPasswordChanged() const;

    void onConfirmPasswordChanged() const;

    void onResetPasswordClicked();

private:
    void updateButtonState() const;

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
