// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STUDENTPAGE_H
#define AAIMS_STUDENTPAGE_H

#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/StudentTableModel.h"
#include "model/TeacherTableModel.h"

class StudentPage : public QWidget {
    Q_OBJECT

public:
    explicit StudentPage(QWidget *parent = nullptr);

    ~StudentPage() override = default;

    void reloadData() const;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QPushButton *btnAddStudent;
    QTableView *tableView;
    StudentTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;
};

#endif //AAIMS_STUDENTPAGE_H
