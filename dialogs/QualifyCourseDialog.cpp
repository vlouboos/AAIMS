// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "QualifyCourseDialog.h"

#include <QApplication>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>
#include <QProgressDialog>
#include <QSplitter>
#include <qtconcurrentrun.h>

#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"
#include "../managements/ClassManager.h"

QualifyCourseDialog::QualifyCourseDialog(QList<QUuid> qualifyingCourses, QWidget *parent) : StyledDialog(parent),
    qualifyingCourses(qualifyingCourses) {
    setWindowTitle("筛选课程");
    resize(900, 600);

    // Use a vertical layout for the main dialog
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Create splitter for left-right layout
    splitter = new QSplitter(Qt::Horizontal);

    // Left panel - Course list
    leftPanel = new QWidget();
    leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->setContentsMargins(0, 0, 5, 0);

    leftTitle = new QLabel("待筛选课程", leftPanel);
    leftTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    leftPanelLayout->addWidget(leftTitle);

    courseList = new QListWidget(leftPanel);
    courseList->setMinimumWidth(250);
    courseList->setStyleSheet("QListWidget { border: 1px solid #cbd5e1; border-radius: 4px; }");
    connect(courseList, &QListWidget::currentItemChanged,
            this, &QualifyCourseDialog::onCourseSelected);

    leftPanelLayout->addWidget(courseList);

    // Right panel - Student list
    rightPanel = new QWidget();
    rightPanelLayout = new QVBoxLayout(rightPanel);
    rightPanelLayout->setContentsMargins(5, 0, 0, 0);

    // Course details section
    detailsWidget = new QWidget(rightPanel);
    detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setContentsMargins(10, 10, 10, 10);
    detailsWidget->setStyleSheet("background-color: #f8fafc; border-radius: 8px; margin-bottom: 10px;");

    selectedCourseLabel = new QLabel("请选择一门课程", rightPanel);
    selectedCourseLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    detailsLayout->addWidget(selectedCourseLabel);

    rightPanelLayout->addWidget(detailsWidget);

    // Students list section
    studentsTitle = new QLabel("选课学生", rightPanel);
    studentsTitle->setStyleSheet("font-size: 14px; font-weight: bold; margin-top: 10px;");
    rightPanelLayout->addWidget(studentsTitle);

    studentsArea = new QScrollArea(rightPanel);
    studentsArea->setWidgetResizable(true);
    studentsArea->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 4px;");
    studentsWidget = new QWidget();
    studentsLayout = new QVBoxLayout(studentsWidget);
    studentsLayout->setAlignment(Qt::AlignTop);
    studentsLayout->setContentsMargins(5, 5, 5, 5);
    studentsArea->setWidget(studentsWidget);

    rightPanelLayout->addWidget(studentsArea);

    // Add panels to splitter
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1); // Left panel gets 1/3 of space
    splitter->setStretchFactor(1, 2); // Right panel gets 2/3 of space

    mainLayout->addWidget(splitter);

    // Buttons at bottom
    buttonLayout = new QHBoxLayout();

    btnFinalize = new QPushButton("完成筛选", this);
    btnFinalize->setObjectName("AddElement"); // Using consistent styling
    connect(btnFinalize, &QPushButton::clicked, this, &QualifyCourseDialog::onFinalizeClicked);

    btnCancel = new QPushButton("关闭", this);
    btnCancel->setObjectName("AddElement");
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnFinalize);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addLayout(buttonLayout);
    populateCourses();

    if (!qualifyingCourses.isEmpty()) {
        updateCourseDetails();
        currentCourseUuid = qualifyingCourses.first();
        courseList->setCurrentRow(0);
    }

    applyStyles();
}

void QualifyCourseDialog::populateCourses() {
    courseList->clear();
    for (const auto &uuid: qualifyingCourses) {
        const auto x = aaims::manager::course::get_courses()[uuid];
        auto *item = new QListWidgetItem(QString("%1-%2").arg(x->id, x->name)); // NOLINT
        item->setData(Qt::UserRole, QVariant::fromValue(uuid));
        courseList->addItem(item);
    }
}

