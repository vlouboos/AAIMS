// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_OOBE_H
#define AAIMS_OOBE_H

#include <qgraphicseffect.h>
#include <QLineEdit>
#include <QLabel>

#include "StyledDialog.h"

class oobeDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit oobeDialog(QWidget *parent = nullptr);

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
    QVBoxLayout *confirmLayout;
    QLabel *confirmLabel;
    QFrame *confirmWrapper;
    QHBoxLayout *confirmInnerLayout;
    QLineEdit *confirmEdit;
    QPushButton *togglePassBtn;
    QPushButton *toggleConfirmBtn;
    QFrame *tipCard;
    QHBoxLayout *tipLayout;
    QLabel *tipIcon;
    QVBoxLayout *tipContentLayout;
    QLabel *tipTitle;
    QLabel *tipContent;
    QVBoxLayout *registerLayout;
    QPushButton *registerBtn;
    QLabel *errorLabel;
    QPushButton *exitButton;
    bool isPassVisible = false;
    bool isConfirmVisible = false;

    void toggleRegisterButton() const;

private slots:
    void registerClicked();
};

#endif
