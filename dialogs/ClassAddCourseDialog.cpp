// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "ClassAddCourseDialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "../managements/CourseManager.h"

ClassAddCourseDialog::ClassAddCourseDialog(const QList<QUuid> &currentCourses, QWidget *parent)
    : StyledDialog(parent), currentCourses(currentCourses), allCourses(aaims::manager::course::get_courses()) {
    setWindowTitle("添加课程");
    resize(500, 600);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("搜索课程...");
    connect(searchEdit, &QLineEdit::textChanged, this, &ClassAddCourseDialog::filterCourses);

    availableList = new QListWidget(this);
    availableList->setSelectionMode(QAbstractItemView::MultiSelection);

    btnAdd = new QPushButton("添加选中", this);
    connect(btnAdd, &QPushButton::clicked, this, &ClassAddCourseDialog::onAddButtonClicked);

    addedArea = new QScrollArea(this);
    addedWidget = new QWidget();
    addedLayout = new QVBoxLayout(addedWidget);
    addedArea->setWidget(addedWidget);
    addedArea->setWidgetResizable(true);

    auto *btnLayout = new QHBoxLayout();
    btnOk = new QPushButton("确定", this);
    btnCancel = new QPushButton("取消", this);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    mainLayout->addWidget(searchEdit);
    mainLayout->addWidget(availableList);
    mainLayout->addWidget(btnAdd);
    mainLayout->addWidget(addedArea);
    mainLayout->addLayout(btnLayout);

    applyStyles();
    populateAvailable();
}

void ClassAddCourseDialog::populateAvailable() {
    availableList->clear();
    QString searchText = searchEdit->text();
    for (const auto &[uuid, course] : allCourses.asKeyValueRange()) {
        if (!currentCourses.contains(uuid) && !addedCourses.contains(uuid)) {
            if (searchText.isEmpty() ||
                course->id.contains(searchText, Qt::CaseInsensitive) ||
                course->name.contains(searchText, Qt::CaseInsensitive)) {
                auto *item = new QListWidgetItem(QString("%1-%2").arg(course->id, course->name));
                item->setData(Qt::UserRole, uuid);
                availableList->addItem(item);
            }
        }
    }
}

void ClassAddCourseDialog::filterCourses() {
    populateAvailable();
}

void ClassAddCourseDialog::onAddButtonClicked() {
    const auto selectedItems = availableList->selectedItems();
    for (auto *item : selectedItems) {
        QUuid uuid = item->data(Qt::UserRole).value<QUuid>();
        addedCourses.append(uuid);
        addToAddedList(uuid);
        delete availableList->takeItem(availableList->row(item));
    }
}

void ClassAddCourseDialog::addToAddedList(const QUuid &uuid) {
    const auto &course = allCourses[uuid];
    auto *hLayout = new QHBoxLayout();
    auto *label = new QLabel(QString("%1-%2").arg(course->id, course->name));
    auto *btnDelete = new QPushButton("删除");
    connect(btnDelete, &QPushButton::clicked, [this, uuid]() { onDeleteButtonClicked(uuid); });
    hLayout->addWidget(label);
    hLayout->addWidget(btnDelete);
    addedLayout->addLayout(hLayout);
    addedLayouts[uuid] = hLayout;
}

void ClassAddCourseDialog::onDeleteButtonClicked(const QUuid &uuid) {
    if (addedLayouts.contains(uuid)) {
        auto *layout = addedLayouts[uuid];
        addedLayout->removeItem(layout);
        delete layout;
        addedLayouts.remove(uuid);
        addedCourses.removeOne(uuid);
        populateAvailable();  // to add back to available
    }
}

QList<QUuid> ClassAddCourseDialog::getAddedCourses() const {
    return addedCourses;
}
