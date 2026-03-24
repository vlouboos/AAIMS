// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "oobeDialog.h"

#include <QFutureWatcher>

#include "../utils/Sha256Util.h"
#include <QGraphicsDropShadowEffect>
#include <QProgressDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>

#include "../managements/AccountManager.h"

oobeDialog::oobeDialog(QWidget *parent) : StyledDialog(parent) {
    setFixedSize(450, 780);
    setWindowTitle("AAIMS Initialization");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    card = new QFrame();
    card->setObjectName("Card");
    card->setFixedSize(400, 700);
    shadow = new QGraphicsDropShadowEffect();
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

    title = new QLabel("初始化 AAIMS", card);
    title->setObjectName("Title");
    title->setAlignment(Qt::AlignCenter);

    subtitle = new QLabel("AAIMS中没有账号，创建一个主管理员账户以继续", card);
    subtitle->setObjectName("Subtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(logo);
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);

    formLayout = new QVBoxLayout();

    userLayout = new QVBoxLayout();
    userLayout->setSpacing(8);

    userLabel = new QLabel("管理员用户名", card);

    userEdit = new QLineEdit(card);
    userEdit->setPlaceholderText("请不要使用admin, root等常见用户名");

    userLayout->addWidget(userLabel);
    userLayout->addWidget(userEdit);

    passLayout = new QVBoxLayout();
    passLayout->setSpacing(8);

    passLabel = new QLabel("密码", card);

    passWrapper = new QFrame(card);
    passWrapper->setObjectName("PassWrapper");

    passInnerLayout = new QHBoxLayout(passWrapper);
    passInnerLayout->setContentsMargins(0, 0, 8, 0);
    passInnerLayout->setSpacing(0);

    passEdit = new QLineEdit(passWrapper);
    passEdit->setPlaceholderText("请设置密码");
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

    confirmLayout = new QVBoxLayout();
    confirmLayout->setSpacing(8);

    confirmLabel = new QLabel("重复密码", card);

    confirmWrapper = new QFrame(card);
    confirmWrapper->setObjectName("PassWrapper");

    confirmInnerLayout = new QHBoxLayout(confirmWrapper);
    confirmInnerLayout->setContentsMargins(0, 0, 8, 0);
    confirmInnerLayout->setSpacing(0);

    confirmEdit = new QLineEdit(confirmWrapper);
    confirmEdit->setPlaceholderText("请输入刚才输入的密码");
    confirmEdit->setEchoMode(QLineEdit::Password);
    confirmEdit->setObjectName("PassEdit");

    toggleConfirmBtn = new QPushButton(confirmWrapper);
    toggleConfirmBtn->setObjectName("ToggleBtn");
    toggleConfirmBtn->setIcon(QIcon(":/assets/hide.svg"));
    toggleConfirmBtn->setCursor(Qt::PointingHandCursor);

    confirmInnerLayout->addWidget(confirmEdit);
    confirmInnerLayout->addWidget(toggleConfirmBtn);

    confirmLayout->addWidget(confirmLabel);
    confirmLayout->addWidget(confirmWrapper);

    formLayout->addLayout(userLayout);
    formLayout->addLayout(passLayout);
    formLayout->addLayout(confirmLayout);

    tipCard = new QFrame(card);
    tipCard->setObjectName("SecurityTipCard");

    tipLayout = new QHBoxLayout(tipCard);
    tipLayout->setAlignment(Qt::AlignTop);

    tipIcon = new QLabel("⚠️", tipCard);

    tipContentLayout = new QVBoxLayout();

    tipTitle = new QLabel("提醒", tipCard);
    tipTitle->setObjectName("TipTitle");

    tipContent = new QLabel("这是唯一的管理员主账号，请妥善保管您的登录\n凭证。", tipCard);
    tipContent->setObjectName("TipContent");

    tipContentLayout->addWidget(tipTitle);
    tipContentLayout->addWidget(tipContent);

    tipLayout->addWidget(tipIcon);
    tipLayout->addLayout(tipContentLayout);

    registerLayout = new QVBoxLayout();
    registerLayout->setSpacing(0);

    errorLabel = new QLabel("两次输入的密码不一致", card);
    errorLabel->setObjectName("RedFieldLabel");
    errorLabel->hide();
    errorLabel->setContentsMargins(4, 0, 0, 0);

    registerBtn = new QPushButton("保存", card);
    registerBtn->setObjectName("LoginBtn");
    registerBtn->setCursor(Qt::ForbiddenCursor);
    registerBtn->setProperty("isReady", false);
    registerBtn->addAction(new QAction());

    registerLayout->addWidget(errorLabel);
    registerLayout->addWidget(registerBtn);

    cardLayout->addLayout(headerLayout);
    cardLayout->addLayout(formLayout);
    cardLayout->addWidget(tipCard);
    cardLayout->addLayout(registerLayout);

    exitButton = new QPushButton("×", card);
    exitButton->setObjectName("ExitButton");
    exitButton->setFixedSize(30, 30);
    exitButton->move(360, 10);

    mainLayout->addWidget(card);
    applyStyles();

    connect(exitButton, &QPushButton::clicked, this, &oobeDialog::reject);
    connect(userEdit, &QLineEdit::textChanged, this, &oobeDialog::toggleRegisterButton);
    connect(passEdit, &QLineEdit::textChanged, this, &oobeDialog::toggleRegisterButton);
    connect(confirmEdit, &QLineEdit::textChanged, this, &oobeDialog::toggleRegisterButton);
    connect(togglePassBtn, &QPushButton::clicked, this, &oobeDialog::onTogglePassword);
    connect(toggleConfirmBtn, &QPushButton::clicked, this, &oobeDialog::onToggleConfirm);
    connect(registerBtn, &QPushButton::clicked, this, &oobeDialog::registerClicked);
}

