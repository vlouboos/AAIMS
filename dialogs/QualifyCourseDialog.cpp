// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "QualifyCourseDialog.h"

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>
#include <QSplitter>

#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"
#include "../managements/ClassManager.h"

QualifyCourseDialog::QualifyCourseDialog(QList<QUuid> qualifyingCourses, QWidget *parent) : StyledDialog(parent),
    qualifyingCourses(qualifyingCourses) {
    qDebug() << "QualifyingCourses:" << qualifyingCourses.count();
    setWindowTitle("筛选课程");
    resize(900, 600);

    mainLayout = new QHBoxLayout(this);
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

    infoLayout = new QHBoxLayout();
    capacityLabel = new QLabel("-", rightPanel);
    currentLabel = new QLabel("-", rightPanel);
    exceededLabel = new QLabel("-", rightPanel);

    capLabel = new QLabel("容量:");
    curLabel = new QLabel("当前:");
    excLabel = new QLabel("超出:");

    infoLayout->addWidget(capLabel);
    infoLayout->addWidget(capacityLabel);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(curLabel);
    infoLayout->addWidget(currentLabel);
    infoLayout->addSpacing(20);
    infoLayout->addWidget(excLabel);
    infoLayout->addWidget(exceededLabel);
    infoLayout->addStretch();

    detailsLayout->addWidget(selectedCourseLabel);
    detailsLayout->addLayout(infoLayout);

    rightPanelLayout->addWidget(detailsWidget);

    // Students list section
    studentsTitle = new QLabel("选课学生", rightPanel);
    studentsTitle->setStyleSheet("font-size: 14px; font-weight: bold; margin-top: 10px;");
    rightPanelLayout->addWidget(studentsTitle);

    studentsArea = new QScrollArea(rightPanel);
    studentsArea->setWidgetResizable(true);
    studentsArea->setStyleSheet("border: none;");
    studentsWidget = new QWidget();
    studentsLayout = new QVBoxLayout(studentsWidget);
    studentsLayout->setAlignment(Qt::AlignTop);
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
    connect(btnFinalize, &QPushButton::clicked, [this] {
        // Move course status from QUALIFYING to STARTED
        if (currentCourseUuid != EMPTY_UUID) {
            // TODO: Update course status to STARTED
        }
    });

    btnCancel = new QPushButton("关闭", this);
    btnCancel->setObjectName("AddElement");
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    btnRemoveSelected = new QPushButton("移除选中", this);
    btnRemoveSelected->setObjectName("RemoveElement");
    connect(btnRemoveSelected, &QPushButton::clicked, this, &QualifyCourseDialog::onRemoveSelectedClicked);

    buttonLayout->addWidget(btnRemoveSelected);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnFinalize);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addLayout(buttonLayout);
    populateCourses();

    if (!qualifyingCourses.isEmpty()) {
        currentCourseUuid = qualifyingCourses.first();
        courseList->setCurrentRow(0);
        updateCourseDetails();
        loadStudentsForCurrentCourse();
    }

    applyStyles();
}

void QualifyCourseDialog::populateCourses() {
    courseList->clear();
    for (const auto &uuid : qualifyingCourses) {
        const auto x = aaims::manager::course::get_courses()[uuid];
        auto *item = new QListWidgetItem(QString("%1-%2").arg(x->id, x->name)); // NOLINT
        item->setData(Qt::UserRole, QVariant::fromValue(uuid));
        courseList->addItem(item);
    }
}

