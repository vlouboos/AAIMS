// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "ClassDetailDialog.h"

#include <QCompleter>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QLineEdit>
#include <qtconcurrentrun.h>
#include <QListWidget>
#include <ranges>

#include "AddDepartmentDialog.h"
#include "AddMajorDialog.h"
#include "AddTeacherDialog.h"
#include "ClassAddCourseDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"
#include "../utils/AsyncJsonIO.h"

ClassDetailDialog::ClassDetailDialog(Class *cls,
                                     QWidget *parent) : StyledDialog(parent), cls(cls), workingCourses(cls->courses) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setFixedSize(450, 500);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    headerLabel = new QLabel("编辑班级信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QFormLayout();
    tableLayout->setSpacing(15);

    editName = new QLineEdit(this);
    editName->setText(cls->name);

    editGrade = new QLineEdit(this);
    editGrade->setText(cls->grade);
    editGrade->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"), this));

    majorLayout = new QHBoxLayout();

    completerMajor = new QCompleter(this);
    completerMajor->setCaseSensitivity(Qt::CaseInsensitive);
    completerMajor->setFilterMode(Qt::MatchContains);
    completerMajor->setCompletionMode(QCompleter::PopupCompletion);

    comboMajor = new QComboBox(this);
    for (const auto &major: aaims::manager::classes::get_majors()) {
        if (!major.get()) continue;
        comboMajor->addItem(major->name, major->uuid);
    }
    comboMajor->setEditable(true);
    comboMajor->setPlaceholderText("请选择专业");
    comboMajor->setCurrentIndex(comboMajor->findData(cls->major));
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
    tableLayout->addRow("专业:", majorLayout);
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

    btnExportGrades = new QPushButton("导出成绩表", this);
    btnExportGrades->setCursor(Qt::PointingHandCursor);
    btnExportGrades->setObjectName("AddElement");

    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnExportGrades);

    mainLayout->addWidget(headerLabel);
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(btnLayout);
    applyStyles();

    connect(btnAddMajor, &QPushButton::clicked, [this] {
        if (AddMajorDialog dialog; dialog.exec() == Accepted) {
            comboMajor->clear();
            for (const auto &major: aaims::manager::classes::get_majors()) {
                if (!major.get()) continue;
                comboMajor->addItem(major->name, major->uuid);
            }
            completerMajor->setModel(comboMajor->model());
        }
    });
    connect(btnAddTeacher, &QPushButton::clicked, [this] {
        if (AddTeacherDialog dialog(this); dialog.exec() == Accepted) {
            const auto &t = aaims::manager::account::get_teachers();
            comboMaster->clear();
            for (auto it = t.begin(); it != t.end(); ++it) {
                QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
                comboMaster->addItem(display, (*it)->uuid);
            }
        }
    });
    connect(btnAddCourse, &QPushButton::clicked, [this] {
        if (ClassAddCourseDialog dialog(this->workingCourses, this); dialog.exec() == Accepted) {
            const auto added = dialog.getAddedCourses();
            const auto &all_courses = aaims::manager::course::get_courses();
            for (const auto &uuid: added) {
                this->workingCourses.append(uuid);
                const auto &course = all_courses[uuid];
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
    connect(courseList, &QListWidget::itemDoubleClicked, [this, allCourses](const QListWidgetItem *item) {
        const auto uuid = item->data(Qt::UserRole).value<QUuid>();
        const auto &course = allCourses[uuid];
        const auto &teacher_accounts = aaims::manager::account::get_teachers();
        const auto &teacher = teacher_accounts[course->teacher];
        const QString info = QString("课程编号: %1\n课程名称: %2\n教师: %3\n学分: %4")
                .arg(course->id, course->name, teacher->name, QString::number(course->credit));
        QMessageBox::information(this, "课程详情", info);
    });
    connect(btnSave, &QPushButton::clicked, this, &ClassDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnExportGrades, &QPushButton::clicked, this, [this, cls] {
        const QString path = QFileDialog::getExistingDirectory(this, "选择目标文件夹", QCoreApplication::applicationDirPath(),
                                                               QFileDialog::ShowDirsOnly);
        if (path.isEmpty()) {
            return;
        }
        if (const QFileInfo info(path); !info.isWritable()) {
            QMessageBox::warning(this, "错误", "选定的目录没有写入权限！");
        } else {
            auto *pd = new QProgressDialog("正在导出...", nullptr, 0, 0, this); // NOLINT
            pd->setWindowModality(Qt::WindowModal);
            pd->show();
            const auto future = QtConcurrent::run([cls, path] {
                for (const auto &courseUuid: cls->courses) {
                    const auto &course = aaims::manager::course::get_courses()[courseUuid];
                    constexpr auto header = "学号,姓名,平时分,期末成绩,总成绩";
                    QStringList lines;
                    for (const auto &studentUuid: cls->students) {
                        const auto &student = aaims::manager::account::get_students()[studentUuid];
                        const auto &rating = aaims::manager::rating::get_ratings()[studentUuid];
                        double performance = 0.0, score = 0.0, finalScore = 0.0;
                        if (rating.get()) {
                            performance = rating->ratings[course->uuid].performance;
                            score = rating->ratings[course->uuid].score;
                            finalScore = rating->ratings[course->uuid].finalScore;
                        }
                        const QString line = QString("%1,%2,%3,%4,%5")
                                .arg(student->username, student->name, QString::number(performance),
                                     QString::number(score), QString::number(finalScore));
                        lines.append(line);
                    }
                    aaims::io::saveCsv(
                        QString("%1/%2%3-%4-%5-%6成绩.csv").arg(path, cls->grade, cls->name, course->semester, course->id, course->name),
                        lines, header);
                }
                QStringList lines;
                for (const auto &studentUuid: cls->students) {
                    for (const auto &student = aaims::manager::account::get_students()[studentUuid]; const auto &[uuid,
                             retake]: student->lessons) {
                        const auto &course = aaims::manager::course::get_courses()[uuid];
                        const auto &rating = aaims::manager::rating::get_ratings()[studentUuid];
                        double performance = 0.0, score = 0.0, finalScore = 0.0;
                        if (rating.get()) {
                            performance = rating->ratings[course->uuid].performance;
                            score = rating->ratings[course->uuid].score;
                            finalScore = rating->ratings[course->uuid].finalScore;
                        }
                        const QString line = QString("%1,%2,%3,%4,%5,%6,%7").arg(
                            student->username, student->name, course->name, retake == 0 ? "自选" : QString("重修%1次").arg(retake),
                            QString::number(performance), QString::number(score),
                            QString::number(finalScore));
                        lines.append(line);
                    }
                }
                if (!lines.isEmpty()) {
                    constexpr auto header = "学号,姓名,课程,类型,平时分,期末成绩,总成绩";
                    aaims::io::saveCsv(
                        QString("%1/%2%3-%4-%5个人课程成绩.csv").arg(path, cls->grade, cls->name,
                                                               QDate::currentDate().toString("yyyy-MM-dd"), cls->name),
                        lines, header);
                }
            });
            const auto watcher = new QFutureWatcher<void>(this); // NOLINT
            connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher, pd] {
                pd->close();
                pd->deleteLater();
                watcher->deleteLater();
                QMessageBox::information(this, "成功", "成绩导出完成！", QMessageBox::Ok);
            });
            watcher->setFuture(future);
        }
    });
}

void ClassDetailDialog::onSaveButtonClicked() {
    const QString &name = editName->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名不能为空！");
        return;
    }
    const QString &grade = editGrade->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "年级不能为空！");
        return;
    }
    const QUuid &major = comboMajor->currentData().value<QUuid>();
    if (!aaims::manager::classes::get_majors().contains(major)) {
        QMessageBox::warning(this, "输入错误", "请选择专业！");
        return;
    }
    if (!comboMaster->currentData().isValid() || !aaims::manager::account::get_teachers().contains(
            comboMaster->currentData().value<QUuid>())) {
        QMessageBox::warning(this, "输入错误", "请选择班主任！");
        return;
    }
    TeacherAccount *teacher = aaims::manager::account::get_teachers()[comboMaster->currentData().value<QUuid>()];
    const QUuid newUuid = teacher->uuid;
    if (teacher->uuid != cls->master) {
        if (teacher->is_class_master()) {
            QMessageBox::warning(this, "输入错误", "该老师已经是另一班级的班主任！");
            return;
        }
        teacher->status |= Account::CLASS_MASTER;
        teacher->managingClass = cls->uuid;
        teacher = aaims::manager::account::get_teachers()[cls->master];
        teacher->status &= ~Account::CLASS_MASTER;
        teacher->managingClass = EMPTY_UUID;
    }
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();

    cls->name = name;
    cls->grade = grade;
    cls->major = major;
    cls->master = newUuid;

    // Update bidirectional relationship between class and courses
    const auto &allCoursesRef = aaims::manager::course::get_courses();
    const auto previousCourses = cls->courses;

    // Remove class from courses that are no longer assigned
    for (const auto &courseUuid: previousCourses) {
        if (!workingCourses.contains(courseUuid) && allCoursesRef.contains(courseUuid)) {
            allCoursesRef[courseUuid]->classes.removeAll(cls->uuid);
        }
    }

    // Add class to courses that are newly assigned
    for (const auto &courseUuid: workingCourses) {
        if (!previousCourses.contains(courseUuid) && allCoursesRef.contains(courseUuid)) {
            if (!allCoursesRef[courseUuid]->classes.contains(cls->uuid)) {
                allCoursesRef[courseUuid]->classes.append(cls->uuid);
                if (allCoursesRef[courseUuid]->status != Course::ENDED)
                    allCoursesRef[courseUuid]->status = Course::STARTED;
            }
        }
    }

    cls->courses = workingCourses;
    const auto future = QtConcurrent::run([] {
        return aaims::manager::classes::saveClasses() &&
               aaims::manager::course::save() &&
               aaims::manager::account::save();
    });
    auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "保存完成", QString("保存班级成功！"));
        accept();
    });
    watcher->setFuture(future);
}
