// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSPAGE_H
#define AAIMS_CLASSPAGE_H

#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/ClassTableModel.h"

class ClassPage : public QWidget {
    Q_OBJECT

public:
    explicit ClassPage(QWidget *parent = nullptr);

    ~ClassPage() override = default;

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

#endif //AAIMS_CLASSPAGE_H