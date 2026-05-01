// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "TeacherClassPage.h"

#include <QFile>
#include <QHeaderView>
#include <QCoreApplication>
#include <QDate>
#include <QMessageBox>
#include <QProgressDialog>

#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"
#include "../managements/CourseManager.h"
#include "../managements/RatingManager.h"
#include "../utils/AsyncJsonIO.h"

TeacherClassPage::TeacherClassPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadClassData();
    
    if (QFile file(":/assets/style.qss"); file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
    }
}

void TeacherClassPage::setupUI() {
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

    btnStat = new QPushButton("统计已修学分及不及格课程", this);
    btnStat->setObjectName("AddElement");

    btnExportGrades = new QPushButton("导出成绩", this);
    btnExportGrades->setObjectName("AddElement");
    btnExportGrades->setFixedWidth(120);

    headerLayout->addLayout(titleContainer);
    headerLayout->addStretch();
    headerLayout->addWidget(btnStat);
    headerLayout->addWidget(btnExportGrades);

    mainLayout->addLayout(headerLayout);

    tableModel = new ClassMemberTableModel(this);

    tableView = new QTableView(this);
    tableView->setModel(tableModel);
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
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    mainLayout->addWidget(tableView);

    connect(btnStat, &QPushButton::clicked, this, &TeacherClassPage::onStatGrades);
    connect(btnExportGrades, &QPushButton::clicked, this, &TeacherClassPage::onExportGrades);
}

void TeacherClassPage::loadClassData() const {
    if (const auto &teacher = aaims::manager::account::get_teachers()[aaims::manager::account::logged->uuid]; teacher->is_class_master()) {
        for (const auto &allClasses = aaims::manager::classes::get_classes(); const auto &[classUuid, cls] : allClasses.asKeyValueRange()) {
            if (cls->master == teacher->uuid) {
                tableModel->setClasses(cls->students);
                subtitleLabel->setText(QString("班级 %1%2 共 %3 名学生").arg(cls->grade).arg(cls->name).arg(cls->students.size()));
                return;
            }
        }
    }
    
    subtitleLabel->setText("您不是任何班级的班主任");
}

