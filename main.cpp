// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <QApplication>
#include <iostream>

#include "AccountLoginDialog.h"
#include "AccountManager.h"

int main(int argc, char *argv[]) {
    qInfo() << "Loading accounts..." << Qt::endl;
    aaims::manager::account::init();
    QApplication a(argc, argv);

    if (AccountLoginDialog loginDlg; loginDlg.exec() == QDialog::Accepted) {
        qInfo() << "Logged in as " << AccountManager::logged->name;
    }
    return 0;
}