void oobeDialog::onTogglePassword() {
    isPassVisible = !isPassVisible;
    passEdit->setEchoMode(isPassVisible ? QLineEdit::Normal : QLineEdit::Password);
    togglePassBtn->setProperty("passwordVisible", isPassVisible);
    togglePassBtn->style()->unpolish(togglePassBtn);
    togglePassBtn->style()->polish(togglePassBtn);
}

void oobeDialog::onToggleConfirm() {
    isConfirmVisible = !isConfirmVisible;
    confirmEdit->setEchoMode(isConfirmVisible ? QLineEdit::Normal : QLineEdit::Password);
    toggleConfirmBtn->setProperty("passwordVisible", isConfirmVisible);
    toggleConfirmBtn->style()->unpolish(toggleConfirmBtn);
    toggleConfirmBtn->style()->polish(toggleConfirmBtn);
}

void oobeDialog::toggleRegisterButton() const {
    if (!userEdit->text().isEmpty() && !passEdit->text().isEmpty() && !confirmEdit->text().isEmpty()) {
        registerBtn->setCursor(Qt::PointingHandCursor);
        registerBtn->setProperty("isReady", true);
    } else {
        registerBtn->setCursor(Qt::ForbiddenCursor);
        registerBtn->setProperty("isReady", false);
    }
    registerBtn->style()->unpolish(registerBtn);
    registerBtn->style()->polish(registerBtn);
}

void oobeDialog::registerClicked() {
    if (!registerBtn->property("isReady").toBool()) return;
    if (passEdit->text() != confirmEdit->text()) {
        errorLabel->show();
        return;
    }
    qInfo() << "Creating root account" << userEdit->text();
    const auto loading = new QProgressDialog("正在创建主管理员账户...", nullptr, 0, 0, this); // NOLINT
    loading->setWindowModality(Qt::WindowModal);
    loading->setCancelButton(nullptr);
    loading->setRange(0, 0);
    loading->show();
    QUuid uuid = QUuid::createUuid();
    const auto masterAccount = std::make_shared<Account>();
    masterAccount->uuid = uuid;
    masterAccount->username = userEdit->text();
    masterAccount->name = "主管理员";
    masterAccount->password = Sha256Util::hash(passEdit->text());
    masterAccount->female = false;
    masterAccount->status = Account::MASTER;
    aaims::manager::account::add(masterAccount);
    const auto future = aaims::manager::account::saveAsync();

    auto *watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcher<bool>::finished, [this, loading, watcher, uuid] {
        loading->close();
        loading->deleteLater();
        watcher->deleteLater();

        if (watcher->result()) {
            QMessageBox::information(this, "成功", "已成功创建主管理员账号。");
            aaims::manager::account::logged = aaims::manager::account::findByUUID(uuid);
            this->accept();
        } else {
            QMessageBox::critical(this, "错误", "无法保存账号数据，请检查磁盘权限。");
        }
    });

    watcher->setFuture(future);
}
