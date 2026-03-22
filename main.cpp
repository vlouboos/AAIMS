// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <QApplication>
#include <iostream>

#include "AccountLoginDialog.h"
#include "oobeDialog.h"
#include "AccountManager.h"

int main(int argc, char *argv[]) {
    const QApplication a(argc, argv);
    qInfo() << "Loading accounts...";
    aaims::manager::account::init();

    while (aaims::manager::account::isLoading()) {
        QCoreApplication::processEvents();
    }
    if (aaims::manager::account::isEmpty()) {
        qInfo() << "No account exists!";
        if (oobeDialog startDialog; startDialog.exec() == QDialog::Rejected) {
            qCritical() << "No account exists and user denied to register!";
            return 0;
        }
    }
    if (aaims::manager::account::logged == nullptr) {
        if (AccountLoginDialog loginDlg; loginDlg.exec() == QDialog::Accepted) {
            qInfo() << "Logged in as" << aaims::manager::account::logged->name;
        }
    }
    return 0;
}
