// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "GradeEditDialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFile>

#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"

GradeEditDialog::GradeEditDialog(const QUuid &courseUuid, QWidget *parent)
    : StyledDialog(parent), courseUuid(courseUuid) {
    setWindowTitle("编辑成绩");
    resize(800, 600);

    setupUI();
    loadStudents();

    applyStyles();
}

void GradeEditDialog::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    headerLabel = new QLabel("编辑课程成绩", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    // Course info
    auto *infoWidget = new QWidget(this);
    infoLayout = new QFormLayout(infoWidget);
    infoWidget->setStyleSheet("background-color: #f8fafc; border-radius: 8px; padding: 10px;");

    courseIdLabel = new QLabel(this);
    courseNameLabel = new QLabel(this);

    infoLayout->addRow("课程编号:", courseIdLabel);
    infoLayout->addRow("课程名称:", courseNameLabel);

    // Load course info
    if (auto course = aaims::manager::course::get_courses()[courseUuid]) {
        courseIdLabel->setText(course->id);
        courseNameLabel->setText(course->name);
    }

    // Tab widget for single edit page
    tabWidget = new QTabWidget(this);

    // Single Edit Page
    singleEditPage = new QWidget();
    singleLayout = new QVBoxLayout(singleEditPage);

    // Grades table
    gradesTable = new QTableWidget(this);
    gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    gradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Disable direct editing, use buttons instead

    // Setup table columns
    gradesTable->setColumnCount(6);
    gradesTable->setHorizontalHeaderLabels({"学号", "姓名", "平时分", "期末分", "总分", "操作"});

    auto *header = gradesTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    singleLayout->addWidget(gradesTable);
    tabWidget->addTab(singleEditPage, "单个编辑");

    // Add tabs to main layout
    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(infoWidget);
    mainLayout->addWidget(tabWidget);

    // Buttons
    btnLayout = new QHBoxLayout();

    btnSave = new QPushButton("保存修改", this);
    btnSave->setObjectName("AddElement");
    connect(btnSave, &QPushButton::clicked, this, &GradeEditDialog::onSaveButtonClicked);

    btnCancel = new QPushButton("取消", this);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addStretch();
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);

    mainLayout->addLayout(btnLayout);
}

void GradeEditDialog::loadStudents() {
    // Get the course
    auto course = aaims::manager::course::get_courses()[courseUuid];
    if (!course) return;

    // Collect all students in this course (direct enrollment + from classes)
    studentUuids.clear();

    // Add directly enrolled students
    for (const auto &studentUuid: course->students) {
        studentUuids.append(studentUuid);
    }

    // Add students from classes enrolled in this course
    const auto &allClasses = aaims::manager::classes::get_classes();
    for (const auto &classUuid: course->classes) {
        if (allClasses.contains(classUuid)) {
            const auto &cls = allClasses[classUuid];
            for (const auto &studentUuid: cls->students) {
                if (!studentUuids.contains(studentUuid)) {
                    studentUuids.append(studentUuid);
                }
            }
        }
    }

    // Set up table
    gradesTable->setRowCount(studentUuids.size());

    const auto &allStudents = aaims::manager::account::get_working_students();
    const auto &allRatings = aaims::manager::rating::get_ratings();

    for (int i = 0; i < studentUuids.size(); ++i) {
        const auto &studentUuid = studentUuids[i];
        const auto &student = allStudents[studentUuid];

        if (student) {
            // Student ID
            auto *idItem = new QTableWidgetItem(student->username);
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
            gradesTable->setItem(i, 0, idItem);

            // Student Name
            auto *nameItem = new QTableWidgetItem(student->name);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            gradesTable->setItem(i, 1, nameItem);

            // Get or create rating for this student and course
            double performance = 0.0;
            double score = 0.0;
            double finalScore = 0.0;

            if (allRatings.contains(studentUuid)) {
                const auto &studentRating = allRatings[studentUuid];
                if (studentRating->ratings.contains(courseUuid)) {
                    const auto &ratingDetail = studentRating->ratings[courseUuid];
                    performance = ratingDetail.performance;
                    score = ratingDetail.score;
                    finalScore = ratingDetail.finalScore;
                }
            }

            // Performance grade
            auto *perfItem = new QTableWidgetItem(QString::number(performance, 'f', 1));
            perfItem->setFlags(perfItem->flags() & ~Qt::ItemIsEditable);
            gradesTable->setItem(i, 2, perfItem);

            // Exam score
            auto *examItem = new QTableWidgetItem(QString::number(score, 'f', 1));
            examItem->setFlags(examItem->flags() & ~Qt::ItemIsEditable);
            gradesTable->setItem(i, 3, examItem);

            // Final grade
            auto *finalItem = new QTableWidgetItem(QString::number(finalScore, 'f', 1));
            finalItem->setFlags(finalItem->flags() & ~Qt::ItemIsEditable);
            gradesTable->setItem(i, 4, finalItem);

            // Action button
            auto *editButton = new QPushButton("编辑");
            editButton->setObjectName("AddElement");
            connect(editButton, &QPushButton::clicked, [this, i]() {
                onEditStudentGrades(i);
            });
            gradesTable->setCellWidget(i, 5, editButton);
        }
    }
}

