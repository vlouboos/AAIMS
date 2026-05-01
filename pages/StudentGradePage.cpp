// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StudentGradePage.h"

#include <QFile>
#include <QHeaderView>

#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"

StudentGradePage::StudentGradePage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadSemesters();

    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }
}

void StudentGradePage::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    headerLayout = new QHBoxLayout();

    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("成绩查询", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0f172a;");

    subtitleLabel = new QLabel("请选择学期后点击查询", this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);

    semesterCombo = new QComboBox(this);
    semesterCombo->setFixedWidth(200);
    semesterCombo->setObjectName("SearchEdit");

    queryButton = new QPushButton("查询", this);
    queryButton->setObjectName("AddElement");
    queryButton->setFixedWidth(100);
    connect(queryButton, &QPushButton::clicked, this, &StudentGradePage::onQueryClicked);

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(semesterCombo);
    headerLayout->addWidget(queryButton);

    mainLayout->addLayout(headerLayout);

    gradesTable = new QTableWidget(this);
    gradesTable->setColumnCount(5);
    gradesTable->setHorizontalHeaderLabels({"课程编号", "课程名称", "平时分", "期末分", "总分"});
    gradesTable->setShowGrid(true);
    gradesTable->setGridStyle(Qt::SolidLine);
    gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    gradesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    gradesTable->setAlternatingRowColors(true);
    gradesTable->setFocusPolicy(Qt::NoFocus);
    gradesTable->verticalHeader()->setVisible(false);
    gradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *header = gradesTable->horizontalHeader();
    header->setSectionsMovable(false);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    mainLayout->addWidget(gradesTable);
}

void StudentGradePage::loadSemesters() {
    semesterCombo->clear();

    const auto &allCourses = aaims::manager::course::get_courses();

    QSet<QString> semesters;

    for (const auto &course: allCourses) {
        if (!course->semester.isEmpty()) {
            semesters.insert(course->semester);
        }
    }

    semesterCombo->addItems(semesters.values());
}

void StudentGradePage::onQueryClicked() {
    QString selectedSemester = semesterCombo->currentText();
    if (selectedSemester.isEmpty()) {
        subtitleLabel->setText("请选择学期");
        return;
    }

    gradesTable->setRowCount(0);

    const auto &student = dynamic_cast<StudentAccount *>(aaims::manager::account::logged);
    const auto &allCourses = aaims::manager::course::get_courses();
    const auto &allRatings = aaims::manager::rating::get_ratings();

    int courseCount = 0;

    QSet<QUuid> courses;
    for (const auto &[uuid, retake]: student->lessons) {
        courses.insert(uuid);
    }
    for (const auto &x: aaims::manager::classes::get_classes()[student->currentClass]->courses) {
        courses.insert(x);
    }
    for (const auto &courseUuid: courses) {
        const auto &course = allCourses[courseUuid];
        if (course->semester != selectedSemester) {
            continue;
        }

        if (allRatings.contains(student->uuid)) {
            const auto &studentRating = allRatings[student->uuid];
            if (studentRating->ratings.contains(courseUuid)) {
                const auto &ratingDetail = studentRating->ratings[courseUuid];
                double performance = ratingDetail.performance;
                double score = ratingDetail.score;
                double finalScore = ratingDetail.finalScore;

                int row = gradesTable->rowCount();
                gradesTable->insertRow(row);

                auto *idItem = new QTableWidgetItem(course->id); // NOLINT
                idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
                gradesTable->setItem(row, 0, idItem);

                auto *nameItem = new QTableWidgetItem(course->name); // NOLINT
                nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
                gradesTable->setItem(row, 1, nameItem);

                auto *perfItem = new QTableWidgetItem(QString::number(performance, 'f', 1)); // NOLINT
                perfItem->setFlags(perfItem->flags() & ~Qt::ItemIsEditable);
                gradesTable->setItem(row, 2, perfItem);

                auto *scoreItem = new QTableWidgetItem(QString::number(score, 'f', 1)); // NOLINT
                scoreItem->setFlags(scoreItem->flags() & ~Qt::ItemIsEditable);
                gradesTable->setItem(row, 3, scoreItem);

                auto *finalItem = new QTableWidgetItem(QString::number(finalScore, 'f', 1)); // NOLINT
                finalItem->setFlags(finalItem->flags() & ~Qt::ItemIsEditable);
                gradesTable->setItem(row, 4, finalItem);

                courseCount++;
            }
        }
    }

    subtitleLabel->setText(QString("共%1门课程").arg(courseCount));
}
