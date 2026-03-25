// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AddDepartmentDialog.h"

#include <QFutureWatcher>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <qtconcurrentrun.h>

#include "../managements/ClassManager.h"

AddDepartmentDialog::AddDepartmentDialog(QWidget *parent) : StyledDialog(parent) {
    setWindowTitle("添加学院");
    setFixedSize(450, 280);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setAlignment(Qt::AlignCenter);

    label = new QLabel("添加学院(一行一个)");

    edit = new QPlainTextEdit(this);
    edit->setPlaceholderText("在此输入学院(一行一个)");
    edit->setFixedHeight(150);

    btnConfirm = new QPushButton("确认");
    btnConfirm->setObjectName("AddElement");
    btnConfirm->setFixedWidth(100);

    mainLayout->addWidget(label);
    mainLayout->addWidget(edit);
    mainLayout->addWidget(btnConfirm);
    applyStyles();

    connect(btnConfirm, &QPushButton::clicked, this, [this] {
        auto *pd = new QProgressDialog("正在添加...", nullptr, 0, 0, this); // NOLINT
        pd->setWindowModality(Qt::WindowModal);
        pd->show();
        const auto &[succeed, fail] = aaims::manager::classes::addDepartment(
            edit->toPlainText().split('\n', Qt::SkipEmptyParts));
        const auto future = QtConcurrent::run([] {
            aaims::manager::classes::saveDepartments();
        });
        const auto watcher = new QFutureWatcher<void>(this); // NOLINT
        connect(watcher, &QFutureWatcher<void>::finished, [this, succeed, fail, pd, watcher] {
            pd->close();
            pd->deleteLater();
            watcher->deleteLater();
            QMessageBox::information(this, "添加完成", QString("成功添加%1个学院(%2个重复)").arg(succeed).arg(fail));
            accept();
        });
        watcher->setFuture(future);
    });
}
