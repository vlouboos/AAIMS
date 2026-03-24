// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "MainWindow.h"

#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <qthreadpool.h>

#include "managements/AccountManager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    const Account *loggedAccount = aaims::manager::account::logged;
    assert(loggedAccount);
    resize(1200, 800);
    setMinimumSize(1200, 800);
    setWindowTitle("AAIMS - 教务信息管理系统");
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    sidebarWidget = new QWidget(centralWidget);
    sidebarWidget->setObjectName("Sidebar");
    sidebarWidget->setFixedWidth(240);

    sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(16, 24, 16, 24);
    sidebarLayout->setSpacing(8);

    logoLabel = new QLabel("🎓 AAIMS", sidebarWidget);
    logoLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px;");

    btnDashboard = new QPushButton("仪表盘", sidebarWidget);
    btnDashboard->setProperty("class", "category-button");
    btnDashboard->setCheckable(true);
    btnDashboard->setChecked(true);
    if (loggedAccount->is_master() || loggedAccount->is_admin()) {
        btnTeachers = new QPushButton("教师", sidebarWidget);
        btnTeachers->setProperty("class", "category-button");
        btnTeachers->setCheckable(true);

        btnStudents = new QPushButton("学生", sidebarWidget);
        btnStudents->setProperty("class", "category-button");
        btnStudents->setCheckable(true);
    }

    sidebarLayout->addWidget(logoLabel);
    sidebarLayout->addWidget(btnDashboard);
    if (loggedAccount->is_master() || loggedAccount->is_admin()) {
        sidebarLayout->addWidget(btnTeachers);
        sidebarLayout->addWidget(btnStudents);
    }
    sidebarLayout->addStretch();

    rightContainer = new QWidget(centralWidget);
    rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    headerWidget = new QWidget(rightContainer);
    headerWidget->setObjectName("Header");
    headerWidget->setFixedHeight(64);

    headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(24, 0, 24, 0);

    pageTitleLabel = new QLabel("仪表盘", headerWidget);
    pageTitleLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #1e293b;");

    menu = new QMenu(this);

    settingAction = new QAction("个人设置", menu);
    logoutAction = new QAction("退出登录", menu);

    menu->addAction(settingAction);
    menu->addSeparator();
    menu->addAction(logoutAction);

    userMenuBtn = new QPushButton(headerWidget);
    userMenuBtn->setText(loggedAccount->name + '(' + loggedAccount->username + ") ");
    // Add a space to prevent something strange from happening.
    if (loggedAccount->is_master()) {
        userMenuBtn->setStyleSheet("font-size: 12px; font-weight: 600; border: 0; color: #AA0000;");
    } else if (loggedAccount->is_admin()) {
        userMenuBtn->setStyleSheet("font-size: 12px; font-weight: 600; border: 0; color: #FF5555;");
    } else if (loggedAccount->is_teacher()) {
        userMenuBtn->setStyleSheet("font-size: 12px; font-weight: 600; border: 0; color: #55FF55;");
    } else if (loggedAccount->is_graduated()) {
        userMenuBtn->setStyleSheet("font-size: 12px; font-weight: 600; border: 0; color: #555555;");
    } else {
        userMenuBtn->setStyleSheet("font-size: 12px; font-weight: 600; border: 0; color: #000000;");
    }
    userMenuBtn->setMenu(menu);

    headerLayout->addWidget(pageTitleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(userMenuBtn);

    contentStack = new QStackedWidget(rightContainer);
    contentStack->setObjectName("ContentArea");
    contentStack->setCurrentIndex(0);

    if (loggedAccount->is_master() || loggedAccount->is_admin()) {
        adminDashboardPage = new AdminDashboardPage(contentStack);
        teacherPage = new TeacherPage(contentStack);
        studentPage = new StudentPage(contentStack);
    }

    contentStack->addWidget(adminDashboardPage);
    contentStack->addWidget(teacherPage);
    contentStack->addWidget(studentPage);

    rightLayout->addWidget(headerWidget);
    rightLayout->addWidget(contentStack);

    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(rightContainer);

    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }

    connect(btnDashboard, &QPushButton::clicked, [this] {
        contentStack->setCurrentIndex(0);
        pageTitleLabel->setText("仪表盘");
        btnDashboard->setChecked(true);
        btnTeachers->setChecked(false);
        btnStudents->setChecked(false);
    });
    if (loggedAccount->is_master() || loggedAccount->is_admin()) {
        connect(btnTeachers, &QPushButton::clicked, [this] {
            contentStack->setCurrentIndex(1);
            pageTitleLabel->setText("教师");
            btnDashboard->setChecked(false);
            btnTeachers->setChecked(true);
            btnStudents->setChecked(false);
        });
        connect(btnStudents, &QPushButton::clicked, [this] {
            contentStack->setCurrentIndex(2);
            pageTitleLabel->setText("学生");
            btnDashboard->setChecked(false);
            btnTeachers->setChecked(false);
            btnStudents->setChecked(true);
        });
    }
    connect(logoutAction, &QAction::triggered, [this] {
        menu->hide();
        const auto result = QMessageBox::question(this, "退出登录", "确定要退出登录吗？",
                                                  QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            closeReason = CloseReason::Logout;
            QThreadPool::globalInstance()->waitForDone();
            aaims::manager::account::logged = nullptr;
            this->close();
        }
    });
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (closeReason == CloseReason::Logout) {
        event->accept();
        return;
    }
    const auto result = QMessageBox::question(this, "退出", "确定要退出吗？",
                                              QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        QThreadPool::globalInstance()->waitForDone();
        aaims::restart = false;
        event->accept();
    } else {
        event->ignore();
    }
}
