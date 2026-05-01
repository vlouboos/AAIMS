// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddMajorDialog.h"

#include <QCompleter>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QProgressDialog>

#include "AddDepartmentDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../utils/AsyncJsonIO.h"
#include "../utils/DataStructures.h"
#include "../utils/Sha256Util.h"

using namespace aaims::model;

AddMajorDialog::AddMajorDialog(QWidget *parent) : StyledDialog(parent) {
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

    nameEdit = new QLineEdit(singleAddPage);
    nameEdit->setPlaceholderText("例如: 软件工程");

    deptLayout = new QHBoxLayout();

    completer = new QCompleter();
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    deptCombo = new QComboBox(singleAddPage);
    deptCombo->addItems(aaims::manager::classes::get_departments());
    deptCombo->setEditable(true);
    deptCombo->setPlaceholderText("请选择学院");
    deptCombo->setInsertPolicy(QComboBox::NoInsert);
    deptCombo->setCompleter(completer);

    btnAddDept = new QPushButton("+", singleAddPage);
    btnAddDept->setStyleSheet("padding: 0; margin: 0;");
    btnAddDept->setObjectName("AddElement");
    btnAddDept->setFixedSize(24, 24);

    deptLayout->addWidget(deptCombo);
    deptLayout->addWidget(btnAddDept);

    btnConfirmSingle = new QPushButton("确认添加", singleAddPage);
    btnConfirmSingle->setCursor(Qt::PointingHandCursor);
    btnConfirmSingle->setObjectName("AddElement");

    singleLayout->addRow("专业名称:", nameEdit);
    singleLayout->addRow("所属院系:", deptLayout);
    singleLayout->addRow("", btnConfirmSingle);

    batchAddPage = new QWidget();

    batchLayout = new QVBoxLayout(batchAddPage);
    batchLayout->setContentsMargins(30, 30, 30, 30);
    batchLayout->setSpacing(20);

    tipLabel = new QLabel("支持导入 .csv 格式的文件。\n请确保列头包含: 专业名称, 所属院系。\n例: 软件工程, 软件学院", batchAddPage);
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
                if (nameEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "输入错误", "专业名称不能为空！");
                    return;
                }
                if (deptCombo->currentIndex() == -1) {
                    QMessageBox::warning(this, "输入错误", "请选择院系！");
                    return;
                }
                if (std::ranges::any_of(aaims::manager::classes::get_majors(), [this](const auto &m) {
                    return m.get() && m->name == nameEdit->text().trimmed();
                })) {
                    QMessageBox::warning(this, "输入错误", "重名的专业已存在！");
                    return;
                }
                auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();

                const auto major = std::make_shared<Major>();
                major->name = nameEdit->text().trimmed();
                major->department = deptCombo->currentText().trimmed();
                if (const QString result = aaims::manager::classes::addMajor(major); !result.isEmpty()) {
                    pd->close();
                    pd->deleteLater();
                    QMessageBox::critical(this, "错误", result);
                    return;
                }
                const auto future = QtConcurrent::run([] { return aaims::manager::classes::saveMajors(); });

                auto *watcher = new QFutureWatcher<void>(this); // NOLINT

                connect(watcher, &QFutureWatcherBase::finished, [this, watcher, pd] {
                    pd->close();
                    pd->deleteLater();
                    watcher->deleteLater();
                    QMessageBox::information(this, "添加完成", QString("添加专业成功！"));
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

        const auto &future = QtConcurrent::run(&AddMajorDialog::importFromCsv, this);
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

QPair<unsigned long long, unsigned long long> AddMajorDialog::importFromCsv() const {
    unsigned long long succeed = 0, failed = 0;
    aaims::io::loadCsv(selectedFilePath, [&succeed, &failed](const auto &lines) {
        for (const auto &line: lines) {
            if (line.trimmed().isEmpty()) {
                continue;
            }

            QStringList fields = line.split(",");
            if (fields.size() < 2) {
                failed++;
                continue;
            }

            const QString name = fields[0].trimmed();
            const QString dept = fields[1].trimmed();
            if (name.isEmpty()) {
                failed++;
                continue;
            }
            if (aaims::manager::classes::findMajorByName(name)) {
                failed++;
                continue;
            }
            if (!aaims::manager::classes::get_departments().contains(dept)) {
                aaims::manager::classes::addDepartment({dept});
            }
            auto major = std::make_shared<Major>();
            major->name = name;
            major->department = dept;

            if (const QString result = aaims::manager::classes::addMajor(major); !result.isEmpty()) {
                failed++;
                continue;
            }
            succeed++;
        }
    });
    aaims::manager::classes::saveDepartments();
    aaims::manager::classes::saveMajors(); // This is synchronized!!!
    return {succeed, failed};
}
