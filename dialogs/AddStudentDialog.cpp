// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddStudentDialog.h"

#include <QCompleter>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QtConcurrentRun>

#include "AddDepartmentDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../utils/DataStructures.h"
#include "../utils/Sha256Util.h"

using namespace aaims::model;

AddStudentDialog::AddStudentDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("新增学生账号");
    setFixedSize(450, 400);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    tabWidget = new QTabWidget(this);

    singleAddPage = new QWidget();

    singleLayout = new QFormLayout(singleAddPage);
    singleLayout->setContentsMargins(30, 30, 30, 30);
    singleLayout->setSpacing(15);

    idEdit = new QLineEdit(singleAddPage);
    idEdit->setPlaceholderText("输入学号");
    idEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"), this));

    nameEdit = new QLineEdit(singleAddPage);
    nameEdit->setPlaceholderText("例如: 张三");

    genderCombo = new QComboBox(singleAddPage);
    genderCombo->addItems({"男", "女"});

    classLayout = new QHBoxLayout();

    completer = new QCompleter();
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    classCombo = new QComboBox(singleAddPage);
    for (const auto &x: aaims::manager::classes::get_all_ptr()) {
        classCombo->addItem(x->grade + x->name, x->uuid);
    }
    classCombo->setEditable(true);
    classCombo->setPlaceholderText("请选择班级");
    classCombo->setInsertPolicy(QComboBox::NoInsert);
    classCombo->setCompleter(completer);

    btnAdd = new QPushButton("+", singleAddPage);
    btnAdd->setStyleSheet("padding: 0; margin: 0;");
    btnAdd->setObjectName("AddElement");
    btnAdd->setFixedSize(24, 24);

    classLayout->addWidget(classCombo);
    classLayout->addWidget(btnAdd);

    roomEdit = new QLineEdit(singleAddPage);
    roomEdit->setPlaceholderText("例如: 火山区0栋110");

    phoneNumberEdit = new QLineEdit(singleAddPage);
    phoneNumberEdit->setPlaceholderText("输入有效的中国大陆手机号");
    phoneNumberEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^1[3-9]\\d{9}$"), this));
    phoneNumberEdit->setMaxLength(11);

    btnConfirmSingle = new QPushButton("确认添加", singleAddPage);
    btnConfirmSingle->setCursor(Qt::PointingHandCursor);
    btnConfirmSingle->setObjectName("AddElement");

    singleLayout->addRow("学号:", idEdit);
    singleLayout->addRow("姓名:", nameEdit);
    singleLayout->addRow("性别:", genderCombo);
    singleLayout->addRow("班级:", classLayout);
    singleLayout->addRow("宿舍(可选):", roomEdit);
    singleLayout->addRow("手机号:", phoneNumberEdit);
    singleLayout->addRow("", btnConfirmSingle);

    batchAddPage = new QWidget();

    batchLayout = new QVBoxLayout(batchAddPage);
    batchLayout->setContentsMargins(30, 30, 30, 30);
    batchLayout->setSpacing(20);

    tipLabel = new QLabel(
        "支持导入 .csv 格式的文件。\n请确保列头包含: 学号, 姓名, 性别, 班级, 宿舍号(占位可选), 手机号\n例: 202525220420,林锦华,男,2025软工R4,,我的11位手机号",
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

    connect(btnAdd, &QPushButton::clicked, [this] {
        if (AddDepartmentDialog dialog; dialog.exec() == Accepted) {
            classCombo->clear();
            classCombo->addItems(aaims::manager::classes::get_departments());
        }
    });

    connect(btnSelectFile, &QPushButton::clicked, [this] {
        selectedFilePath = QFileDialog::getOpenFileName(
            this, "选择学生名录", "", "CSV 文件 (*.csv);;所有文件 (*.*)"
        ).trimmed();
        if (!selectedFilePath.isEmpty()) {
            fileStatusLabel->setText("已就绪: " + selectedFilePath.split('/').last());
            btnConfirmBatch->setEnabled(true);
        }
    });

    connect(btnConfirmSingle, &QPushButton::clicked, [this] {
                if (idEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "输入错误", "工号不能为空！");
                    return;
                }
                if (nameEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "输入错误", "姓名不能为空！");
                    return;
                }
                const auto &clsUuid = classCombo->currentData().value<QUuid>();
                auto *cls = aaims::manager::classes::get_classes()[clsUuid].get();
                if (!cls) {
                    QMessageBox::warning(this, "输入错误", "请选择班级！");
                    return;
                }
                static const QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
                if (phoneNumberEdit->text().trimmed().length() != 11 || !phoneRegex.match(
                        phoneNumberEdit->text().trimmed()).isValid()) {
                    QMessageBox::warning(this, "输入错误", "无效的中国大陆手机号！");
                    return;
                }
                auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();

                const auto student = std::make_shared<StudentAccount>();
                student->username = idEdit->text().trimmed();
                student->name = nameEdit->text().trimmed();
                student->password = Sha256Util::hash("123456");
                student->female = genderCombo->currentIndex() == 1;
                student->status = 0;
                student->currentClass = cls->uuid;
                student->dormitory = roomEdit->text().trimmed();
                student->phoneNumber = phoneNumberEdit->text().trimmed();
                if (const QString result = aaims::manager::account::add(student); !result.isEmpty()) {
                    pd->close();
                    pd->deleteLater();
                    QMessageBox::critical(this, "错误", result);
                    return;
                }
                cls->students.append(student->uuid);
                const auto future = aaims::manager::account::saveAsync();;

                auto *watcher = new QFutureWatcher<void>(this); // NOLINT

                connect(watcher, &QFutureWatcherBase::finished, [this, watcher, pd] {
                    pd->close();
                    pd->deleteLater();
                    watcher->deleteLater();
                    QMessageBox::information(this, "添加完成", QString("添加学生成功！"));
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

        const auto &future = QtConcurrent::run(&AddStudentDialog::importFromCsv, this);
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

QPair<unsigned long long, unsigned long long> AddStudentDialog::importFromCsv() const {
    static const QString password = Sha256Util::hash("123456");
    static const QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    unsigned long long succeed = 0, failed = 0;
    QFile file(selectedFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {0, 0};

    QTextStream in(&file);
    in.readLine(); // Skip first line

    auto classes = aaims::manager::classes::get_all_ptr();
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

        const QString username = fields[0].trimmed();
        const QString name = fields[1].trimmed();
        const bool isFemale = fields[2].trimmed() == "女";
        const QString cls = fields[3].trimmed();
        const QString dormitory = fields[4].trimmed();
        const QString phone = fields[5].trimmed();
        if (username.isEmpty() || name.isEmpty() || cls.isEmpty() || phone.length() != 11 || !phoneRegex.match(phone).
            isValid()) {
            failed++;
            continue;
        }
        if (aaims::manager::account::findByUsername(username)) {
            failed++;
            continue;
        }
        const auto it = std::ranges::find_if(classes, [cls](const auto *x) {
            return QString(x->grade).append(x->name) == cls;
        });
        if (it == classes.end()) {
            failed++;
            continue;
        }
        auto student = std::make_shared<StudentAccount>();
        student->username = username;
        student->name = name;
        student->password = password;
        student->female = isFemale;
        student->status = 0;
        student->currentClass = (*it)->uuid;
        student->dormitory = dormitory;
        student->phoneNumber = phone;

        if (const QString result = aaims::manager::account::add(student); !result.isEmpty()) {
            failed++;
            continue;
        }
        (*it)->students.append(student->uuid);
        succeed++;
    }
    aaims::manager::classes::saveDepartments();
    aaims::manager::account::save(); // This is synchronized!!!

    return {succeed, failed};
}
