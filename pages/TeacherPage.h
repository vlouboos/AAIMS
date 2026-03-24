// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TEACHERPAGE_H
#define AAIMS_TEACHERPAGE_H

#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>

#include "model/TeacherTableModel.h"

// 引入你的 Model

class TeacherPage : public QWidget {
    Q_OBJECT

public:
    explicit TeacherPage(QWidget *parent = nullptr);

    ~TeacherPage() override = default;

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
};

#endif //AAIMS_TEACHERPAGE_H
