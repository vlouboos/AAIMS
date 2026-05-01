// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "GradeEditDialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>

#include "SingleGradeEditDialog.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"
#include "../pages/delegate/GradeDelegate.h"
#include "../pages/model/FilterProxyModel.h"
#include "../utils/AsyncJsonIO.h"

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

    batchImportPage = new QWidget();

    batchLayout = new QVBoxLayout(batchImportPage);
    batchLayout->setContentsMargins(30, 30, 30, 30);
    batchLayout->setSpacing(20);

    tipLabel = new QLabel("支持导入 .csv 格式的文件。\n请确保列头包含: 学号,平时分,期末分,总成绩。\n例: 202525220433,100,99.5,100", batchImportPage);
    tipLabel->setStyleSheet("color: #64748b; line-height: 1.5;");

    btnSelectFile = new QPushButton("选择 CSV 文件", batchImportPage);
    btnSelectFile->setStyleSheet("padding: 8px; border: 1px dashed #cbd5e1; border-radius: 6px; background: #f8fafc;");

    fileStatusLabel = new QLabel("未选择文件", batchImportPage);
    fileStatusLabel->setAlignment(Qt::AlignCenter);
    fileStatusLabel->setStyleSheet("font-size: 12px; color: #94a3b8;");

    btnConfirmBatch = new QPushButton("开始批量导入", batchImportPage);
    btnConfirmBatch->setEnabled(false);
    btnConfirmBatch->setStyleSheet(
        "QPushButton { background-color: #f1f5f9; color: #94a3b8; padding: 10px; border-radius: 6px; font-weight: bold; border: none; }"
        "QPushButton:enabled { background-color: #10b981; color: white; }"
        "QPushButton:enabled:hover { background-color: #059669; }"
    );

    batchLayout->addWidget(tipLabel);
    batchLayout->addWidget(btnSelectFile);
    batchLayout->addWidget(fileStatusLabel);
    batchLayout->addStretch();
    batchLayout->addWidget(btnConfirmBatch);

    tabWidget->addTab(singleEditPage, "单个编辑");
    tabWidget->addTab(batchImportPage, "批量导入");

    // Add tabs to main layout
    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(infoWidget);
    mainLayout->addWidget(tabWidget);

    applyStyles();

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

    connect(btnSelectFile, &QPushButton::clicked, [this] {
        selectedFilePath = QFileDialog::getOpenFileName(
            this, "选择成绩单", "", "CSV 文件 (*.csv);;所有文件 (*.*)"
        ).trimmed();
        if (!selectedFilePath.isEmpty()) {
            fileStatusLabel->setText("已就绪: " + selectedFilePath.split('/').last());
            btnConfirmBatch->setEnabled(true);
        }
    });

    connect(btnConfirmBatch, &QPushButton::clicked, this, &GradeEditDialog::importFromCsv);
}

QPair<unsigned long long, unsigned long long> GradeEditDialog::importFromCsv() const {
    unsigned long long succeed = 0, failed = 0;
    aaims::io::loadCsv(selectedFilePath, [this, &succeed, &failed](const auto &lines) {
        for (const QString &line: lines) {
            if (line.trimmed().isEmpty()) {
                continue;
            }

            QStringList fields = line.split(",");
            if (fields.size() < 4) {
                failed++;
                continue;
            }

            const QString username = fields[0].trimmed();
            const QString performance = fields[1].trimmed();
            const QString score = fields[3].trimmed();
            const QString finalScore = fields[4].trimmed();
            if (username.isEmpty() || performance.isEmpty() || finalScore.isEmpty()) {
                failed++;
                continue;
            }
            StudentRating::RatingDetail detail;
            detail.performance = performance.toDouble();
            detail.score = score.toDouble();
            detail.finalScore = finalScore.toDouble();
            auto student_rating = aaims::manager::rating::get_ratings()[aaims::manager::account::findByUsername(username)->uuid];
            if (!student_rating.get()) {
                aaims::manager::rating::get_ratings()[aaims::manager::account::findByUsername(username)->uuid] = std::make_shared<StudentRating>();
            }
            student_rating->ratings[courseUuid] = detail;
            succeed++;
        }
    });
    aaims::manager::classes::saveDepartments();
    aaims::manager::account::save(); // This is synchronized!!!

    return {succeed, failed};
}