void QualifyCourseDialog::updateCourseDetails() const {
    if (currentCourseUuid == EMPTY_UUID) return;
    const auto currentCourse = aaims::manager::course::get_courses()[currentCourseUuid];

    // Update selected course label
    selectedCourseLabel->setText(QString("课程: %1-%2").arg(currentCourse->id, currentCourse->name));

    // Calculate current enrollment (students directly enrolled + students from classes)
    unsigned long long currentEnrollment = currentCourse->students.size();
    for (const auto &classUuid: currentCourse->classes) {
        const auto &allClasses = aaims::manager::classes::get_classes();
        if (allClasses.contains(classUuid)) {
            const auto &cls = allClasses[classUuid];
            currentEnrollment += cls->students.size();
        }
    }

    // Determine capacity - in a real implementation, this would come from a course property
    // For now, we'll use a heuristic based on course characteristics
    int capacity = 50; // Default capacity

    // In a real system, you might calculate capacity based on classroom size, online vs in-person, etc.
    // For now, we'll use a simple approach
    if (currentCourse->online) {
        capacity = 100; // Online courses might have higher capacity
    } else {
        capacity = 50; // Traditional in-person courses
    }

    capacityLabel->setText(QString::number(capacity));
    currentLabel->setText(QString::number(currentEnrollment));

    int exceeded = static_cast<int>(currentEnrollment) - capacity;
    if (exceeded > 0) {
        exceededLabel->setText(QString::number(exceeded));
        exceededLabel->setStyleSheet("color: red;");
    } else {
        exceededLabel->setText(QString::number(0));
        exceededLabel->setStyleSheet("");
    }
}

void QualifyCourseDialog::loadStudentsForCurrentCourse() {
    // Clear existing widgets from the layout and reset checkbox hash
    QLayoutItem *child;
    while ((child = studentsLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = child->widget()) {
            widget->deleteLater();
        }
        delete child;
    }
    studentCheckBoxes.clear();

    if (currentCourseUuid == EMPTY_UUID) return;
    const auto currentCourse = aaims::manager::course::get_courses()[currentCourseUuid];

    // Get all students directly enrolled in this course
    const auto &allStudents = aaims::manager::account::get_working_students();
    for (const auto &studentUuid: currentCourse->students) {
        if (allStudents.contains(studentUuid)) {
            const auto &student = allStudents[studentUuid];
            auto *studentLayout = new QHBoxLayout; // NOLINT
            auto *checkBox = new QCheckBox(); // NOLINT
            auto *studentInfo = new QLabel(QString("%1 (%2)").arg(student->username, student->name)); // NOLINT
            studentInfo->setStyleSheet("padding: 5px;");
            
            // Store the checkbox for later access
            studentCheckBoxes[studentUuid] = checkBox;

            studentLayout->addWidget(checkBox);
            studentLayout->addWidget(studentInfo);
            studentLayout->addStretch();

            auto *container = new QWidget; // NOLINT
            container->setLayout(studentLayout);
            container->setStyleSheet(
                "background-color: white; border: 1px solid #e2e8f0; border-radius: 4px; margin: 2px;");
            studentsLayout->addWidget(container);
        }
    }

    // Add stretch to fill remaining space
    studentsLayout->addStretch();
}

void QualifyCourseDialog::onCourseSelected(QListWidgetItem *current, QListWidgetItem *previous) {
    if (current) {
        // Retrieve course pointer from user data
        const auto uuid = current->data(Qt::UserRole).value<QUuid>();
        currentCourseUuid = uuid;

        updateCourseDetails();
        loadStudentsForCurrentCourse();
    }
}

void QualifyCourseDialog::onRemoveSelectedClicked() {
    if (currentCourseUuid == EMPTY_UUID) return;
    
    const auto currentCourse = aaims::manager::course::get_courses()[currentCourseUuid];
    QList<QUuid> studentsToRemove;
    
    // Collect all checked students
    for (auto it = studentCheckBoxes.constBegin(); it != studentCheckBoxes.constEnd(); ++it) {
        if (it.value()->isChecked()) {
            studentsToRemove.append(it.key());
        }
    }
    
    if (studentsToRemove.isEmpty()) {
        QMessageBox::information(this, "提示", "没有选中任何学生。");
        return;
    }
    
    auto result = QMessageBox::question(this, "确认移除", 
                                       QString("确定要移除选中的 %1 名学生吗？").arg(studentsToRemove.size()),
                                       QMessageBox::Yes | QMessageBox::No);
    
    if (result == QMessageBox::Yes) {
        // Remove all selected students
        for (const auto &studentUuid : studentsToRemove) {
            currentCourse->students.removeAll(studentUuid);
        }
        
        // Update the UI
        updateCourseDetails();
        loadStudentsForCurrentCourse();
        
        // Save the changes
        aaims::manager::course::save();
    }
}