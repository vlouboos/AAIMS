// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddCourseDialog.h"

#include <QCompleter>
#include <QFormLayout>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QProgressDialog>
#include <qtconcurrentrun.h>

#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"

AddCourseDialog::AddCourseDialog(QWidget *parent) : StyledDialog(parent) {
    mainLayout = new QVBoxLayout(this);

    formLayout = new QFormLayout();

    editId = new QLineEdit(this);
    editName = new QLineEdit(this);

    teacherLayout = new QHBoxLayout();

    comboTeacher = new QComboBox(this);
    const auto &teachers = aaims::manager::account::get_teachers();
    for (auto it = teachers.begin(); it != teachers.end(); ++it) {
        QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
        comboTeacher->addItem(display, (*it)->uuid);
    }
    comboTeacher->setEditable(true);
    comboTeacher->setPlaceholderText("例如: 张三");
    comboTeacher->setInsertPolicy(QComboBox::NoInsert);

    teacherCompleter = new QCompleter(comboTeacher->model());
    teacherCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    teacherCompleter->setFilterMode(Qt::MatchContains);
    teacherCompleter->setCompletionMode(QCompleter::InlineCompletion);

    comboTeacher->setCompleter(teacherCompleter);

    btnAddTeacher = new QPushButton("+", this);
    btnAddTeacher->setStyleSheet("padding: 0; margin: 0;");
    btnAddTeacher->setObjectName("AddElement");
    btnAddTeacher->setFixedSize(24, 24);

    teacherLayout->addWidget(comboTeacher);
    teacherLayout->addWidget(btnAddTeacher);

    comboCredits = new QComboBox(this);
    for (int i = 1; i <= 4; i++) comboCredits->addItem(QString::number(i) + " 学分", i);

    formLayout->addRow("课程编号:", editId);
    formLayout->addRow("课程名称:", editName);
    formLayout->addRow("授课教师:", teacherLayout);
    formLayout->addRow("学分:", comboCredits);

    timeGroup = new QGroupBox("上课时间", this);
    groupLayout = new QVBoxLayout(timeGroup);

    btnAddSlot = new QPushButton("+ 添加时段", this);
    connect(btnAddSlot, &QPushButton::clicked, this, &AddCourseDialog::onAddSlotClicked);
    groupLayout->addWidget(btnAddSlot, 0, Qt::AlignRight);

    timeSlotsLayout = new QVBoxLayout();
    groupLayout->addLayout(timeSlotsLayout);

    btnLayout = new QHBoxLayout();
    btnCancel = new QPushButton("取消", this);
    btnSave = new QPushButton("添加", this);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(timeGroup);
    mainLayout->addLayout(btnLayout);

    setWindowTitle("添加课程");
    resize(500, 400);

    const auto w = new TimeSlot(Course::LessonTime(), this);
    // Strange, IDK why it keeps stupid clangD.
    connect(w, &TimeSlot::removeRequested, this, [this, w] { removeSlot(w); });
    connect(btnSave, &QPushButton::clicked, this, [this] {
        if (validateForm()) {
            auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
            pd->setWindowModality(Qt::WindowModal);
            pd->show();
            const auto course = std::make_shared<Course>();

            TeacherAccount *teacher = aaims::manager::account::get_teachers()[comboTeacher->currentData().value<
                QUuid>()];
            if (teacher->is_class_master()) {
                QMessageBox::warning(this, "输入错误", "该老师已经是另一班级的班主任！");
                return;
            }
            course->id = editId->text().trimmed();
            course->name = editName->text().trimmed();
            course->teacher = teacher->uuid;
            course->credit = comboCredits->currentData().value<int>();
            course->status = Course::ACCEPTING;
            for (const auto &x: slotWidgets) {
                course->times.append(x->toData());
            }
            aaims::manager::course::add(course);
            teacher->teachingClasses.append(course->uuid);
            const auto future = QtConcurrent::run([] { return aaims::manager::course::save(); });
            const auto watcher = new QFutureWatcher<bool>(this); // NOLINT
            connect(watcher, &QFutureWatcher<bool>::finished, this, [this, pd, watcher] {
                pd->close();
                pd->deleteLater();
                watcher->deleteLater();
                QMessageBox::information(this, "添加完成", QString("添加课程成功！"));
                accept();
            });
            watcher->setFuture(future);
        }
    });
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    timeSlotsLayout->addWidget(w);
    slotWidgets.append(w);
    applyStyles();
}

void AddCourseDialog::onAddSlotClicked() {
    const auto w = new TimeSlot(Course::LessonTime(), this);
    connect(w, &TimeSlot::removeRequested, this, [this, w] { removeSlot(w); });
    timeSlotsLayout->addWidget(w);
    slotWidgets.append(w);
}

void AddCourseDialog::removeSlot(TimeSlot *slot) {
    if (slotWidgets.size() <= 1) {
        QMessageBox::warning(this, "提示", "至少需要保留一个上课时间段！");
        return;
    }

    slotWidgets.removeOne(slot);
    timeSlotsLayout->removeWidget(slot);
    slot->deleteLater();
}

bool AddCourseDialog::validateForm() {
    if (editId->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "课程编号不能为空");
        return false;
    }
    if (editName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "课程名称不能为空");
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
            if (occupied[data.dayOfWeek - 1][data.startTime + i] & mask) {
                QMessageBox::warning(this, "时间冲突", "时间安排存在冲突！");
                return false;
            }
            occupied[data.dayOfWeek - 1][data.startTime + i] |= mask;
        }
        return true;
    });
}
