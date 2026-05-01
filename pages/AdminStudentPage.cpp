// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AdminStudentPage.h"

#include <QFutureWatcher>
#include <QProgressDialog>
#include <qtconcurrentrun.h>

#include "../dialogs/AddStudentDialog.h"
#include "../dialogs/StudentDetailDialog.h"
#include "../managements/AccountManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"
#include "../utils/AsyncJsonIO.h"
#include "delegate/OperationDelegate.h"
#include "model/FilterProxyModel.h"

AdminStudentPage::AdminStudentPage(QWidget *parent) : QWidget(parent) {
    tableModel = new StudentTableModel(this);
    proxyModel = new FilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    headerLayout = new QHBoxLayout();

    titleContainer = new QVBoxLayout();

    titleLabel = new QLabel("学生管理", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0f172a;");

    subtitleLabel = new QLabel("加载中", this);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #64748b;");

    titleContainer->addWidget(titleLabel);
    titleContainer->addWidget(subtitleLabel);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索学生姓名、学号...");
    searchEdit->setFixedWidth(280);
    searchEdit->setObjectName("SearchEdit");

    btnStat = new QPushButton("统计已修学分及不及级课程", this);
    btnStat->setCursor(Qt::PointingHandCursor);
    btnStat->setObjectName("AddElement");

    btnAddStudent = new QPushButton("+ 新增学生", this);
    btnAddStudent->setCursor(Qt::PointingHandCursor);
    btnAddStudent->setObjectName("AddElement");

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(searchEdit);
    headerLayout->addWidget(btnStat);
    headerLayout->addWidget(btnAddStudent);

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
    tableView->setColumnWidth(0, 140);
    tableView->setColumnWidth(1, 100);
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

    connect(btnStat, &QPushButton::clicked, [this] {
        const QString path = QFileDialog::getExistingDirectory(this, "选择目标文件夹",
                                                               QCoreApplication::applicationDirPath(),
                                                               QFileDialog::ShowDirsOnly);
        if (path.isEmpty()) {
            return;
        }
        if (const QFileInfo info(path); !info.isWritable()) {
            QMessageBox::warning(this, "错误", "选定的目录没有写入权限！");
        } else {
            auto *pd = new QProgressDialog("正在导出...", nullptr, 0, 0, this); // NOLINT
            pd->setWindowModality(Qt::WindowModal);
            pd->show();
            const auto future = QtConcurrent::run([path] {
                QStringList lines;
                for (const auto &student: aaims::manager::account::get_students()) {
                    const auto &rating = aaims::manager::rating::get_ratings()[student->uuid];
                    long long credits = 0;
                    long long failCount = 0;
                    QStringList failCourses;
                    if (rating.get()) {
                        for (const auto &[key, r]: rating->ratings.asKeyValueRange()) {
                            const auto &course = aaims::manager::course::get_courses()[key];
                            if (r.finalScore >= 60 && r.score >= 60) {
                                credits += course->credit;
                            } else {
                                failCount++;
                                failCourses.append(course->name);
                            }
                        }
                    }
                    lines.append(QString("%1,%2,%3,%4,%5").arg(student->username, student->name,
                                                               QString::number(credits), QString::number(failCount),
                                                               failCourses.join(";")));
                }
                if (!lines.isEmpty()) {
                    constexpr auto h = "学号,姓名,已修学分,不及格课程数,不及格课程";
                    aaims::io::saveCsv(
                        QString("%1/%2-学生已修学分及不及格课程.csv").arg(path, QDate::currentDate().toString("yyyy-MM-dd")),
                        lines, h);
                }
            });
            const auto watcher = new QFutureWatcher<void>(this); // NOLINT
            connect(watcher, &QFutureWatcher<void>::finished, [this, pd, watcher] {
                pd->close();
                pd->deleteLater();
                watcher->deleteLater();
                QMessageBox::information(this, "成功", "导出完成！");
            });
            watcher->setFuture(future);
        }
    });

    connect(btnAddStudent, &QPushButton::clicked, [this] {
        if (AddStudentDialog dialog(this); dialog.exec() == QDialog::Accepted) {
            reloadData();
        }
    });

    connect(delegate, &OperationDelegate::openEdit, [this](const QModelIndex &index) {
        if (StudentAccount *account = aaims::manager::account::get_students()[tableModel->getAccount(
            proxyModel->mapToSource(index))]) {
            if (StudentDetailDialog dialog(account, this); dialog.exec() == QDialog::Accepted) {
                reloadData();
            }
        }
    });

    connect(delegate, &OperationDelegate::confirmDelete, [this](const QModelIndex &index) {
        if (StudentAccount *account = aaims::manager::account::get_students()[tableModel->getAccount(
            proxyModel->mapToSource(index))]) {
            const auto result = QMessageBox::warning(this, "危险操作",
                                                     QString("确定要删除学生 %1 (%2) 吗？\n该操作不可撤销！").arg(
                                                         account->name, account->username),
                                                     QMessageBox::Yes | QMessageBox::No);

            if (result == QMessageBox::Yes) {
                auto *pd = new QProgressDialog("正在删除...", nullptr, 0, 0, this); // NOLINT
                pd->setWindowModality(Qt::WindowModal);
                pd->show();
                const auto cls = aaims::manager::classes::get_classes()[account->currentClass];
                cls->students.removeOne(account->uuid);
                aaims::manager::account::remove(account);
                reloadData();
                const auto future = QtConcurrent::run([] {
                    return aaims::manager::classes::saveClasses() && aaims::manager::account::save();
                });;
                auto watcher = new QFutureWatcher<bool>(this); // NOLINT
                connect(watcher, &QFutureWatcherBase::finished, [this, pd, watcher] {
                    pd->close();
                    pd->deleteLater();
                    watcher->deleteLater();
                    QMessageBox::information(this, "删除完成", QString("删除学生成功！"));
                });
                watcher->setFuture(future);
            }
        }
    });


    connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex &index) {
        if (StudentAccount *account = aaims::manager::account::get_students()[tableModel->getAccount(
            proxyModel->mapToSource(index))]) {
            if (StudentDetailDialog dialog(account, this); dialog.exec() == QDialog::Accepted) {
                reloadData();
            }
        }
    });

    reloadData();
}

void AdminStudentPage::reloadData() const {
    tableModel->setStudents(aaims::manager::account::get_students().keys());
    proxyModel->sort(0);
    subtitleLabel->setText(QString("管理系统内共 %1 名学生").arg(tableModel->rowCount(QModelIndex())));
}
