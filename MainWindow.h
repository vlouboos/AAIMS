// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#pragma once

#include <QMainWindow>
#include <QMenu>
#include <QStackedWidget>
#include <qlabel.h>

#include "pages/AdminDashboardPage.h"
#include "pages/StudentPage.h"
#include "pages/TeacherPage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    enum class CloseReason {
        Exit,
        Logout
    };

    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QWidget *sidebarWidget;
    QVBoxLayout *sidebarLayout;
    QLabel *logoLabel;
    QPushButton *btnDashboard;
    QPushButton *btnTeachers;
    QPushButton *btnStudents;
    QWidget *rightContainer;
    QVBoxLayout *rightLayout;
    QWidget *headerWidget;
    QHBoxLayout *headerLayout;
    QLabel *pageTitleLabel;
    QMenu *menu;
    QAction *settingAction;
    QAction *logoutAction;
    QPushButton *userMenuBtn;
    QStackedWidget *contentStack;
    AdminDashboardPage *adminDashboardPage;
    TeacherPage *teacherPage;
    StudentPage *studentPage;
    CloseReason closeReason = CloseReason::Exit;

protected:
    void closeEvent(QCloseEvent *event) override;
};
