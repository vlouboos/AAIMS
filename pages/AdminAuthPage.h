// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADMINAUTHPAGE_H
#define AAIMS_ADMINAUTHPAGE_H

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QWidget>

#include "model/AuthTabelModel.h"

class AdminAuthPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminAuthPage(QWidget *parent = nullptr);

    void reloadData() const;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QTableView *tableView;
    AuthTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;

};

#endif //AAIMS_ADMINAUTHPAGE_H
