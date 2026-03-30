// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADMINCLASSPAGE_H
#define AAIMS_ADMINCLASSPAGE_H

#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/ClassTableModel.h"

class AdminClassPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminClassPage(QWidget *parent = nullptr);

    void reloadData() const;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QPushButton *btnAddClass;
    QTableView *tableView;
    ClassTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;
};

#endif //AAIMS_ADMINCLASSPAGE_H