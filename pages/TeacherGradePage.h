// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TEACHERGRADEPAGE_H
#define AAIMS_TEACHERGRADEPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include "model/GradeTableModel.h"

class TeacherGradePage : public QWidget {
    Q_OBJECT

public:
    explicit TeacherGradePage(QWidget *parent = nullptr);

    void reloadData() const;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QTableView *tableView;
    GradeTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;
};


#endif //AAIMS_TEACHERGRADEPAGE_H
