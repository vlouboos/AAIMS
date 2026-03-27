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

#include "AddDepartmentDialog.h"
#include "AddTeacherDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"

ClassDetailDialog::ClassDetailDialog(Classes *classes,
                                     QWidget *parent) : StyledDialog(parent), cls(classes) {
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setFixedSize(450, 320);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    headerLabel = new QLabel("编辑班级信息", this);
    headerLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0f172a;");

    tableLayout = new QFormLayout();
    tableLayout->setSpacing(15);

    editName = new QLineEdit(this);
    editName->setText(classes->name);

    editGrade = new QLineEdit(this);
    editGrade->setText(classes->grade);
    editGrade->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"), this));

    deptLayout = new QHBoxLayout();

    completerDept = new QCompleter(this);
    completerDept->setCaseSensitivity(Qt::CaseInsensitive);
    completerDept->setFilterMode(Qt::MatchContains);
    completerDept->setCompletionMode(QCompleter::PopupCompletion);

    comboDept = new QComboBox(this);
    comboDept->addItems(aaims::manager::classes::get_departments());
    comboDept->setEditable(true);
    comboDept->setPlaceholderText("请选择学院");
    if (aaims::manager::classes::get_departments().contains(classes->department)) {
        comboDept->setCurrentText(classes->department);
    }
    comboDept->setInsertPolicy(QComboBox::NoInsert);
    comboDept->setCompleter(completerDept);

    btnAddDept = new QPushButton("+", this);
    btnAddDept->setStyleSheet("padding: 0; margin: 0;");
    btnAddDept->setObjectName("AddElement");
    btnAddDept->setFixedSize(24, 24);

    deptLayout->addWidget(comboDept);
    deptLayout->addWidget(btnAddDept);

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
    if (teachers.contains(classes->master)) {
        const TeacherAccount *t = teachers[classes->master];
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

    tableLayout->addRow("班级名称:", editName);
    tableLayout->addRow("年级:", editGrade);
    tableLayout->addRow("院系:", deptLayout);
    tableLayout->addRow("班主任:", masterLayout);

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

    connect(btnAddDept, &QPushButton::clicked, [this] {
        if (AddDepartmentDialog dialog; dialog.exec() == Accepted) {
            comboDept->clear();
            comboDept->addItems(aaims::manager::classes::get_departments());
        }
    });
    connect(btnAddTeacher, &QPushButton::clicked, [this] {
        if (AddTeacherDialog dialog; dialog.exec() == Accepted) {
            const auto &t = aaims::manager::account::get_teachers();
            comboMaster->clear();
            for (auto it = t.begin(); it != t.end(); ++it) {
                QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
                comboMaster->addItem(display, (*it)->uuid);
            }
        }
    });
    connect(btnSave, &QPushButton::clicked, this, &ClassDetailDialog::onSaveButtonClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
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
    const QString &department = comboDept->currentText().trimmed();
    if (!aaims::manager::classes::get_departments().contains(department)) {
        QMessageBox::warning(this, "输入错误", "请选择院系！");
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
        teacher->managingClass = Account::EMPTY_UUID;
    }
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();

    cls->name = name;
    cls->grade = grade;
    cls->department = department;
    cls->master = newUuid;
    const auto future = QtConcurrent::run([] {
        return aaims::manager::classes::saveClasses() &&
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
