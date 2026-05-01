// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddClassDialog.h"

#include <QCompleter>
#include <QFutureWatcher>
#include <QProgressDialog>

#include "AddDepartmentDialog.h"
#include "AddMajorDialog.h"
#include "AddTeacherDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../utils/AsyncJsonIO.h"

AddClassDialog::AddClassDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("新增班级");
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

    majorLayout = new QHBoxLayout();

    majorCombo = new QComboBox(singleAddPage);
    for (const auto &major: aaims::manager::classes::get_majors()) {
        if (!major.get()) continue;
        majorCombo->addItem(major->name, major->uuid);
    }
    majorCombo->setEditable(true);
    majorCombo->setPlaceholderText("请选择专业");
    majorCombo->setInsertPolicy(QComboBox::NoInsert);

    majorCompleter = new QCompleter(majorCombo->model());
    majorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    majorCompleter->setFilterMode(Qt::MatchContains);
    majorCompleter->setCompletionMode(QCompleter::InlineCompletion);

    majorCombo->setCompleter(majorCompleter);

    btnAddMajor = new QPushButton("+", singleAddPage);
    btnAddMajor->setStyleSheet("padding: 0; margin: 0;");
    btnAddMajor->setObjectName("AddElement");
    btnAddMajor->setFixedSize(24, 24);

    majorLayout->addWidget(majorCombo);
    majorLayout->addWidget(btnAddMajor);

    masterLayout = new QHBoxLayout();

    masterCombo = new QComboBox(this);
    const auto &teachers = aaims::manager::account::get_teachers();

    for (auto it = teachers.begin(); it != teachers.end(); ++it) {
        QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
        masterCombo->addItem(display, (*it)->uuid);
    }
    masterCombo->setEditable(true);
    masterCombo->setPlaceholderText("例如: 张三");
    masterCombo->setInsertPolicy(QComboBox::NoInsert);

    masterCompleter = new QCompleter(masterCombo->model());
    masterCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    masterCompleter->setFilterMode(Qt::MatchContains);
    masterCompleter->setCompletionMode(QCompleter::InlineCompletion);

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
    singleLayout->addRow("专业:", majorLayout);
    singleLayout->addRow("班主任:", masterLayout);
    singleLayout->addRow("", btnConfirmSingle);

    batchAddPage = new QWidget();

    batchLayout = new QVBoxLayout(batchAddPage);
    batchLayout->setContentsMargins(30, 30, 30, 30);
    batchLayout->setSpacing(20);

    tipLabel = new QLabel("支持导入 .csv 格式的文件。\n请确保列头包含: 年级、班级名字、院系、班主任。\n例: 2025,软工R4,软件学院,李华(数学与信息学院)", batchAddPage);
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

    connect(btnAddMajor, &QPushButton::clicked, [this] {
        if (AddMajorDialog dialog; dialog.exec() == Accepted) {
            majorCombo->clear();
            for (const auto &major: aaims::manager::classes::get_majors()) {
                majorCombo->addItem(major->name, major->uuid);
            }
            majorCompleter->setModel(majorCombo->model());
        }
    });
    connect(btnAddTeacher, &QPushButton::clicked, [this] {
        if (AddTeacherDialog dialog(this); dialog.exec() == Accepted) {
            const auto &t = aaims::manager::account::get_teachers();
            masterCombo->clear();
            for (auto it = t.begin(); it != t.end(); ++it) {
                QString display = QString("%1(%2)").arg((*it)->name, (*it)->department);
                masterCombo->addItem(display, (*it)->uuid);
            }
            masterCompleter->setModel(masterCombo->model());
        }
    });

    connect(btnSelectFile, &QPushButton::clicked, [this] {
        selectedFilePath = QFileDialog::getOpenFileName(
            this, "选择班级名录", "", "CSV 文件 (*.csv);;所有文件 (*.*)"
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
                const QUuid &major = majorCombo->currentData().value<QUuid>();
                if (!aaims::manager::classes::get_majors().contains(major)) {
                    QMessageBox::warning(this, "输入错误", "请选择专业！");
                    return;
                }
                const QUuid &teacherUuid = masterCombo->currentData().value<QUuid>();
                if (!masterCombo->currentData().isValid() || !aaims::manager::account::get_teachers().contains(
                        teacherUuid)) {
                    QMessageBox::warning(this, "输入错误", "请选择班主任！");
                    return;
                }
                auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();

                TeacherAccount *teacher = aaims::manager::account::get_teachers()[teacherUuid];
                if (teacher->is_class_master()) {
                    QMessageBox::warning(this, "输入错误", "该老师已经是另一班级的班主任！");
                    return;
                }
                const auto cls = std::make_shared<Class>();
                cls->grade = gradeEdit->text().trimmed();
                cls->name = nameEdit->text().trimmed();
                cls->major = major;
                cls->master = teacher->uuid;
                if (const QString result = aaims::manager::classes::add(cls); !result.isEmpty()) {
                    pd->close();
                    pd->deleteLater();
                    QMessageBox::critical(this, "错误", result);
                    return;
                }
                teacher->status |= Account::CLASS_MASTER;
                teacher->managingClass = cls->uuid;
                const auto future = QtConcurrent::run([] {
                    return aaims::manager::classes::saveClasses() &&
                           aaims::manager::account::save();
                });

                auto *watcher = new QFutureWatcher<void>(this); // NOLINT

                connect(watcher, &QFutureWatcherBase::finished, [this, watcher, pd] {
                    pd->close();
                    pd->deleteLater();
                    watcher->deleteLater();
                    QMessageBox::information(this, "添加完成", QString("添加班级成功！"));
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
    auto teachers = aaims::manager::account::get_teachers();
    auto majors = aaims::manager::classes::get_majors();
    aaims::io::loadCsv(selectedFilePath, [&succeed, &failed, teachers, majors](const auto &lines) {
        for (const QString &line: lines) {
            if (line.trimmed().isEmpty()) {
                continue;
            }

            QStringList fields = line.split(",");
            if (fields.size() < 4) {
                failed++;
                continue;
            }

            const QString grade = fields[0].trimmed();
            const QString name = fields[1].trimmed();
            const QString major = fields[2].trimmed();
            const QString master = fields[3].trimmed();
            if (grade.isEmpty() || name.isEmpty() || major.isEmpty() || master.isEmpty()) {
                failed++;
                continue;
            }
            auto ma = std::ranges::find_if(majors,
                                           [major](const auto &m) {
                                               return major == m->name;
                                           });
            if (ma == majors.end()) {
                failed++;
                continue;
            }
            if (auto classes = aaims::manager::classes::get_all_ptr(); std::ranges::any_of(
                classes, [grade, name, ma](const Class *cls) {
                    return cls->grade == grade && cls->name == name && cls->major == (*ma)->uuid;
                })) {
                failed++;
                continue;
            }
            auto it = std::ranges::find_if(teachers,
                                           [master](const auto *t) {
                                               return master == QString("%1(%2)").arg(t->name).arg(t->department);
                                           });
            if (it == teachers.end()) {
                failed++;
                continue;
            }
            if ((*it)->is_class_master()) {
                failed++;
                continue;
            }
            auto cls = std::make_shared<Class>();
            cls->grade = grade;
            cls->name = name;
            cls->major = (*ma)->uuid;
            cls->master = (*it)->uuid;
            if (const QString result = aaims::manager::classes::add(cls); !result.isEmpty()) {
                failed++;
                continue;
            }
            (*it)->status |= Account::CLASS_MASTER;
            (*it)->managingClass = cls->uuid;
            succeed++;
        }
    });
    aaims::manager::classes::saveDepartments();
    aaims::manager::classes::saveClasses(); // This is synchronized!!!
    aaims::manager::account::save();

    return {succeed, failed};
}
