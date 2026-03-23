// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STUDENTPAGE_H
#define AAIMS_STUDENTPAGE_H

#include <QLabel>
#include <QVBoxLayout>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>

#include "model/StudentTableModel.h"

class StudentsPage : public QWidget {
    Q_OBJECT

public:
    explicit StudentsPage(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QTableView *tableView;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QPushButton *btnAdd;
    StudentTableModel *model;
};

#endif //AAIMS_STUDENTPAGE_H
