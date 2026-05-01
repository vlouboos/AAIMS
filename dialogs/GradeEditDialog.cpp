// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "GradeEditDialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFile>

#include "SingleGradeEditDialog.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"
#include "../pages/delegate/GradeDelegate.h"
#include "../pages/model/FilterProxyModel.h"

GradeEditDialog::GradeEditDialog(const QUuid &courseUuid, QWidget *parent)
    : StyledDialog(parent), courseUuid(courseUuid) {
    setWindowTitle("编辑成绩");
    resize(800, 600);

    tableModel = new SingleGradeTableModel(this);
    proxyModel = new FilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    headerLabel = new QLabel("编辑课程成绩", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    infoWidget = new QWidget(this);
    infoWidget->setStyleSheet("background-color: #f8fafc; border-radius: 8px; padding: 10px;");

    infoLayout = new QFormLayout(infoWidget);

    courseIdLabel = new QLabel(this);
    courseNameLabel = new QLabel(this);

    infoLayout->addRow("课程编号:", courseIdLabel);
    infoLayout->addRow("课程名称:", courseNameLabel);

    // Tab widget for single edit page
    tabWidget = new QTabWidget(this);

    // Single Edit Page
    singleEditPage = new QWidget();
    singleLayout = new QVBoxLayout(singleEditPage);

    // Grades table
    auto *delegate = new GradeDelegate(this);

    tableView = new QTableView(singleEditPage);
    tableView->setModel(proxyModel);
    tableView->setShowGrid(true);
    tableView->setGridStyle(Qt::SolidLine);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setAlternatingRowColors(true);
    tableView->setFocusPolicy(Qt::NoFocus);
    tableView->verticalHeader()->setVisible(false);
    tableView->setItemDelegateForColumn(5, delegate);

    auto *header = tableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::Fixed);

    singleLayout->addWidget(tableView);

    tabWidget->addTab(singleEditPage, "单个编辑");

    // Add tabs to main layout
    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(infoWidget);
    mainLayout->addWidget(tabWidget);

    // Load course info
    if (const auto &course = aaims::manager::course::get_courses()[courseUuid]) {
        QList<QUuid> students = course->students;
        for (const auto &clsUuid: course->classes) {
            const auto &cls = aaims::manager::classes::get_classes()[clsUuid];
            students.append(cls->students);
        }
        courseIdLabel->setText(course->id);
        courseNameLabel->setText(course->name);
        tableModel->setStudents(students, courseUuid);
        proxyModel->sort(0);
    }

    connect(delegate, &GradeDelegate::edit, [this, courseUuid](const QModelIndex &index) {
        StudentRating *rating = aaims::manager::rating::get_ratings()[tableModel->getStudent(
            proxyModel->mapToSource(index))].get();
        if (rating) {
            SingleGradeEditDialog(rating->ratings[courseUuid], this).exec();
        } else {
            const auto r = std::make_shared<StudentRating>();
            aaims::manager::rating::get_ratings()[tableModel->getStudent(proxyModel->mapToSource(index))] = r;
            SingleGradeEditDialog(r->ratings[courseUuid], this).exec();
        }
    });
    connect(tableView, &QTableView::doubleClicked, this, [this, courseUuid](const QModelIndex &index) {
        StudentRating *rating = aaims::manager::rating::get_ratings()[tableModel->getStudent(
            proxyModel->mapToSource(index))].get();
        if (rating) {
            SingleGradeEditDialog(rating->ratings[courseUuid], this).exec();
        } else {
            const auto r = std::make_shared<StudentRating>();
            aaims::manager::rating::get_ratings()[tableModel->getStudent(proxyModel->mapToSource(index))] = r;
            SingleGradeEditDialog(r->ratings[courseUuid], this).exec();
        }
    });

    applyStyles();
}
