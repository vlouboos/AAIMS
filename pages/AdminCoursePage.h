// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADMINCOURSEPAGE_H
#define AAIMS_ADMINCOURSEPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include "model/CourseTableModel.h"

class AdminCoursePage : public QWidget {
    Q_OBJECT

public:
    explicit AdminCoursePage(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QPushButton *btnAddStudent;
    QTableView *tableView;
    CourseTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;

    void reloadData() const;
};

#endif //AAIMS_ADMINCOURSEPAGE_H
