// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AccountLoginDialog.h"

#include <qcoreevent.h>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QStyle>
#include <QTextStream>

#include "AccountManager.h"
#include "sha256util.h"

AccountLoginDialog::AccountLoginDialog(QWidget *parent) : QDialog(parent) {
    this->setFixedSize(450, 600);
    this->setWindowTitle("AAIMS Login");
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    card = new QFrame(this);
    card->setObjectName("Card");
    card->setFixedSize(400, 520);
    shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 40));
    shadow->setOffset(0, 10);
    card->setGraphicsEffect(shadow);

    cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 40, 32, 32);
    cardLayout->setSpacing(24);

    headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(8);

    logo = new QLabel("🎓", card);
    logo->setObjectName("Logo");
    logo->setAlignment(Qt::AlignCenter);

    title = new QLabel("AAIMS Secure Login", card);
    title->setObjectName("Title");
    title->setAlignment(Qt::AlignCenter);

    subtitle = new QLabel("教务信息管理系统", card);
    subtitle->setObjectName("Subtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(logo);
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    cardLayout->addLayout(headerLayout);

    formLayout = new QVBoxLayout();
    formLayout->setSpacing(20);

    userLayout = new QVBoxLayout();
    userLayout->setSpacing(8);
    userLabel = new QLabel("用户名", card);
    userLabel->setObjectName("FieldLabel");
    userEdit = new QLineEdit(card);
    userEdit->setPlaceholderText("请输入用户名");
    userLayout->addWidget(userLabel);
    userLayout->addWidget(userEdit);

    passLayout = new QVBoxLayout();
    passLayout->setSpacing(8);
    passLabel = new QLabel("密码", card);
    passLabel->setObjectName("FieldLabel");

    passWrapper = new QFrame(card);
    passWrapper->setObjectName("PassWrapper");
    passInnerLayout = new QHBoxLayout(passWrapper);
    passInnerLayout->setContentsMargins(0, 0, 8, 0);
    passInnerLayout->setSpacing(0);

    passEdit = new QLineEdit(passWrapper);
    passEdit->setPlaceholderText("请输入密码");
    passEdit->setEchoMode(QLineEdit::Password);
    passEdit->setObjectName("PassEdit");

    togglePassBtn = new QPushButton(passWrapper);
    togglePassBtn->setObjectName("ToggleBtn");
    togglePassBtn->setIcon(QIcon(":/assets/hide.svg"));
    togglePassBtn->setCursor(Qt::PointingHandCursor);

    passInnerLayout->addWidget(passEdit);
    passInnerLayout->addWidget(togglePassBtn);

    passLayout->addWidget(passLabel);
    passLayout->addWidget(passWrapper);

    loginLayout = new QVBoxLayout();
    loginLayout->setSpacing(0);
    errorLabel = new QLabel("用户名或密码错误", card);
    errorLabel->setObjectName("RedFieldLabel");
    errorLabel->hide();
    errorLabel->setContentsMargins(4, 0, 0, 0);
    loginLayout->addWidget(errorLabel);

    loginBtn = new QPushButton("登录", card);
    loginBtn->setObjectName("LoginBtn");
    loginBtn->setCursor(Qt::ForbiddenCursor);
    loginBtn->setProperty("isReady", false);
    loginBtn->addAction(new QAction());
    loginLayout->addWidget(loginBtn);

    formLayout->addLayout(userLayout);
    formLayout->addLayout(passLayout);
    formLayout->addSpacing(10);
    formLayout->addLayout(loginLayout);
    cardLayout->addLayout(formLayout);

    footerLayout = new QHBoxLayout();
    footerLayout->setAlignment(Qt::AlignCenter);
    footerLayout->setSpacing(32);

    forgotBtn = new QPushButton("🔑 忘记密码?", card);
    helpBtn = new QPushButton("❓ 帮助中心", card);
    forgotBtn->setObjectName("LinkBtn");
    helpBtn->setObjectName("LinkBtn");
    forgotBtn->setCursor(Qt::PointingHandCursor);
    helpBtn->setCursor(Qt::PointingHandCursor);

    footerLayout->addWidget(forgotBtn);
    footerLayout->addWidget(helpBtn);

    cardLayout->addStretch();
    cardLayout->addLayout(footerLayout);
    exitButton = new QPushButton("×", card); // 放在 card 上而不是 this
    exitButton->setObjectName("ExitButton");
    exitButton->setFixedSize(30, 30);
    exitButton->move(360, 10);
    mainLayout->addWidget(card);

    applyStyles();

    connect(loginBtn, &QPushButton::clicked, this, &AccountLoginDialog::onLoginClicked);
    connect(togglePassBtn, &QPushButton::clicked, this, &AccountLoginDialog::onTogglePassword);
    connect(exitButton, &QPushButton::clicked, this, &AccountLoginDialog::close);
    connect(userEdit, &QLineEdit::textChanged, this, &AccountLoginDialog::toggleLoginButton);
    connect(passEdit, &QLineEdit::textChanged, this, &AccountLoginDialog::toggleLoginButton);
    passEdit->installEventFilter(this);
}

bool AccountLoginDialog::eventFilter(QObject *obj, QEvent *event) {
    if (obj == passEdit) {
        if (event->type() == QEvent::FocusIn) {
            passWrapper->setProperty("focused", true);
            passWrapper->style()->unpolish(passWrapper);
            passWrapper->style()->polish(passWrapper);
        } else if (event->type() == QEvent::FocusOut) {
            passWrapper->setProperty("focused", false);
            passWrapper->style()->unpolish(passWrapper);
            passWrapper->style()->polish(passWrapper);
        }
    }
    return QDialog::eventFilter(obj, event);
}

void AccountLoginDialog::applyStyles() {
    QFile file(":/assets/style.qss");

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&file);
        this->setStyleSheet(ts.readAll());
        file.close();
    } else {
        qDebug() << "Failed to open QSS file!";
    }
}

void AccountLoginDialog::onTogglePassword() {
    isPasswordVisible = !isPasswordVisible;
    passEdit->setEchoMode(isPasswordVisible ? QLineEdit::Normal : QLineEdit::Password);
    togglePassBtn->setProperty("passwordVisible", isPasswordVisible);
    togglePassBtn->style()->unpolish(togglePassBtn);
    togglePassBtn->style()->polish(togglePassBtn);
}

void AccountLoginDialog::toggleLoginButton() const {
    if (!userEdit->text().isEmpty() && !passEdit->text().isEmpty()) {
        loginBtn->setCursor(Qt::PointingHandCursor);
        loginBtn->setProperty("isReady", true);
    } else {
        loginBtn->setCursor(Qt::ForbiddenCursor);
        loginBtn->setProperty("isReady", false);
    }
    loginBtn->style()->unpolish(loginBtn);
    loginBtn->style()->polish(loginBtn);
}

void AccountLoginDialog::onLoginClicked() {
    if (!loginBtn->property("isReady").toBool()) return;
    const QString u = userEdit->text();
    const QString p = Sha256Util::hash(passEdit->text());
    if (Account *a = AccountManager::tryLogin(u.toStdString(), p.toStdString()); !a) {
        errorLabel->show();
    } else {
        AccountManager::logged = a;
        accept();
    }
}
