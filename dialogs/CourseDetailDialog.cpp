// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "CourseDetailDialog.h"

#include <QCompleter>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <qtconcurrentrun.h>

#include "AddTeacherDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"
#include "../utils/DataStructures.h"

CourseDetailDialog::CourseDetailDialog(Course *course, QWidget *parent) : StyledDialog(parent), course(course) {
    setWindowTitle("课程详情");
    resize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    mainLayout = new QVBoxLayout(this);
    headerLabel = new QLabel("编辑课程信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    comboSemester = new QComboBox(this);
    QDate currentDate = QDate::currentDate();
    int currentYear = currentDate.year();
    int currentMonth = currentDate.month();
    int targetYear = currentYear;
    int targetTerm = currentMonth >= 9 || currentMonth == 1 ? 1 : 2;
    if (currentMonth > 8) {
        targetYear += 1;
    }
    const QString defaultData = QString("%1-%2").arg(targetYear).arg(targetTerm);
    if (targetTerm == 1) {
        for (int y = currentYear; y <= currentYear + 2; ++y) {
            const QString textAutumn = QString("%1-%2学年 秋季学期").arg(y).arg(y + 1);
            const QString dataAutumn = QString("%1-1").arg(y);
            comboSemester->addItem(textAutumn, dataAutumn);
            const QString textSpring = QString("%1-%2学年 春季学期").arg(y).arg(y + 1);
            const QString dataSpring = QString("%1-2").arg(y);
            comboSemester->addItem(textSpring, dataSpring);
        }
    } else {
        for (int y = currentYear; y <= currentYear + 2; ++y) {
            if (y != currentYear) {
                const QString textAutumn = QString("%1-%2学年 秋季学期").arg(y - 1).arg(y);
                const QString dataAutumn = QString("%1-1").arg(y);
                comboSemester->addItem(textAutumn, dataAutumn);
            }
            const QString textSpring = QString("%1-%2学年 春季学期").arg(y - 1).arg(y);
            const QString dataSpring = QString("%1-2").arg(y);
            comboSemester->addItem(textSpring, dataSpring);
        }
    }

    if (!course->semester.isEmpty()) {
        if (const int found = comboSemester->findData(course->semester); found != -1) {
            comboSemester->setCurrentIndex(found);
        } else {
            comboSemester->insertItem(0, "(未更改)", course->semester);
            comboSemester->setCurrentIndex(0);
        }
    } else if (int defaultIndex = comboSemester->findData(defaultData); defaultIndex != -1) {
        comboSemester->setCurrentIndex(defaultIndex + 1); // Default set to next semester.
    }

    editId = new QLineEdit(this);
    editId->setText(course->id);

    editName = new QLineEdit(this);
    editName->setText(course->name);

    teacherLayout = new QHBoxLayout();

    teacherCompleter = new QCompleter();
    teacherCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    teacherCompleter->setFilterMode(Qt::MatchContains);
    teacherCompleter->setCompletionMode(QCompleter::PopupCompletion);
    teacherCompleter->setCompletionColumn(0);

    comboTeacher = new QComboBox(this);
    const auto &teachers = aaims::manager::account::get_teachers();

    for (auto it = teachers.begin(); it != teachers.end(); ++it) {
        QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
        comboTeacher->addItem(display, (*it)->uuid);
    }
    comboTeacher->setEditable(true);
    comboTeacher->setPlaceholderText("例如: 张三");
    if (teachers.contains(course->teacher)) {
        const TeacherAccount *t = teachers[course->teacher];
        comboTeacher->setCurrentText(QString("%1(%2)").arg(t->name, t->department));
    }
    comboTeacher->setInsertPolicy(QComboBox::NoInsert);
    comboTeacher->setCompleter(teacherCompleter);

    btnAddTeacher = new QPushButton("+", this);
    btnAddTeacher->setStyleSheet("padding: 0; margin: 0;");
    btnAddTeacher->setObjectName("AddElement");
    btnAddTeacher->setFixedSize(24, 24);

    teacherLayout->addWidget(comboTeacher);
    teacherLayout->addWidget(btnAddTeacher);

    comboCredits = new QComboBox(this);
    for (int i = 1; i <= 4; i++) comboCredits->addItem(QString::number(i) + " 学分", i);
    comboCredits->setCurrentIndex(course->credit - 1);

    onlineCheck = new QPushButton(this);
    onlineCheck->setObjectName("CheckBox");
    onlineCheck->setCheckable(true);
    onlineCheck->setFixedSize(24, 24);
    onlineCheck->setChecked(course->online);

    comboStatus = new QComboBox(this);
    comboStatus->addItem("选课中", Course::ACCEPTING);
    comboStatus->addItem("筛选中", Course::QUALIFYING);
    comboStatus->addItem("已开课", Course::STARTED);
    comboStatus->addItem("已结课", Course::ENDED);
    comboStatus->setCurrentIndex(course->status == Course::ACCEPTING
                                     ? 0
                                     : course->status == Course::QUALIFYING
                                           ? 1
                                           : course->status == Course::STARTED
                                                 ? 2
                                                 : 3);


    btnSetAssignmentRule = new QPushButton("设置选课筛选规则", this);
    btnSetAssignmentRule->setObjectName("AddElement");

    formLayout->addRow("学期:", comboSemester);
    formLayout->addRow("课程编号:", editId);
    formLayout->addRow("课程名称:", editName);
    formLayout->addRow("授课教师:", teacherLayout);
    formLayout->addRow("学分:", comboCredits);
    formLayout->addRow("线上:", onlineCheck);
    formLayout->addRow("课程状态:", comboStatus);
    formLayout->addRow("选课规则:", btnSetAssignmentRule);

    btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    timeGroup = new QGroupBox("上课时间", this);

    groupLayout = new QVBoxLayout(timeGroup);

    btnAddSlot = new QPushButton("+ 添加时段", this);

    timeSlotsLayout = new QVBoxLayout();

    groupLayout->addWidget(btnAddSlot, 0, Qt::AlignRight);
    groupLayout->addLayout(timeSlotsLayout);

    btnSave = new QPushButton("保存修改", this);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnSave->setObjectName("AddElement");

    btnCancel = new QPushButton("取消", this);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setObjectName("AddElement");

    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);

    mainLayout->addWidget(headerLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(timeGroup);

    mainLayout->addLayout(btnLayout);

    applyStyles();

    for (const auto &x: course->times) {
        const auto w = new TimeSlot(x, this);
        // Strange, IDK why it keeps stupid clangD.
        connect(w, &TimeSlot::removeRequested, this, [this, w] { removeSlot(w); });
        timeSlotsLayout->addWidget(w);
        slotWidgets.append(w);
    }

    connect(btnSave, &QPushButton::clicked, this, [this, course] {
        if (validateForm()) {
            auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
            pd->setWindowModality(Qt::WindowModal);
            pd->setWindowFlag(Qt::Popup);
            pd->show();
            TeacherAccount *teacher = aaims::manager::account::get_teachers()[comboTeacher->currentData().value<
                QUuid>()];

            QList<Course::LessonTime> times;
            for (const auto &x: slotWidgets) {
                times.append(x->toData());
            }
            if (teacher->is_occupied(comboSemester->currentData().value<QString>(), times, course->times)) {
                pd->close();
                pd->deleteLater();
                QMessageBox::warning(this, "输入错误", "该教师已有课程时间与当前课程冲突！");
                return;
            }
            course->id = editId->text().trimmed();
            course->name = editName->text().trimmed();
            TeacherAccount *old = aaims::manager::account::get_teachers()[course->teacher];
            old->removeCourse(course);
            course->teacher = teacher->uuid;
            course->credit = comboCredits->currentData().value<int>();
            course->semester = comboSemester->currentData().value<QString>();
            course->status = comboStatus->currentData().value<int>();
            course->times.clear();
            course->times.append(times);
            teacher->addCourse(course);
            course->assignmentRule = assignmentRule;

            const auto future = QtConcurrent::run([] {
                return aaims::manager::course::save() && aaims::manager::account::save();
            });
            const auto watcher = new QFutureWatcher<bool>(this); // NOLINT
            connect(watcher, &QFutureWatcher<bool>::finished, this, [this, pd, watcher] {
                pd->close();
                pd->deleteLater();
                watcher->deleteLater();
                QMessageBox::information(this, "修改完成", QString("保存课程成功！"));
                accept();
            });
            watcher->setFuture(future);
        }
    });
    connect(btnCancel, &QPushButton::clicked, this, &CourseDetailDialog::reject);
    connect(btnAddSlot, &QPushButton::clicked, this, &CourseDetailDialog::onAddSlotClicked);
    connect(btnAddTeacher, &QPushButton::clicked, this, [this] {
        if (AddTeacherDialog dialog; dialog.exec() == Accepted) {
            comboTeacher->clear();
            const auto &teacher_accounts = aaims::manager::account::get_teachers();
            for (auto it = teacher_accounts.begin(); it != teacher_accounts.end(); ++it) {
                QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
                comboTeacher->addItem(display, (*it)->uuid);
            }
        }
    });

    connect(onlineCheck, &QPushButton::clicked, this, [this] {
        if (!onlineCheck->isChecked() && slotWidgets.empty()) onAddSlotClicked();
    });

    connect(btnSetAssignmentRule, &QPushButton::clicked, this, [this] {
        if (AssignmentRuleDialog dialog(assignmentRule, this); dialog.exec() == Accepted) {
            assignmentRule = dialog.getRule();
        }
    });
}

void CourseDetailDialog::onAddSlotClicked() {
    const auto w = new TimeSlot(Course::LessonTime(), this);
    // Strange, IDK why it keeps stupid clangD.
    connect(w, &TimeSlot::removeRequested, this, [this, w] { removeSlot(w); });
    timeSlotsLayout->addWidget(w);
    slotWidgets.append(w);
}

void CourseDetailDialog::removeSlot(TimeSlot *slot) {
    if (slotWidgets.size() <= 1 && !onlineCheck->isChecked()) {
        QMessageBox::warning(this, "提示", "线下课至少需要保留一个上课时间段！");
        return;
    }

    slotWidgets.removeOne(slot);
    timeSlotsLayout->removeWidget(slot);
    slot->deleteLater();
}


bool CourseDetailDialog::validateForm() {
    if (editId->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "课程编号不能为空");
        return false;
    }
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "课程名称不能为空");
        return false;
    }
    if (!course->classes.empty() && comboStatus->currentData().value<int>() != Course::STARTED && comboStatus->
        currentData().value<int>() != Course::ENDED) {
        QMessageBox::warning(this, "错误", "已分配班级的课程状态只能为“已开课”或“已结束”！");
        return false;
    }
    // [0-6][0-14]
    int occupied[7][15];
    return std::ranges::all_of(slotWidgets, [this, &occupied](const auto w) {
        const auto data = w->toData();
        if (data.weekEnd < data.weekStart) {
            QMessageBox::warning(this, "时间错误", "结束周不能早于起始周！");
            return false;
        }
        if (data.startTime + data.duration > 15) {
            QMessageBox::warning(this, "时间溢出", "排课时间超出了当天的最大课节范围！");
            return false;
        }
        int mask = 0;
        for (int i = data.weekStart; i <= data.weekEnd; ++i) {
            mask |= 1 << (i - 1);
        }
        for (int i = 0; i < data.duration; i++) {
            if (occupied[data.dayOfWeek][data.startTime + i] & mask) {
                QMessageBox::warning(this, "时间冲突", "时间安排存在冲突！");
                return false;
            }
            occupied[data.dayOfWeek][data.startTime + i] |= mask;
        }
        return true;
    });
}