void GradeEditDialog::onEditStudentGrades(int row) {
    if (row < 0 || row >= studentUuids.size()) return;

    QUuid studentUuid = studentUuids[row];
    const auto &allStudents = aaims::manager::account::get_working_students();
    const auto &student = allStudents[studentUuid];

    if (!student) return;

    // Get current grades
    double currentPerformance = 0.0;
    double currentScore = 0.0;

    const auto &allRatings = aaims::manager::rating::get_ratings();
    if (allRatings.contains(studentUuid)) {
        const auto &studentRating = allRatings[studentUuid];
        if (studentRating->ratings.contains(courseUuid)) {
            const auto &ratingDetail = studentRating->ratings[courseUuid];
            currentPerformance = ratingDetail.performance;
            currentScore = ratingDetail.score;
        }
    }

    // Create a dialog to edit grades for this student
    QDialog editDialog(this);
    editDialog.setWindowTitle(QString("编辑 %1 的成绩").arg(student->name));
    editDialog.resize(300, 200);

    auto *layout = new QVBoxLayout(&editDialog);

    auto *formLayout = new QFormLayout();

    auto *perfEdit = new QLineEdit(&editDialog);
    perfEdit->setText(QString::number(currentPerformance, 'f', 1));
    formLayout->addRow("平时分:", perfEdit);

    auto *scoreEdit = new QLineEdit(&editDialog);
    scoreEdit->setText(QString::number(currentScore, 'f', 1));
    formLayout->addRow("期末分:", scoreEdit);

    layout->addLayout(formLayout);

    auto *btnLayout = new QHBoxLayout();
    auto *okBtn = new QPushButton("确定", &editDialog);
    auto *cancelBtn = new QPushButton("取消", &editDialog);

    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, [&editDialog, perfEdit, scoreEdit, this, studentUuid]() {
        bool ok1, ok2;
        double perf = perfEdit->text().toDouble(&ok1);
        double score = scoreEdit->text().toDouble(&ok2);

        if (!ok1 || !ok2) {
            QMessageBox::warning(&editDialog, "输入错误", "请输入有效的数字！");
            return;
        }

        // Calculate final score (40% performance + 60% exam)
        double finalScore = perf * 0.4 + score * 0.6;

        // Ensure rating entry exists
        if (!aaims::manager::rating::get_ratings().contains(studentUuid)) {
            auto newRating = std::make_shared<StudentRating>();
            newRating->student_id = studentUuid;

            aaims::manager::rating::get_ratings()[studentUuid] = newRating;
        }

        auto &studentRating = aaims::manager::rating::get_ratings()[studentUuid];

        // Update or create rating detail for this course
        auto &ratingDetail = studentRating->ratings[courseUuid];
        ratingDetail.course_uuid = courseUuid;
        ratingDetail.performance = perf;
        ratingDetail.score = score;
        ratingDetail.finalScore = finalScore;

        // Update the table display
        for (int i = 0; i < gradesTable->rowCount(); ++i) {
            if (studentUuids[i] == studentUuid) {
                gradesTable->item(i, 2)->setText(QString::number(perf, 'f', 1));
                gradesTable->item(i, 3)->setText(QString::number(score, 'f', 1));
                gradesTable->item(i, 4)->setText(QString::number(finalScore, 'f', 1));
                break;
            }
        }

        editDialog.accept();
    });

    connect(cancelBtn, &QPushButton::clicked, &editDialog, &QDialog::reject);

    editDialog.exec();
}

void GradeEditDialog::onSaveButtonClicked() {
    // Save ratings using the RatingManager
    if (aaims::manager::rating::save()) {
        QMessageBox::information(this, "保存成功", "成绩已保存！");
        accept();
    } else {
        QMessageBox::critical(this, "保存失败", "保存成绩时出现错误！");
    }
}