void TeacherClassPage::onExportGrades() {
    const auto &teacher = aaims::manager::account::get_teachers()[aaims::manager::account::logged->uuid];
    
    if (!teacher->is_class_master()) {
        QMessageBox::warning(this, "提示", "您不是任何班级的班主任，无法导出成绩");
        return;
    }
    
    const auto &allClasses = aaims::manager::classes::get_classes();
    Class *cls = nullptr;
    
    for (const auto &[classUuid, classPtr] : allClasses.asKeyValueRange()) {
        if (classPtr->master == teacher->uuid) {
            cls = classPtr.get();
            break;
        }
    }
    
    if (!cls) {
        QMessageBox::warning(this, "错误", "未找到您管理的班级");
        return;
    }
    
    const QString path = QFileDialog::getExistingDirectory(this, "选择目标文件夹", QCoreApplication::applicationDirPath(),
                                                           QFileDialog::ShowDirsOnly);
    if (path.isEmpty()) {
        return;
    }
    
    if (const QFileInfo info(path); !info.isWritable()) {
        QMessageBox::warning(this, "错误", "选定的目录没有写入权限！");
        return;
    }
    
    auto *pd = new QProgressDialog("正在导出...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    
    const auto future = QtConcurrent::run([cls, path] {
        for (const auto &courseUuid: cls->courses) {
            const auto &course = aaims::manager::course::get_courses()[courseUuid];
            constexpr auto header = "学号,姓名,平时分,期末成绩,总成绩";
            QStringList lines;
            for (const auto &studentUuid: cls->students) {
                const auto &student = aaims::manager::account::get_students()[studentUuid];
                const auto &rating = aaims::manager::rating::get_ratings()[studentUuid];
                double performance = 0.0, score = 0.0, finalScore = 0.0;
                if (rating.get()) {
                    performance = rating->ratings[course->uuid].performance;
                    score = rating->ratings[course->uuid].score;
                    finalScore = rating->ratings[course->uuid].finalScore;
                }
                const QString line = QString("%1,%2,%3,%4,%5")
                        .arg(student->username, student->name, QString::number(performance),
                             QString::number(score), QString::number(finalScore));
                lines.append(line);
            }
            aaims::io::saveCsv(
                QString("%1/%2%3-%4-%5-%6成绩.csv").arg(path, cls->grade, cls->name, course->semester, course->id, course->name),
                lines, header);
        }
        QStringList lines;
        for (const auto &studentUuid: cls->students) {
            for (const auto &student = aaims::manager::account::get_students()[studentUuid]; const auto &[uuid,
                         retake]: student->lessons) {
                const auto &course = aaims::manager::course::get_courses()[uuid];
                const auto &rating = aaims::manager::rating::get_ratings()[studentUuid];
                double performance = 0.0, score = 0.0, finalScore = 0.0;
                if (rating.get()) {
                    performance = rating->ratings[course->uuid].performance;
                    score = rating->ratings[course->uuid].score;
                    finalScore = rating->ratings[course->uuid].finalScore;
                }
                const QString line = QString("%1,%2,%3,%4,%5,%6,%7").arg(
                    student->username, student->name, course->name, retake == 0 ? "自选" : QString("重修%1次").arg(retake),
                    QString::number(performance), QString::number(score),
                    QString::number(finalScore));
                lines.append(line);
            }
        }
        if (!lines.isEmpty()) {
            constexpr auto header = "学号,姓名,课程,类型,平时分,期末成绩,总成绩";
            aaims::io::saveCsv(
                QString("%1/%2%3-%4-%5个人课程成绩.csv").arg(path, cls->grade, cls->name,
                                                               QDate::currentDate().toString("yyyy-MM-dd"), cls->name),
                lines, header);
        }
    });
    
    const auto watcher = new QFutureWatcher<void>(this); // NOLINT
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher, pd] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "成功", "成绩导出完成！", QMessageBox::Ok);
    });
    watcher->setFuture(future);
}

void TeacherClassPage::onStatGrades() {
    const auto &teacher = aaims::manager::account::get_teachers()[aaims::manager::account::logged->uuid];
    
    if (!teacher->is_class_master()) {
        QMessageBox::warning(this, "提示", "您不是任何班级的班主任，无法统计成绩");
        return;
    }
    
    const auto &allClasses = aaims::manager::classes::get_classes();
    Class *cls = nullptr;
    
    for (const auto &[classUuid, classPtr] : allClasses.asKeyValueRange()) {
        if (classPtr->master == teacher->uuid) {
            cls = classPtr.get();
            break;
        }
    }
    
    if (!cls) {
        QMessageBox::warning(this, "错误", "未找到您管理的班级");
        return;
    }
    
    const QString path = QFileDialog::getExistingDirectory(this, "选择目标文件夹", QCoreApplication::applicationDirPath(),
                                                           QFileDialog::ShowDirsOnly);
    if (path.isEmpty()) {
        return;
    }
    
    if (const QFileInfo info(path); !info.isWritable()) {
        QMessageBox::warning(this, "错误", "选定的目录没有写入权限！");
        return;
    }
    
    auto *pd = new QProgressDialog("正在统计...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    
    const auto future = QtConcurrent::run([cls, path] {
        QStringList lines;
        for (const auto &studentUuid: cls->students) {
            const auto &student = aaims::manager::account::get_students()[studentUuid];
            const auto &rating = aaims::manager::rating::get_ratings()[studentUuid];
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
                QString("%1/%2%3-%4-学生已修学分及不及格课程.csv").arg(path, cls->grade, cls->name,
                                                                                   QDate::currentDate().toString("yyyy-MM-dd")),
                lines, h);
        }
    });
    
    const auto watcher = new QFutureWatcher<void>(this); // NOLINT
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher, pd] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        QMessageBox::information(this, "成功", "统计完成！");
    });
    watcher->setFuture(future);
}