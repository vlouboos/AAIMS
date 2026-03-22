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
    aaims::manager::account::onLoaded([&] {
        if (aaims::manager::account::isEmpty()) {
            qInfo() << "No account exists!";
            if (oobeDialog startDialog; startDialog.exec() != QDialog::Accepted) {
                qCritical() << "No account exists and user denied to register!";
                return a.exec();
            }
        }
        if (AccountLoginDialog loginDlg; loginDlg.exec() == QDialog::Accepted) {
            qInfo() << "Logged in as" << aaims::manager::account::logged->name;
        }
        return 0;
    });
    aaims::manager::account::init();
    return a.exec();
}
