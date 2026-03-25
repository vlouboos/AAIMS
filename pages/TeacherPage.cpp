// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherPage.h"

#include <QFutureWatcher>
#include <QProgressDialog>

#include "../dialogs/AddTeacherDialog.h"
#include "../dialogs/TeacherDetailDialog.h"
#include "../managements/AccountManager.h"
#include "delegate/OperationDelegate.h"
#include "model/FilterProxyModel.h"

TeacherPage::TeacherPage(QWidget *parent) : QWidget(parent) {
    tableModel = new TeacherTableModel(this);
    proxyModel = new FilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    headerLayout = new QHBoxLayout();

    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("教师管理", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0f172a;");

    subtitleLabel = new QLabel("加载中", this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索教师姓名、工号...");
    searchEdit->setFixedWidth(280);
    searchEdit->setObjectName("SearchEdit");

    btnAddTeacher = new QPushButton("+ 新增教师", this);
    btnAddTeacher->setCursor(Qt::PointingHandCursor);
    btnAddTeacher->setObjectName("AddElement");

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(searchEdit);
    headerLayout->addWidget(btnAddTeacher);

    mainLayout->addLayout(headerLayout);

    tableView = new QTableView(this);
    tableView->setModel(proxyModel);
    tableView->setShowGrid(true);
    tableView->setGridStyle(Qt::SolidLine);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setAlternatingRowColors(true);
    tableView->setFocusPolicy(Qt::NoFocus);
    tableView->verticalHeader()->setVisible(false);

    auto *header = tableView->horizontalHeader();
    header->setSectionsMovable(false);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Fixed);
    header->setSectionResizeMode(5, QHeaderView::Fixed);

    auto *delegate = new OperationDelegate(this);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setColumnWidth(0, 120);
    tableView->setColumnWidth(1, 120);
    tableView->setColumnWidth(4, 100);
    tableView->setColumnWidth(5, 120);
    tableView->setItemDelegateForColumn(5, delegate);

    mainLayout->addWidget(tableView);

    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }

    connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
        proxyModel->setFilterFixedString(text);
    });

    connect(btnAddTeacher, &QPushButton::clicked, [this] {
        if (AddTeacherDialog dialog(this); dialog.exec() == QDialog::Accepted) {
            reloadData();
        }
    });

    connect(delegate, &OperationDelegate::openEdit, [this](const QModelIndex &index) {
        if (TeacherAccount *account = aaims::manager::account::get_teachers()[tableModel->getAccount(
            proxyModel->mapToSource(index))]) {
            if (TeacherDetailDialog dialog(account, this); dialog.exec() == QDialog::Accepted) {
                reloadData();
            }
        }
    });

    connect(delegate, &OperationDelegate::confirmDelete, [this](const QModelIndex &index) {
        if (TeacherAccount *account = aaims::manager::account::get_teachers()[tableModel->getAccount(
            proxyModel->mapToSource(index))]) {
            if (account->is_occupied()) {
                QMessageBox::critical(this, "非法操作", "该教师有课程或是班主任，请先转移课程或转移班级！", QMessageBox::Ok);
                return;
            }
            const auto result = QMessageBox::warning(this, "危险操作",
                                                     QString("确定要删除教师 %1 (%2) 吗？\n该操作不可撤销！").arg(
                                                         account->name, account->username),
                                                     QMessageBox::Yes | QMessageBox::No);

            if (result == QMessageBox::Yes) {
                auto *pd = new QProgressDialog("正在删除...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();
                aaims::manager::account::remove(account);
                reloadData();
                const auto future = aaims::manager::account::saveAsync();
                auto watcher = new QFutureWatcher<bool>(this); // NOLINT
                connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
                    pd->close();
                    pd->deleteLater();
                    watcher->deleteLater();
                    QMessageBox::information(this, "删除完成", QString("删除教师成功！"));
                });
                watcher->setFuture(future);
            }
        }
    });

    connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex &index) {
                if (TeacherAccount *account = aaims::manager::account::get_teachers()[tableModel->getAccount(
                    proxyModel->mapToSource(index))]) {
                    if (TeacherDetailDialog dialog(account, this); dialog.exec() == QDialog::Accepted) {
                        reloadData();
                    }
                }
            }
    );

    reloadData();
}


void TeacherPage::reloadData() const {
    tableModel->setTeachers(aaims::manager::account::get_teachers().keys());
    proxyModel->sort(0);
    subtitleLabel->setText(QString("管理系统内共 %1 名教师").arg(tableModel->rowCount(QModelIndex())));
}
