// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_TEACHERCLASSPAGE_H
#define AAIMS_TEACHERCLASSPAGE_H

#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QProgressDialog>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QtConcurrent>

#include "model/ClassMemberTableModel.h"

class TeacherClassPage : public QWidget {
    Q_OBJECT

public:
    explicit TeacherClassPage(QWidget *parent = nullptr);

private slots:
    void onExportGrades();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QPushButton *btnExportGrades;
    QTableView *tableView;
    ClassMemberTableModel *tableModel;

    void setupUI();

    void loadClassData() const;
};

#endif //AAIMS_TEACHERCLASSPAGE_H
