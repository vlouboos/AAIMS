// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AssignmentRoleSlot.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCompleter>
#include <QMessageBox>

#include "../../managements/AccountManager.h"
#include "../../managements/ClassManager.h"

AssignmentRoleSlot::AssignmentRoleSlot(const Course::AssignmentRule &rule, QWidget *parent) : QWidget(parent),
    isFemale(rule.isFemale) {
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    comboRuleType = new QComboBox(this);
    comboRuleType->addItem("院系", Course::AssignmentRule::DEPARTMENT);
    comboRuleType->addItem("班级", Course::AssignmentRule::CLASS);
    comboRuleType->addItem("性别", Course::AssignmentRule::GENDER);
    comboRuleType->addItem("年级", Course::AssignmentRule::GRADE);
    comboRuleType->addItem("专业", Course::AssignmentRule::MAJOR);
    comboRuleType->addItem("学生", Course::AssignmentRule::SINGLE);
    
    // Set current selection based on rule status
    int currentIndex = 0;
    if (rule.specific_department()) {
        currentIndex = 0;
        targetDepartments = rule.targetDepartments;
    } else if (rule.specific_class()) {
        currentIndex = 1;
        targetClasses = rule.targetClasses;
    } else if (rule.specific_gender()) {
        currentIndex = 2;
        isFemale = rule.isFemale;
    } else if (rule.specific_grade()) {
        currentIndex = 3;
        targetGrades = rule.targetGrades;
    } else if (rule.specific_major()) {
        currentIndex = 4;
        targetMajors = rule.targetMajors;
    } else if (rule.specific_single()) {
        currentIndex = 5;
        specificStudents = rule.specificStudents;
    }
    comboRuleType->setCurrentIndex(currentIndex);

    // Create target selection area
    targetList = new QListWidget(this);
    targetList->setMaximumHeight(100);

    // Populate target list based on rule type
    int ruleType = comboRuleType->currentData().toInt();
    switch (ruleType) {
        case Course::AssignmentRule::DEPARTMENT: {
            for (const auto &dept : targetDepartments) {
                auto *item = new QListWidgetItem(dept, targetList);
                item->setCheckState(Qt::Checked);
            }
            break;
        }
        case Course::AssignmentRule::CLASS: {
            for (const auto &classUuid : targetClasses) {
                const auto &allClasses = aaims::manager::classes::get_classes();
                if (allClasses.contains(classUuid)) {
                    auto *item = new QListWidgetItem(allClasses[classUuid]->name, targetList);
                    item->setData(Qt::UserRole, classUuid.toString());
                    item->setCheckState(Qt::Checked);
                }
            }
            break;
        }
        case Course::AssignmentRule::GENDER: {
            auto *maleItem = new QListWidgetItem("男", targetList);
            maleItem->setData(Qt::UserRole, "male");
            maleItem->setCheckState(isFemale ? Qt::Unchecked : Qt::Checked);
            
            auto *femaleItem = new QListWidgetItem("女", targetList);
            femaleItem->setData(Qt::UserRole, "female");
            femaleItem->setCheckState(isFemale ? Qt::Checked : Qt::Unchecked);
            break;
        }
        case Course::AssignmentRule::GRADE: {
            for (const auto &grade : targetGrades) {
                auto *item = new QListWidgetItem(grade, targetList);
                item->setCheckState(Qt::Checked);
            }
            break;
        }
        case Course::AssignmentRule::MAJOR: {
            for (const auto &majorUuid : targetMajors) {
                const auto &allMajors = aaims::manager::classes::get_majors();
                if (allMajors.contains(majorUuid)) {
                    auto *item = new QListWidgetItem(allMajors[majorUuid]->name, targetList);
                    item->setData(Qt::UserRole, majorUuid.toString());
                    item->setCheckState(Qt::Checked);
                }
            }
            break;
        }
        case Course::AssignmentRule::SINGLE: {
            for (const auto &studentUuid : specificStudents) {
                const auto &allStudents = aaims::manager::account::get_working_students();
                if (allStudents.contains(studentUuid)) {
                    auto *item = new QListWidgetItem(allStudents[studentUuid]->name, targetList);
                    item->setData(Qt::UserRole, studentUuid.toString());
                    item->setCheckState(Qt::Checked);
                }
            }
            break;
        }
    }

    btnRemove = new QPushButton("-", this);
    btnRemove->setFixedSize(30, 30);

    layout->addWidget(comboRuleType);
    layout->addWidget(targetList);
    layout->addWidget(btnRemove);

    // Only connect the signal if we want the user to be able to change rule type
    // For now, we'll disable changing rule type to preserve the original rule's meaning
    comboRuleType->setEnabled(false);  // Disable changing rule type after creation
    
    connect(btnRemove, &QPushButton::clicked, this, &AssignmentRoleSlot::removeRequested);
}



Course::AssignmentRule AssignmentRoleSlot::toData() const {
    Course::AssignmentRule rule;
    int ruleType = comboRuleType->currentData().toInt();
    rule.status = ruleType;
    
    // Get all checked items from the list
    for (int i = 0; i < targetList->count(); ++i) {
        QListWidgetItem *item = targetList->item(i);
        if (item->checkState() == Qt::Checked) {
            switch (ruleType) {
                case Course::AssignmentRule::DEPARTMENT:
                    rule.targetDepartments.append(item->text());
                    break;
                case Course::AssignmentRule::CLASS: {
                    QUuid classUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
                    if (classUuid != aaims::model::EMPTY_UUID) {
                        rule.targetClasses.append(classUuid);
                    }
                    break;
                }
                case Course::AssignmentRule::GENDER:
                    rule.isFemale = (item->text() == "女");
                    break;
                case Course::AssignmentRule::GRADE:
                    rule.targetGrades.append(item->text());
                    break;
                case Course::AssignmentRule::MAJOR: {
                    QUuid majorUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
                    if (majorUuid != aaims::model::EMPTY_UUID) {
                        rule.targetMajors.append(majorUuid);
                    }
                    break;
                }
                case Course::AssignmentRule::SINGLE: {
                    QUuid studentUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
                    if (studentUuid != aaims::model::EMPTY_UUID) {
                        rule.specificStudents.append(studentUuid);
                    }
                    break;
                }
            }
        }
    }
    
    return rule;
}