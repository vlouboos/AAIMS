// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <QApplication>
#include <QCommandLineParser>

#include "dialogs/AccountLoginDialog.h"
#include "dialogs/oobeDialog.h"
#include "managements/AccountManager.h"
#include "MainWindow.h"
#include "managements/ClassManager.h"

int main(int argc, char *argv[]) {
    const QApplication a(argc, argv);
    QApplication::setApplicationName("AAIMS");

    qInfo() << "Loading departments & classes...";
    aaims::manager::classes::init();
    qInfo() << "Loading accounts...";
    aaims::manager::account::init();

    while (aaims::manager::account::isLoading()) {
        QCoreApplication::processEvents();
    }
    if (aaims::manager::account::isEmpty()) {
        qInfo() << "No account exists!";
        if (oobeDialog startDialog; startDialog.exec() == QDialog::Rejected) {
            qCritical() << "No account exists and user denied to register!";
            exit(0);
        }
    }
    while (aaims::restart) {
        if (aaims::manager::account::logged == nullptr) {
            if (AccountLoginDialog loginDlg; loginDlg.exec() == QDialog::Rejected) {
                QThreadPool::globalInstance()->waitForDone();
                exit(0);
            }
            qInfo() << "Logged in as" << aaims::manager::account::logged->name;
        }
        MainWindow main_window;
        main_window.show();
        qInfo() << "Exited with code" << QApplication::exec();
    }
    QThreadPool::globalInstance()->waitForDone();
    return 0;
}
