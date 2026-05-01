// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "SelectCourseDialog.h"

#include <QMessageBox>
#include <QHeaderView>

#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"

SelectCourseDialog::SelectCourseDialog(const QList<QUuid> &availableCourses, QWidget *parent)
    : StyledDialog(parent), availableCourses(availableCourses) {
    setWindowTitle("选课");
    resize(500, 600);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    headerLabel = new QLabel("选择课程", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    // Scroll area for courses list
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    coursesWidget = new QWidget(scrollArea);
    coursesLayout = new QVBoxLayout(coursesWidget);
    coursesLayout->setSpacing(5);

    scrollArea->setWidget(coursesWidget);

    // Buttons
    btnLayout = new QHBoxLayout();

    confirmBtn = new QPushButton("确认选课", this);
    confirmBtn->setObjectName("AddElement");
    connect(confirmBtn, &QPushButton::clicked, this, &SelectCourseDialog::onConfirmClicked);

    cancelBtn = new QPushButton("取消", this);
    cancelBtn->setObjectName("AddElement");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addStretch();
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(scrollArea);
    mainLayout->addLayout(btnLayout);
    populateCourses();

    applyStyles();
}

void SelectCourseDialog::populateCourses() {
    QLayoutItem *child;
    while ((child = coursesLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = child->widget()) {
            widget->deleteLater();
        }
        delete child;
    }

    // Get the current student
    auto *student = dynamic_cast<StudentAccount *>(aaims::manager::account::logged);
    if (!student) {
        QMessageBox::warning(this, "错误", "无法获取当前登录的学生信息");
        return;
    }

    const auto &allCourses = aaims::manager::course::get_courses();

    for (const auto &courseUuid: availableCourses) {
        if (allCourses.contains(courseUuid)) {
            const auto &course = allCourses[courseUuid];

            // Create horizontal layout for each course item
            auto *courseLayout = new QHBoxLayout; // NOLINT

            // Create checkbox for course selection
            auto *checkBox = new QCheckBox();
            // Check if student is already enrolled in this course
            bool isEnrolled = course->students.contains(student->uuid);
            checkBox->setChecked(isEnrolled);

            // Create label with course info
            auto *courseLabel = new QLabel(QString("%1-%2").arg(course->id, course->name)); // NOLINT
            courseLabel->setStyleSheet("padding: 8px;");

            courseLayout->addWidget(checkBox);
            courseLayout->addWidget(courseLabel);
            courseLayout->addStretch();

            // Store checkbox reference for later access
            courseCheckBoxes[courseUuid] = checkBox;

            // Create container widget
            auto *container = new QWidget; // NOLINT
            container->setLayout(courseLayout);
            container->setStyleSheet(
                "background-color: white; border: 1px solid #e2e8f0; border-radius: 4px; margin: 2px;");

            coursesLayout->addWidget(container);
        }
    }

    // Add stretch to fill remaining space
    coursesLayout->addStretch();
}

void SelectCourseDialog::onConfirmClicked() {
    const auto *student = dynamic_cast<StudentAccount *>(aaims::manager::account::logged);
    const auto &allCourses = aaims::manager::course::get_courses();

    // Process all course selections
    for (auto it = courseCheckBoxes.constBegin(); it != courseCheckBoxes.constEnd(); ++it) {
        const QUuid &courseUuid = it.key();
        const QCheckBox *checkBox = it.value();

        if (allCourses.contains(courseUuid)) {
            auto &course = allCourses[courseUuid];

            if (checkBox->isChecked()) {
                // Add student to course if not already enrolled
                if (!course->students.contains(student->uuid)) {
                    course->students.append(student->uuid);
                }
            } else {
                // Remove student from course if enrolled
                course->students.removeAll(student->uuid);
            }
        }
    }

    aaims::manager::course::save();

    QMessageBox::information(this, "成功", "选课信息已保存！");
    accept();
}
