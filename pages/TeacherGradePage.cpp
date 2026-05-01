// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherGradePage.h"

#include <QFile>
#include <QHeaderView>

#include "delegate/GradeDelegate.h"
#include "model/FilterProxyModel.h"
#include "../dialogs/GradeEditDialog.h"

TeacherGradePage::TeacherGradePage(QWidget *parent) : QWidget(parent) {
    tableModel = new GradeTableModel(this);
    proxyModel = new FilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    headerLayout = new QHBoxLayout();

    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("成绩管理", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0f172a;");

    subtitleLabel = new QLabel("加载中", this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索课程编号、课程名...");
    searchEdit->setFixedWidth(280);
    searchEdit->setObjectName("SearchEdit");

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(searchEdit);

    mainLayout->addLayout(headerLayout);

    tableView = new QTableView(this);
    tableView->setModel(proxyModel);
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
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->setSectionResizeMode(4, QHeaderView::Fixed);

    auto *delegate = new GradeDelegate(this);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setColumnWidth(0, 140);
    tableView->setColumnWidth(1, 100);
    tableView->setColumnWidth(2, 200);
    tableView->setColumnWidth(3, 100);
    tableView->setColumnWidth(4, 120);
    tableView->setItemDelegateForColumn(4, delegate);

    mainLayout->addWidget(tableView);

    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }

    connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
        proxyModel->setFilterFixedString(text);
    });

    connect(delegate, &GradeDelegate::edit, [this](const QModelIndex &index) {
        if (const Course *course = aaims::manager::course::get_courses()[tableModel->getCourse(
            proxyModel->mapToSource(index))].get()) {
            GradeEditDialog(course->uuid, this).exec();
        }
    });

    connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex &index) {
        if (const Course *course = aaims::manager::course::get_courses()[tableModel->getCourse(
            proxyModel->mapToSource(index))].get()) {
            GradeEditDialog(course->uuid, this).exec();
        }
    });

    tableModel->setCourses(aaims::manager::account::get_teachers()[aaims::manager::account::logged->uuid]->courses);
    proxyModel->sort(0);
    subtitleLabel->setText(QString("管理系统内共 %1 个您的课程").arg(tableModel->rowCount(QModelIndex())));
}