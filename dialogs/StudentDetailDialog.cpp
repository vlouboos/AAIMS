// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "StudentDetailDialog.h"

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QValidator>
#include <qtconcurrentrun.h>
#include <ranges>

#include "AddClassDialog.h"
#include "ClassAddCourseDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"

StudentDetailDialog::StudentDetailDialog(StudentAccount *account, QWidget *parent) : StyledDialog(parent),
    account(account) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setFixedSize(500, 600);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignTop);

    headerLabel = new QLabel("编辑学生信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QFormLayout();
    tableLayout->setSpacing(15);

    editId = new QLineEdit(this);
    editId->setStyleSheet("color: #a0a0a9;");
    editId->setEnabled(false);
    editId->setText(account->username);

    editName = new QLineEdit(this);
    editName->setText(account->name);

    classLayout = new QHBoxLayout();

    completer = new QCompleter(aaims::manager::classes::get_departments());
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    comboClass = new QComboBox(this);
    for (const auto &x: aaims::manager::classes::get_all_ptr()) {
        comboClass->addItem(x->grade + x->name, x->uuid);
    }
    comboClass->setEditable(true);
    comboClass->setPlaceholderText("请选择班级");
    if (aaims::manager::classes::get_classes().contains(account->currentClass)) {
        const auto cls = aaims::manager::classes::get_classes()[account->currentClass];
        comboClass->setCurrentText(cls->grade + cls->name);
    }
    comboClass->setInsertPolicy(QComboBox::NoInsert);
    comboClass->setCompleter(completer);

    btnAdd = new QPushButton("+", this);
    btnAdd->setStyleSheet("padding: 0; margin: 0;");
    btnAdd->setObjectName("AddElement");
    btnAdd->setFixedSize(24, 24);

    classLayout->addWidget(comboClass);
    classLayout->addWidget(btnAdd);

    comboStatus = new QComboBox(this);
    comboStatus->setPlaceholderText("请选择状态");
    comboStatus->addItems({"在校", "毕业", "休学"});
    if (account->is_graduated()) comboStatus->setCurrentIndex(1);
    else if (account->is_suspended()) comboStatus->setCurrentIndex(2);
    else comboStatus->setCurrentIndex(0);

    editRoom = new QLineEdit(this);
    editRoom->setText(account->dormitory);

    editPhoneNumber = new QLineEdit(this);
    editPhoneNumber->setText(account->phoneNumber);
    editPhoneNumber->setValidator(new QRegularExpressionValidator(QRegularExpression("^1[3-9]\\d{9}$"), this));

    tableLayout->addRow("用户名:", editId);
    tableLayout->addRow("姓名:", editName);
    tableLayout->addRow("班级:", classLayout);
    tableLayout->addRow("状态:", comboStatus);
    tableLayout->addRow("宿舍:", editRoom);
    tableLayout->addRow("手机号码:", editPhoneNumber);

    // Initialize workingCourses from student's lessons
    for (const auto &[courseUuid, retake]: account->lessons) {
        workingCourses.append(courseUuid);
    }

    // Courses section
    coursesLayout = new QVBoxLayout();

    coursesLabel = new QLabel(QString("共%1个课程").arg(workingCourses.size()), this);
    coursesLabel->setMaximumHeight(40);

    coursesEditLayout = new QHBoxLayout();
    coursesEditLayout->setAlignment(Qt::AlignTop);

    courses = new QScrollArea();

    courseList = new QListWidget();

    const auto &allCourses = aaims::manager::course::get_courses();
    for (const auto &[courseUuid, retake]: account->lessons) {
        if (allCourses.contains(courseUuid)) {
            const auto &course = allCourses[courseUuid];
            const auto item = new QListWidgetItem(QString("%1-%2").arg(course->id, course->name)); // NOLINT
            item->setData(Qt::UserRole, courseUuid);
            courseList->addItem(item);
        }
    }

    courses->setWidget(courseList);
    courses->setWidgetResizable(true);

    courseBtnLayout = new QVBoxLayout();

    btnAddCourse = new QPushButton("+", this);
    btnAddCourse->setStyleSheet("padding: 0; margin: 0;");
    btnAddCourse->setObjectName("AddElement");
    btnAddCourse->setFixedSize(24, 24);

    btnRemoveCourse = new QPushButton("-", this);
    btnRemoveCourse->setStyleSheet("padding: 0; margin: 0;");
    btnRemoveCourse->setObjectName("AddElement");
    btnRemoveCourse->setFixedSize(24, 24);
    btnRemoveCourse->setEnabled(false);

    courseBtnLayout->addWidget(btnAddCourse);
    courseBtnLayout->addWidget(btnRemoveCourse);

    coursesEditLayout->addWidget(courses);
    coursesEditLayout->addLayout(courseBtnLayout);

    coursesLayout->addWidget(coursesLabel);
    coursesLayout->addLayout(coursesEditLayout);

    tableLayout->addRow("选修课程:", coursesLayout);

    btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    btnSave = new QPushButton("保存修改", this);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnSave->setObjectName("AddElement");

    btnCancel = new QPushButton("取消", this);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setObjectName("AddElement");

    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);

    mainLayout->addWidget(headerLabel);
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(btnLayout);
    applyStyles();

    connect(btnAdd, &QPushButton::clicked, [this] {
        if (AddClassDialog dialog(this); dialog.exec() == Accepted) {
            comboClass->clear();
            for (const auto &x: aaims::manager::classes::get_all_ptr()) {
                comboClass->addItem(x->grade + x->name, x->uuid);
            }
        }
    });

    connect(btnAddCourse, &QPushButton::clicked, [this, allCourses, account] {
        // Build list of courses NOT eligible for this student
        QList<QUuid> ineligibleCourses;
        for (const auto &[courseUuid, course] : allCourses.asKeyValueRange()) {
            // Check if student is eligible for this course based on assignment rule
            if (!aaims::manager::account::student::is_course_eligible(account, course.get())) {
                ineligibleCourses.append(courseUuid);
            }
        }

        // Pass workingCourses + ineligible courses to hide them from selection
        const QList<QUuid> restrictedCourses = this->workingCourses + ineligibleCourses;

        if (ClassAddCourseDialog dialog(restrictedCourses, this); dialog.exec() == Accepted) {
            for (const auto added = dialog.getAddedCourses(); const auto &uuid: added) {
                this->workingCourses.append(uuid);
                const auto &course = allCourses[uuid];
                auto *item = new QListWidgetItem(QString("%1-%2").arg(course->id, course->name)); // NOLINT
                item->setData(Qt::UserRole, uuid);
                courseList->addItem(item);
            }
            coursesLabel->setText(QString("共%1个课程").arg(this->workingCourses.size()));
        }
    });

    connect(btnRemoveCourse, &QPushButton::clicked, [this] {
        const auto selectedItems = courseList->selectedItems();
        if (selectedItems.isEmpty()) {
            return;
        }
        if (QMessageBox::warning(this, "确定", "确定要删除选中的课程吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            for (const auto item: std::ranges::reverse_view(selectedItems)) {
                const auto uuid = item->data(Qt::UserRole).value<QUuid>();
                this->workingCourses.removeAll(uuid);
                delete courseList->takeItem(courseList->row(item));
            }
            btnRemoveCourse->setEnabled(false);
            coursesLabel->setText(QString("共%1个课程").arg(this->workingCourses.size()));
        }
    });

    connect(courseList, &QListWidget::itemSelectionChanged, [this] {
        btnRemoveCourse->setEnabled(!courseList->selectedItems().isEmpty());
    });

    connect(courseList, &QListWidget::itemDoubleClicked, [this](const QListWidgetItem *item) {
        const auto uuid = item->data(Qt::UserRole).value<QUuid>();
        const auto &all_courses = aaims::manager::course::get_courses();
        const auto &course = all_courses[uuid];
        const auto &teacher_accounts = aaims::manager::account::get_teachers();
        const auto &teacher = teacher_accounts[course->teacher];
        const QString info = QString("课程编号: %1\n课程名称: %2\n教师: %3\n学分: %4")
                .arg(course->id, course->name, teacher->name, QString::number(course->credit));
        QMessageBox::information(this, "课程详情", info);
    });

    connect(btnSave, &QPushButton::clicked, this, &StudentDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void StudentDetailDialog::onSaveButtonClicked() {
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名不能为空！");
        return;
    }
    const auto &clsUuid = comboClass->currentData().value<QUuid>();
    auto *cls = aaims::manager::classes::get_classes()[clsUuid].get();
    if (!cls) {
        QMessageBox::warning(this, "输入错误", "请选择班级！");
        return;
    }
    static const QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    if (editPhoneNumber->text().trimmed().length() != 11 || !phoneRegex.match(
            editPhoneNumber->text().trimmed()).isValid()) {
        QMessageBox::warning(this, "输入错误", "无效的中国大陆手机号！");
        return;
    }
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    account->name = editName->text().trimmed();
    aaims::manager::classes::get_classes()[account->currentClass]->students.removeOne(account->uuid);
    cls->students.append(account->uuid);
    account->currentClass = clsUuid;
    account->phoneNumber = editPhoneNumber->text().trimmed();

    // Update course enrollment
    const auto &allCoursesRef = aaims::manager::course::get_courses();
    const auto previousCourseIds = [this]() {
        QList<QUuid> ids;
        for (const auto &[uuid, retake]: account->lessons) {
            ids.append(uuid);
        }
        return ids;
    }();

    // Remove student from courses that are no longer selected
    for (const auto &courseUuid: previousCourseIds) {
        if (!workingCourses.contains(courseUuid) && allCoursesRef.contains(courseUuid)) {
            allCoursesRef[courseUuid]->students.removeAll(account->uuid);
        }
    }

    // Add student to courses that are newly selected
    for (const auto &courseUuid: workingCourses) {
        if (!previousCourseIds.contains(courseUuid) && allCoursesRef.contains(courseUuid)) {
            if (!allCoursesRef[courseUuid]->students.contains(account->uuid)) {
                allCoursesRef[courseUuid]->students.append(account->uuid);
            }
        }
    }

    // Update student's lesson list
    account->lessons.clear();
    for (const auto &courseUuid: workingCourses) {
        account->lessons.append(CourseStatus{courseUuid, 0});
    }

    switch (comboStatus->currentIndex()) {
        case 0: {
            if (account->is_suspended()) aaims::manager::account::get_suspended_students().remove(account->uuid);
            else if (account->is_graduated()) aaims::manager::account::get_graduated_students().remove(account->uuid);
            else break;
            aaims::manager::account::get_working_students()[account->uuid] = account;
            account->status &= ~Account::SUSPENDED & ~Account::GRADUATED;
            break;
        }
        case 1: {
            if (account->is_suspended()) aaims::manager::account::get_suspended_students().remove(account->uuid);
            else if (!account->is_graduated()) aaims::manager::account::get_working_students().remove(account->uuid);
            else break;
            aaims::manager::account::get_graduated_students()[account->uuid] = account;
            account->status &= ~Account::SUSPENDED;
            account->status |= Account::GRADUATED;
            break;
        }
        case 2: {
            if (account->is_graduated()) aaims::manager::account::get_graduated_students().remove(account->uuid);
            else if (!account->is_suspended()) aaims::manager::account::get_working_students().remove(account->uuid);
            else break;
            aaims::manager::account::get_suspended_students()[account->uuid] = account;
            account->status &= ~Account::GRADUATED;
            account->status |= Account::SUSPENDED;
            break;
        }
        default: break;
    }
    const auto future = QtConcurrent::run([] {
        return aaims::manager::classes::saveClasses() && aaims::manager::course::save() &&
               aaims::manager::account::save();
    });
    auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "保存完成", QString("保存学生信息成功！"));
        accept();
    });
    watcher->setFuture(future);
}

