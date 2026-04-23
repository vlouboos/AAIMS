// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AdminAuthPage.h"

#include <QFutureWatcher>
#include <QHeaderView>
#include <QProgressDialog>

#include "../dialogs/AddClassDialog.h"
#include "../dialogs/ClassDetailDialog.h"
#include "../managements/ClassManager.h"
#include "../utils/Sha256Util.h"
#include "delegate/AuthDelegate.h"
#include "delegate/OperationDelegate.h"
#include "model/FilterProxyModel.h"

AdminAuthPage::AdminAuthPage(QWidget *parent) : QWidget(parent) {
    tableModel = new AuthTableModel(this);
    proxyModel = new FilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    headerLayout = new QHBoxLayout();

    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("班级管理", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0f172a;");

    subtitleLabel = new QLabel("加载中", this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索账号用户名、姓名...");
    searchEdit->setFixedWidth(280);
    searchEdit->setObjectName("SearchEdit");

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(searchEdit);

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

    auto *delegate = new AuthDelegate(this);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setColumnWidth(0, 120);
    tableView->setColumnWidth(1, 120);
    tableView->setColumnWidth(4, 120);
    tableView->setItemDelegateForColumn(4, delegate);

    mainLayout->addWidget(tableView);

    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }

    connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
        proxyModel->setFilterFixedString(text);
    });

    connect(delegate, &AuthDelegate::confirmReset, [this](const QModelIndex &index) {
        if (Account *account = aaims::manager::account::all()[tableModel->getAccount(
            proxyModel->mapToSource(index))].get()) {
            if (account->is_master()) {
                QMessageBox::warning(this, "无法重置", "无法重置主管理员的密码！", QMessageBox::Ok);
                return;
            }
            if (account->uuid == aaims::manager::account::logged->uuid) {
                QMessageBox::warning(this, "无法重置", "无法重置自己的密码！", QMessageBox::Ok);
                return;
            }
            if (QMessageBox::warning(this, "确认", "确认重置该用户的密码？", QMessageBox::Yes | QMessageBox::No) ==
                QMessageBox::Yes) {
                auto *pd = new QProgressDialog("正在重置...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();
                account->password = Sha256Util::hash("123456");
                const auto future = aaims::manager::account::saveAsync();
                auto watcher = new QFutureWatcher<bool>(this); // NOLINT
                connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
                    pd->close();
                    watcher->deleteLater();
                    QMessageBox::information(this, "重置完成", QString("重置密码成功！新密码为 123456"));
                });
                watcher->setFuture(future);
            }
        }
    });

    connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex &index) {
                if (Class *cls = aaims::manager::classes::get_classes()[tableModel->getAccount(
                    proxyModel->mapToSource(index))].get()) {
                    if (ClassDetailDialog dialog(cls, this); dialog.exec() == QDialog::Accepted) {
                        reloadData();
                    }
                }
            }
    );

    reloadData();
}


void AdminAuthPage::reloadData() const {
    tableModel->setAccounts(aaims::manager::account::all().keys());
    proxyModel->sort(0);
    subtitleLabel->setText(QString("管理系统内共 %1 个账号").arg(tableModel->rowCount(QModelIndex())));
}
