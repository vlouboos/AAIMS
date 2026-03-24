// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherPage.h"

#include "../dialogs/AddTeacherDialog.h"
#include "../managements/AccountManager.h"
#include "delegate/TeacherOperationDelegate.h"

TeacherPage::TeacherPage(QWidget *parent) : QWidget(parent) {
    tableModel = new TeacherTableModel(this);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    headerLayout = new QHBoxLayout();

    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("教师管理", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0f172a;");

    subtitleLabel = new QLabel("加载中", this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索教师姓名、院系...");
    searchEdit->setFixedWidth(280);
    searchEdit->setObjectName("SearchEdit");

    btnAddTeacher = new QPushButton("+ 新增教师", this);
    btnAddTeacher->setCursor(Qt::PointingHandCursor);
    btnAddTeacher->setObjectName("AddElement");

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(searchEdit);
    headerLayout->addWidget(btnAddTeacher);

    mainLayout->addLayout(headerLayout);

    tableView = new QTableView(this);
    tableView->setModel(tableModel);
    tableView->setShowGrid(true);
    tableView->setGridStyle(Qt::SolidLine);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setAlternatingRowColors(true);
    tableView->setFocusPolicy(Qt::NoFocus);
    tableView->verticalHeader()->setVisible(false);

    auto *header = tableView->horizontalHeader();
    header->setSectionsMovable(false);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->setSectionResizeMode(4, QHeaderView::Fixed);

    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setColumnWidth(0, 120);
    tableView->setColumnWidth(3, 100);
    tableView->setColumnWidth(4, 120);
    tableView->setItemDelegateForColumn(4, new TeacherOperationDelegate(this));

    mainLayout->addWidget(tableView);

    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }

    connect(searchEdit, &QLineEdit::textChanged, [] {
        // TODO: 实时搜索
    });

    connect(btnAddTeacher, &QPushButton::clicked, [this] {
        if (AddTeacherDialog dialog(this); dialog.exec() == QDialog::Accepted) {
            reloadData();
        }
    });

    reloadData();
}

void TeacherPage::reloadData() const {
    tableModel->setTeachers(aaims::manager::account::get_teachers().values());
    subtitleLabel->setText(QString("管理系统内共 %1 名教师").arg(tableModel->rowCount()));
}
