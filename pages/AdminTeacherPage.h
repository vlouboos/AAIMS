// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ADMINTEACHERPAGE_H
#define AAIMS_ADMINTEACHERPAGE_H

#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QSortFilterProxyModel>

#include "model/TeacherTableModel.h"

class AdminTeacherPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminTeacherPage(QWidget *parent = nullptr);

    ~AdminTeacherPage() override = default;

    void reloadData() const;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *searchEdit;
    QPushButton *btnAddTeacher;
    QTableView *tableView;
    TeacherTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;
};

#endif //AAIMS_ADMINTEACHERPAGE_H
