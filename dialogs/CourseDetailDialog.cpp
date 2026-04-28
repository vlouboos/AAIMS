// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "CourseDetailDialog.h"

#include <QLineEdit>

#include "../utils/DataStructures.h"

CourseDetailDialog::CourseDetailDialog(aaims::model::Course *course, QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("课程详情");
    resize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    mainLayout = new QVBoxLayout(this);
    headerLabel = new QLabel("编辑课程信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QFormLayout();
    tableLayout->setSpacing(15);

    editId = new QLineEdit(this);
    editId->setText(course->id);

    editName = new QLineEdit(this);
    editName->setText(course->name);

    editGrade = new QLineEdit(this);
    editGrade->setText(cls->grade);
    editGrade->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"), this));

    majorLayout = new QHBoxLayout();

    completerMajor = new QCompleter(this);
    completerMajor->setCaseSensitivity(Qt::CaseInsensitive);
    completerMajor->setFilterMode(Qt::MatchContains);
    completerMajor->setCompletionMode(QCompleter::PopupCompletion);

    comboMajor = new QComboBox(this);
    comboMajor->addItems(aaims::manager::classes::get_departments());
    comboMajor->setEditable(true);
    comboMajor->setPlaceholderText("请选择学院");
    if (aaims::manager::classes::get_departments().contains(cls->department)) {
        comboMajor->setCurrentText(cls->department);
    }
    comboMajor->setInsertPolicy(QComboBox::NoInsert);
    comboMajor->setCompleter(completerMajor);

    btnAddMajor = new QPushButton("+", this);
    btnAddMajor->setStyleSheet("padding: 0; margin: 0;");
    btnAddMajor->setObjectName("AddElement");
    btnAddMajor->setFixedSize(24, 24);

    majorLayout->addWidget(comboMajor);
    majorLayout->addWidget(btnAddMajor);

    masterLayout = new QHBoxLayout();

    completerMaster = new QCompleter();
    completerMaster->setCaseSensitivity(Qt::CaseInsensitive);
    completerMaster->setFilterMode(Qt::MatchContains);
    completerMaster->setCompletionMode(QCompleter::PopupCompletion);
    completerMaster->setCompletionColumn(0);

    comboMaster = new QComboBox(this);
    const auto &teachers = aaims::manager::account::get_teachers();

    for (auto it = teachers.begin(); it != teachers.end(); ++it) {
        QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
        comboMaster->addItem(display, (*it)->uuid);
    }
    comboMaster->setEditable(true);
    comboMaster->setPlaceholderText("例如: 张三");
    if (teachers.contains(cls->master)) {
        const TeacherAccount *t = teachers[cls->master];
        comboMaster->setCurrentText(QString("%1(%2)").arg(t->name, t->department));
    }
    comboMaster->setInsertPolicy(QComboBox::NoInsert);
    comboMaster->setCompleter(completerMaster);

    btnAddTeacher = new QPushButton("+", this);
    btnAddTeacher->setStyleSheet("padding: 0; margin: 0;");
    btnAddTeacher->setObjectName("AddElement");
    btnAddTeacher->setFixedSize(24, 24);

    masterLayout->addWidget(comboMaster);
    masterLayout->addWidget(btnAddTeacher);

    coursesLayout = new QVBoxLayout();

    coursesLabel = new QLabel(QString("共%1个课程").arg(cls->courses.size()), this);
    coursesLabel->setMaximumHeight(40);

    coursesEditLayout = new QHBoxLayout();
    coursesEditLayout->setAlignment(Qt::AlignTop);

    courses = new QScrollArea();

    courseList = new QListWidget();

    const auto &allCourses = aaims::manager::course::get_courses();
    for (const auto &courseUuid: cls->courses) {
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

    tableLayout->addRow("班级名称:", editName);
    tableLayout->addRow("年级:", editGrade);
    tableLayout->addRow("院系:", majorLayout);
    tableLayout->addRow("班主任:", masterLayout);
    tableLayout->addRow("班级课程:", coursesLayout);

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
}
