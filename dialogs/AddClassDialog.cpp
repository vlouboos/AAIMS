// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddClassDialog.h"

#include <QCompleter>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QtConcurrentRun>

#include "AddDepartmentDialog.h"
#include "AddTeacherDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"

AddClassDialog::AddClassDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("新增教师账号");
    setFixedSize(450, 380);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    tabWidget = new QTabWidget(this);

    singleAddPage = new QWidget();

    singleLayout = new QFormLayout(singleAddPage);
    singleLayout->setContentsMargins(30, 30, 30, 30);
    singleLayout->setSpacing(15);

    gradeEdit = new QLineEdit(singleAddPage);
    gradeEdit->setPlaceholderText("例如: 2025");
    gradeEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"), this));

    nameEdit = new QLineEdit(singleAddPage);
    nameEdit->setPlaceholderText("例如: 软工R4");

    deptLayout = new QHBoxLayout();

    deptCompleter = new QCompleter();
    deptCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    deptCompleter->setFilterMode(Qt::MatchContains);
    deptCompleter->setCompletionMode(QCompleter::PopupCompletion);

    deptCombo = new QComboBox(singleAddPage);
    deptCombo->addItems(aaims::manager::classes::get_departments());
    deptCombo->setEditable(true);
    deptCombo->setPlaceholderText("请选择学院");
    deptCombo->setInsertPolicy(QComboBox::NoInsert);
    deptCombo->setCompleter(deptCompleter);

    btnAddDept = new QPushButton("+", singleAddPage);
    btnAddDept->setStyleSheet("padding: 0; margin: 0;");
    btnAddDept->setObjectName("AddElement");
    btnAddDept->setFixedSize(24, 24);

    deptLayout->addWidget(deptCombo);
    deptLayout->addWidget(btnAddDept);

    masterLayout = new QHBoxLayout();

    masterCompleter = new QCompleter();
    masterCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    masterCompleter->setFilterMode(Qt::MatchContains);
    masterCompleter->setCompletionMode(QCompleter::PopupCompletion);

    masterCombo = new QComboBox(this);
    const auto &teachers = aaims::manager::account::get_teachers();

    for (auto it = teachers.begin(); it != teachers.end(); ++it) {
        QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
        masterCombo->addItem(display, (*it)->uuid);
    }
    masterCombo->setEditable(true);
    masterCombo->setPlaceholderText("例如: 张三");
    masterCombo->setInsertPolicy(QComboBox::NoInsert);
    masterCombo->setCompleter(masterCompleter);

    btnAddTeacher = new QPushButton("+", singleAddPage);
    btnAddTeacher->setStyleSheet("padding: 0; margin: 0;");
    btnAddTeacher->setObjectName("AddElement");
    btnAddTeacher->setFixedSize(24, 24);

    masterLayout->addWidget(masterCombo);
    masterLayout->addWidget(btnAddTeacher);

    btnConfirmSingle = new QPushButton("确认添加", singleAddPage);
    btnConfirmSingle->setCursor(Qt::PointingHandCursor);
    btnConfirmSingle->setObjectName("AddElement");

    singleLayout->addRow("年级:", gradeEdit);
    singleLayout->addRow("班级名字:", nameEdit);
    singleLayout->addRow("院系:", deptLayout);
    singleLayout->addRow("班主任:", masterLayout);
    singleLayout->addRow("", btnConfirmSingle);

    batchAddPage = new QWidget();

    batchLayout = new QVBoxLayout(batchAddPage);
    batchLayout->setContentsMargins(30, 30, 30, 30);
    batchLayout->setSpacing(20);

    tipLabel = new QLabel("支持导入 .csv 格式的文件。\n请确保列头包含: 工号, 姓名, 性别, 院系, 手机号", batchAddPage);
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

    connect(btnAddDept, &QPushButton::clicked, [this] {
        if (AddDepartmentDialog dialog; dialog.exec() == Accepted) {
            deptCombo->clear();
            deptCombo->addItems(aaims::manager::classes::get_departments());
        }
    });
    connect(btnAddTeacher, &QPushButton::clicked, [this] {
        if (AddTeacherDialog dialog; dialog.exec() == Accepted) {
            const auto &t = aaims::manager::account::get_teachers();
            masterCombo->clear();
            for (auto it = t.begin(); it != t.end(); ++it) {
                QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
                masterCombo->addItem(display, (*it)->uuid);
            }
        }
    });

    connect(btnSelectFile, &QPushButton::clicked, [this] {
        selectedFilePath = QFileDialog::getOpenFileName(
            this, "选择教师名录", "", "CSV 文件 (*.csv);;所有文件 (*.*)"
        ).trimmed();
        if (!selectedFilePath.isEmpty()) {
            fileStatusLabel->setText("已就绪: " + selectedFilePath.split('/').last());
            btnConfirmBatch->setEnabled(true);
        }
    });

    connect(btnConfirmSingle, &QPushButton::clicked, [this] {
                if (gradeEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "输入错误", "年级不能为空！");
                    return;
                }
                if (nameEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "输入错误", "班级名字不能为空！");
                    return;
                }
                if (deptCombo->currentIndex() == -1) {
                    QMessageBox::warning(this, "输入错误", "请选择院系！");
                    return;
                }
                if (!masterCombo->currentData().isValid() || !aaims::manager::account::get_teachers().contains(
                        masterCombo->currentData().value<QUuid>())) {
                    QMessageBox::warning(this, "输入错误", "请选择班主任！");
                    return;
                }
                auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();

                const auto cls = std::make_shared<aaims::model::Classes>();
                cls->grade = gradeEdit->text().trimmed();
                cls->name = nameEdit->text().trimmed();
                cls->department = deptCombo->currentText().trimmed();
                cls->master = masterCombo->currentData().value<QUuid>();
                if (const QString result = aaims::manager::classes::add(cls); !result.isEmpty()) {
                    pd->close();
                    pd->deleteLater();
                    QMessageBox::critical(this, "错误", result);
                    return;
                }
                const auto future = QtConcurrent::run([] { return aaims::manager::classes::saveClasses(); });

                auto *watcher = new QFutureWatcher<void>(this); // NOLINT

                connect(watcher, &QFutureWatcherBase::finished, [this, watcher, pd] {
                    pd->close();
                    pd->deleteLater();
                    watcher->deleteLater();
                    QMessageBox::information(this, "添加完成", QString("添加教师成功！"));
                    accept();
                });
                watcher->setFuture(future);
            }
    );

    connect(btnConfirmBatch, &QPushButton::clicked, [this] {
        if (selectedFilePath.isEmpty()) return;

        auto *pd = new QProgressDialog("正在导入...", nullptr, 0, 0, this); // NOLINT
        pd->setWindowModality(Qt::WindowModal);
        pd->show();

        const auto &future = QtConcurrent::run(&AddClassDialog::importFromCsv, this);
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

QPair<unsigned long long, unsigned long long> AddClassDialog::importFromCsv() const {
    unsigned long long succeed = 0, failed = 0;
    QFile file(selectedFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {0, 0};

    QTextStream in(&file);
    in.readLine(); // Skip first line

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

        const QString grade = fields[0].trimmed();
        const QString name = fields[1].trimmed();
        const QString dept = fields[3].trimmed();
        const QString phone = fields[4].trimmed();
        if (grade.isEmpty() || name.isEmpty()) {
            failed++;
            continue;
        }
        if (auto classes = aaims::manager::classes::get_all_ptr(); std::ranges::any_of(
            classes, [grade, name, dept](const aaims::model::Classes *cls) {
                return cls->grade == grade && cls->name == name && cls->department == dept;
            })) {
            failed++;
            continue;
        }
        if (!aaims::manager::classes::get_departments().contains(dept)) {
            aaims::manager::classes::addDepartment({dept});
        }
        auto cls = std::make_shared<aaims::model::Classes>();
        cls->grade = grade;
        cls->name = name;
        cls->department = dept;

        aaims::manager::classes::add(cls);
        succeed++;
    }
    aaims::manager::classes::saveDepartments();
    aaims::manager::classes::saveClasses(); // This is synchronized!!!

    return {succeed, failed};
}
