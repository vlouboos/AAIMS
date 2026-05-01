// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_GRADEEDITDIALOG_H
#define AAIMS_GRADEEDITDIALOG_H

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTabWidget>

#include "StyledDialog.h"
#include "../pages/model/SingleGradeTableModel.h"
#include "../utils/DataStructures.h"

class GradeEditDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit GradeEditDialog(const QUuid &courseUuid, QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QFormLayout *infoLayout;
    QWidget *infoWidget;
    QLabel *courseIdLabel;
    QLabel *courseNameLabel;

    QTabWidget *tabWidget;
    QWidget *singleEditPage;
    QVBoxLayout *singleLayout;
    QTableView *tableView;
    SingleGradeTableModel *tableModel;
    QSortFilterProxyModel *proxyModel;

    QUuid courseUuid;
    QList<QUuid> studentUuids; // List of students in this course
};

#endif //AAIMS_GRADEEDITDIALOG_H