// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ACCOUNTLOGINDIALOG_H
#define AAIMS_ACCOUNTLOGINDIALOG_H

#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <qgraphicseffect.h>

#include "StyledDialog.h"

class AccountLoginDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit AccountLoginDialog(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

    void onTogglePassword();

    void toggleLoginButton() const;

    void switchToPassword() const;

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QVBoxLayout *mainLayout;
    QFrame *card;
    QGraphicsDropShadowEffect *shadow;
    QVBoxLayout *cardLayout;
    QVBoxLayout *headerLayout;
    QLabel *logo;
    QLabel *title;
    QLabel *subtitle;
    QVBoxLayout *formLayout;
    QVBoxLayout *userLayout;
    QLabel *userLabel;
    QLineEdit *userEdit;
    QVBoxLayout *passLayout;
    QLabel *passLabel;
    QFrame *passWrapper;
    QHBoxLayout *passInnerLayout;
    QLineEdit *passEdit;
    QPushButton *togglePassBtn;
    QVBoxLayout *loginLayout;
    QLabel *errorLabel;
    QPushButton *loginBtn;
    QHBoxLayout *footerLayout;
    QPushButton *forgotBtn;
    QPushButton *helpBtn;
    QPushButton *exitButton;
    bool isPasswordVisible = false;
};

#endif //AAIMS_ACCOUNTLOGINDIALOG_H