void QualifyCourseDialog::updateCourseDetails() const {
    if (currentCourseUuid == EMPTY_UUID) return;
    const auto currentCourse = aaims::manager::course::get_courses()[currentCourseUuid];

    selectedCourseLabel->setText(QString("课程: %1-%2").arg(currentCourse->id, currentCourse->name));
}

void QualifyCourseDialog::loadStudentsForCurrentCourse() {
    QLayoutItem *child;
    while ((child = studentsLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = child->widget()) {
            widget->deleteLater();
        }
        delete child;
    }
    studentCheckBoxButtons.clear();

    if (currentCourseUuid == EMPTY_UUID) return;
    const auto currentCourse = aaims::manager::course::get_courses()[currentCourseUuid];

    const auto &allStudents = aaims::manager::account::get_working_students();
    for (const auto &studentUuid: currentCourse->students) {
        if (allStudents.contains(studentUuid)) {
            const auto &student = allStudents[studentUuid];
            auto *studentLayout = new QHBoxLayout; // NOLINT
            auto *checkBoxBtn = new QPushButton; // NOLINT
            checkBoxBtn->setObjectName("CheckBox");
            checkBoxBtn->setFixedSize(24, 24);
            checkBoxBtn->setStyleSheet(
                "QPushButton#CheckBox {\nborder-radius: 6px;\nborder: 1px solid #e2e8f0;\n}\nQPushButton#CheckBox:checked {\nbackground-color: #2563eb;\ncolor: white;\n}");
            checkBoxBtn->setCheckable(true);

            auto *studentInfo = new QLabel(QString("%1 (%2)").arg(student->name, student->username)); // NOLINT
            studentInfo->setStyleSheet("padding: 5px;");
            studentCheckBoxButtons[studentUuid] = checkBoxBtn;

            studentLayout->addWidget(checkBoxBtn);
            studentLayout->addWidget(studentInfo);

            auto *container = new QWidget; // NOLINT
            container->setLayout(studentLayout);
            container->setStyleSheet(
                "background-color: white; border: 1px solid #e2e8f0; border-radius: 4px; margin: 2px;");
            studentsLayout->addWidget(container);
        }
    }
    studentsLayout->addStretch();
}

void QualifyCourseDialog::onCourseSelected(const QListWidgetItem *current, [[maybe_unused]] QListWidgetItem *previous) {
    if (current) {
        const auto uuid = current->data(Qt::UserRole).value<QUuid>();
        currentCourseUuid = uuid;

        updateCourseDetails();
        loadStudentsForCurrentCourse();
    }
}

void QualifyCourseDialog::onFinalizeClicked() {
    if (currentCourseUuid == EMPTY_UUID) return;

    const auto result = QMessageBox::question(this, "确认",
                                              "确定要完成筛选并开始课程吗？",
                                              QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        auto *pd = new QProgressDialog("正在筛选...", nullptr, 0, 0, this); // NOLINT
        pd->setWindowModality(Qt::WindowModal);
        pd->show();

        const auto future = QtConcurrent::run([this] {
            const auto currentCourse = aaims::manager::course::get_courses()[currentCourseUuid];
            QList<QUuid> studentsToRemove;

            for (auto it = studentCheckBoxButtons.constBegin(); it != studentCheckBoxButtons.constEnd(); ++it) {
                if (!it.value()->isChecked()) {
                    studentsToRemove.append(it.key());
                }
            }

            for (const auto &studentUuid: studentsToRemove) {
                currentCourse->students.removeAll(studentUuid);
            }
            for (const auto &x: currentCourse->students) {
                const auto &student = aaims::manager::account::get_students()[x];
                student->lessons.append({currentCourseUuid, 0});
            }

            currentCourse->status = Course::STARTED;
            return aaims::manager::course::save() && aaims::manager::account::save();
        });
        const auto watcher = new QFutureWatcher<bool>(this); // NOLINT
        connect(watcher, &QFutureWatcher<bool>::finished, [this, pd, watcher] {
            pd->close();
            pd->deleteLater();
            watcher->deleteLater();
            QMessageBox::information(this, "成功", "筛选完成，课程已开始。");
            accept();
        });
        watcher->setFuture(future);
    }
}
