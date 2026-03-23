// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StudentPage.h"

#include "delegate/StatusBadgeDelegate.h"

StudentsPage::StudentsPage(QWidget *parent) : QWidget(parent) {
    model = new StudentTableModel();
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(24);

    headerLayout = new QHBoxLayout();
    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("学生档案", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #0f172a;");

    subtitleLabel = new QLabel(QString("管理系统内共 %1 名学生").arg(model->rowCount()), this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);
    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索姓名、学号...");
    searchEdit->setFixedWidth(260);
    searchEdit->setStyleSheet("padding: 8px 12px; border: 1px solid #e2e8f0; border-radius: 6px;");

    btnAdd = new QPushButton("+ 添加学生", this);
    btnAdd->setObjectName("AddElement");

    headerLayout->addWidget(searchEdit);
    headerLayout->addWidget(btnAdd);

    tableView = new QTableView(this);
    tableView->setModel(model);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setShowGrid(true);
    tableView->setGridStyle(Qt::SolidLine);
    tableView->setFocusPolicy(Qt::NoFocus);
    tableView->verticalHeader()->setVisible(false);
    tableView->setAlternatingRowColors(true);

    auto *header = tableView->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionsClickable(true);
    header->setSectionsMovable(true);
    header->setFirstSectionMovable(false);
    header->setDefaultSectionSize(150);
    header->setMinimumSectionSize(150);

    tableView->setItemDelegateForColumn(StudentTableModel::Status, new StatusBadgeDelegate(this));

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tableView);
}
