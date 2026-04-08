// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddCourseDialog.h"

#include <QCompleter>
#include <QFile>
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
    setWindowTitle("添加课程");
    resize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    mainLayout = new QVBoxLayout(this);

    tabWidget = new QTabWidget();

    singleAddPage = new QWidget();

    singleLayout = new QVBoxLayout(singleAddPage);

    formLayout = new QFormLayout();

    editId = new QLineEdit(singleAddPage);
    editName = new QLineEdit(singleAddPage);

    teacherLayout = new QHBoxLayout();

    comboTeacher = new QComboBox(singleAddPage);
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

    btnAddTeacher = new QPushButton("+", singleAddPage);
    btnAddTeacher->setStyleSheet("padding: 0; margin: 0;");
    btnAddTeacher->setObjectName("AddElement");
    btnAddTeacher->setFixedSize(24, 24);

    teacherLayout->addWidget(comboTeacher);
    teacherLayout->addWidget(btnAddTeacher);

    comboCredits = new QComboBox(singleAddPage);
    for (int i = 1; i <= 4; i++) comboCredits->addItem(QString::number(i) + " 学分", i);

    formLayout->addRow("课程编号:", editId);
    formLayout->addRow("课程名称:", editName);
    formLayout->addRow("授课教师:", teacherLayout);
    formLayout->addRow("学分:", comboCredits);

    timeGroup = new QGroupBox("上课时间", singleAddPage);

    groupLayout = new QVBoxLayout(timeGroup);

    btnAddSlot = new QPushButton("+ 添加时段", singleAddPage);

    timeSlotsLayout = new QVBoxLayout();

    groupLayout->addWidget(btnAddSlot, 0, Qt::AlignRight);
    groupLayout->addLayout(timeSlotsLayout);

    btnLayout = new QHBoxLayout();
    btnCancel = new QPushButton("取消", singleAddPage);
    btnSave = new QPushButton("添加", singleAddPage);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);

    singleLayout->addLayout(formLayout);
    singleLayout->addWidget(timeGroup);
    singleLayout->addLayout(btnLayout);

    batchAddPage = new QWidget();

    batchLayout = new QVBoxLayout(batchAddPage);
    batchLayout->setContentsMargins(30, 30, 30, 30);
    batchLayout->setSpacing(20);

    tipLabel = new QLabel(
        "支持导入 .csv 格式的文件。\n请确保列头包含: 编号, 名字, 教师, 学分, 上课时间。\n时间格式: 起始周:结束周:周i:第j节:上k节时。\n多个时间用分号“;”间隔，周日为“0”，8:00为“1”。\n例: HM#1,高等数学,李华(数学与信息学院),2,1:16:1:1:2;1:16:3:1:2",
        batchAddPage);
    tipLabel->setStyleSheet("color: #64748b; line-height: 1.5;");

    btnSelectFile = new QPushButton("选择 CSV 文件", batchAddPage);
    btnSelectFile->setStyleSheet("padding: 8px; border: 1px dashed #cbd5e1; border-radius: 6px; background: #f8fafc;");

    fileStatusLabel = new QLabel("未选择文件", batchAddPage);
    fileStatusLabel->setAlignment(Qt::AlignCenter);
    fileStatusLabel->setStyleSheet("font-size: 12px; color: #94a3b8;");

    btnConfirmBatch = new QPushButton("开始批量导入", batchAddPage);
    btnConfirmBatch->setEnabled(false);
    btnConfirmBatch->setStyleSheet(
        "QPushButton { background-color: #f1f5f9; color: #94a3b8; padding: 10px; border-radius: 6px; font-weight: bold; border: none; }"
        "QPushButton:enabled { background-color: #10b981; color: white; }"
        "QPushButton:enabled:hover { background-color: #059669; }"
    );

    batchLayout->addWidget(tipLabel);
    batchLayout->addWidget(btnSelectFile);
    batchLayout->addWidget(fileStatusLabel);
    batchLayout->addStretch();
    batchLayout->addWidget(btnConfirmBatch);

    tabWidget->addTab(singleAddPage, "手动录入");
    tabWidget->addTab(batchAddPage, "批量导入");

    mainLayout->addWidget(tabWidget);

    applyStyles();

    connect(btnAddSlot, &QPushButton::clicked, this, &AddCourseDialog::onAddSlotClicked);
    connect(btnSave, &QPushButton::clicked, this, [this] {
        if (validateForm()) {
            auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
            pd->setWindowModality(Qt::WindowModal);
            pd->show();
            const auto course = std::make_shared<Course>();

            TeacherAccount *teacher = aaims::manager::account::get_teachers()[comboTeacher->currentData().value<
                QUuid>()];

            QList<Course::LessonTime> courses;
            for (const auto &x: slotWidgets) {
                courses.append(x->toData());
            }
            if (teacher->free(courses)) {
                QMessageBox::warning(this, "输入错误", "该教师已有课程时间与当前课程冲突！");
                return;
            }
            course->id = editId->text().trimmed();
            course->name = editName->text().trimmed();
            course->teacher = teacher->uuid;
            course->credit = comboCredits->currentData().value<int>();
            course->status = Course::ACCEPTING;
            course->times.append(courses);
            aaims::manager::course::add(course);
            teacher->courses.append(course->uuid);
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

    connect(btnConfirmBatch, &QPushButton::clicked, [this] {
        if (selectedFilePath.isEmpty()) return;

        auto *pd = new QProgressDialog("正在导入...", nullptr, 0, 0, this); // NOLINT
        pd->setWindowModality(Qt::WindowModal);
        pd->show();

        const auto &future = QtConcurrent::run(&AddCourseDialog::importFromCsv, this);
        auto *watcher = new QFutureWatcher<QPair<unsigned long long, unsigned long long> >(this); // NOLINT
        connect(watcher, &QFutureWatcherBase::finished, [this, watcher, pd] {
            const auto &[succeed, failed] = watcher->result();
            pd->close();
            pd->deleteLater();
            watcher->deleteLater();
            QMessageBox::information(this, "导入完成", QString("后台解析成功！\n实际导入成功: %1\n重复/失败: %2").arg(succeed).arg(failed));
            accept();
        });

        watcher->setFuture(future);
    });
}

QPair<unsigned long long, unsigned long long> AddCourseDialog::importFromCsv() const {
    unsigned long long succeed = 0, failed = 0;
    QFile file(selectedFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {0, 0};

    QTextStream in(&file);
    in.readLine(); // Skip first line
    auto teachers = aaims::manager::account::get_teachers();
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }

        QStringList fields = line.split(",");
        if (fields.size() < 5) {
            failed++;
            continue;
        }

        const QString id = fields[0].trimmed();
        const QString name = fields[1].trimmed();
        const QString teacher = fields[2].trimmed();
        const QString credit = fields[3].trimmed();
        const QString times = fields[4].trimmed();
        if (id.isEmpty() || name.isEmpty() || teacher.isEmpty() || credit.isEmpty() || times.isEmpty()) {
            failed++;
            continue;
        }
        if (auto classes = aaims::manager::course::get_courses().values(); std::ranges::any_of(
            classes, [id](const auto &cls) {
                return cls->id == id;
            })) {
            failed++;
            continue;
        }
        auto it = std::ranges::find_if(teachers,
                                       [teacher](const auto *t) {
                                           return teacher == QString("%1(%2)").arg(t->name).arg(t->department);
                                       });
        if (it == teachers.end()) {
            failed++;
            continue;
        }
        QList<Course::LessonTime> timeList;
        for (QStringList timeStringList = times.split(";"); const auto &time: timeStringList) {
            QStringList timeVal = time.trimmed().split(":");
            if (timeVal.size() < 5) continue;
            const QString startWeek = timeVal[0].trimmed();
            const QString endWeek = timeVal[1].trimmed();
            const QString dayOfWeek = timeVal[2].trimmed();
            const QString startTime = timeVal[3].trimmed();
            const QString duration = timeVal[4].trimmed();
            timeList.append({
                startTime.toInt(), endWeek.toInt(), dayOfWeek.toInt(), startWeek.toInt() - 1, duration.toInt()
            });
        }
        if ((*it)->free(timeList)) {
            failed++;
            continue;
        }
        auto course = std::make_shared<Course>();
        if (const QString result = aaims::manager::course::add(course); !result.isEmpty()) {
            failed++;
            continue;
        }
        (*it)->status |= Account::CLASS_MASTER;
        (*it)->managingClass = course->uuid;
        succeed++;
    }
    aaims::manager::course::save(); // This is synchronized!!!

    return {succeed, failed};
}

void AddCourseDialog::onAddSlotClicked() {
    const auto w = new TimeSlot(Course::LessonTime(), singleAddPage);
    // Strange, IDK why it keeps stupid clangD.
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
