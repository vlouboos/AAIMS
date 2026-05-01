// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "AssignmentRuleDialog.h"

#include <QLineEdit>
#include <QMessageBox>

#include "../managements/AccountManager.h"
#include "../managements/ClassManager.h"

AssignmentRuleDialog::AssignmentRuleDialog(const Course::AssignmentRule &rule, QWidget *parent)
    : QDialog(parent), currentRule(rule) {
    
    setWindowTitle("设置分配规则");
    resize(500, 400);
    setModal(true);
    
    setupUI(rule);
}

void AssignmentRuleDialog::setupUI(const Course::AssignmentRule &rule) {
    mainLayout = new QVBoxLayout(this);
    
    formLayout = new QFormLayout();
    
    comboRuleType = new QComboBox(this);
    comboRuleType->addItem("按院系筛选", Course::AssignmentRule::DEPARTMENT);
    comboRuleType->addItem("按班级筛选", Course::AssignmentRule::CLASS);
    comboRuleType->addItem("按性别筛选", Course::AssignmentRule::GENDER);
    comboRuleType->addItem("按年级筛选", Course::AssignmentRule::GRADE);
    comboRuleType->addItem("按专业筛选", Course::AssignmentRule::MAJOR);
    comboRuleType->addItem("按学生筛选", Course::AssignmentRule::SINGLE);
    
    // Set current selection to the first rule type by default
    comboRuleType->setCurrentIndex(0);
    
    formLayout->addRow("规则类型:", comboRuleType);
    
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("搜索...");
    
    targetList = new QListWidget(this);
    targetList->setSelectionMode(QAbstractItemView::MultiSelection);
    
    // Show info about selection
    infoLabel = new QLabel("请选择适用的目标（多选）", this);
    infoLabel->setStyleSheet("color: #64748b; font-size: 12px;");
    
    // Initialize all items for all rule types
    initializeAllItems(rule);
    
    // Show department items by default
    showItemsForRuleType(Course::AssignmentRule::DEPARTMENT);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(searchBox);
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(targetList);
    
    // Buttons
    btnLayout = new QHBoxLayout();
    btnOk = new QPushButton("确定", this);
    btnCancel = new QPushButton("取消", this);
    
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(btnLayout);
    
    // Connect signals
    connect(comboRuleType, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &AssignmentRuleDialog::onRuleTypeChanged);
    connect(searchBox, &QLineEdit::textChanged, 
            this, &AssignmentRuleDialog::onSearchTextChanged);
    connect(btnOk, &QPushButton::clicked, this, &AssignmentRuleDialog::onOkClicked);
    connect(btnCancel, &QPushButton::clicked, this, &AssignmentRuleDialog::onCancelClicked);
    
    // Connect to handle gender selection exclusivity
    connect(targetList, &QListWidget::itemChanged, this, &AssignmentRuleDialog::onItemChanged);
}

void AssignmentRuleDialog::initializeAllItems(const Course::AssignmentRule &rule) {
    // Clear any existing items
    allDeptItems.clear();
    allClassItems.clear();
    allGenderItems.clear();
    allGradeItems.clear();
    allMajorItems.clear();
    allStudentItems.clear();
    
    // Create department items
    for (const auto &departments = aaims::manager::classes::get_departments(); const auto &dept : departments) {
        auto *item = new QListWidgetItem(dept); // NOLINT
        item->setCheckState(Qt::Unchecked);
        // Check if this department is in the original rule
        if (rule.targetDepartments.contains(dept)) {
            item->setCheckState(Qt::Checked);
        }
        item->setData(Qt::UserRole + 1, "department"); // Mark the type
        allDeptItems.append(item);
    }
    
    // Create class items
    const auto &classes = aaims::manager::classes::get_classes();
    for (const auto &[uuid, cls] : classes.asKeyValueRange()) {
        auto *item = new QListWidgetItem(cls->name); // NOLINT
        item->setData(Qt::UserRole, uuid.toString());
        item->setCheckState(Qt::Unchecked);
        // Check if this class is in the original rule
        if (rule.targetClasses.contains(uuid)) {
            item->setCheckState(Qt::Checked);
        }
        item->setData(Qt::UserRole + 1, "class"); // Mark the type
        allClassItems.append(item);
    }
    
    // Create gender items
    auto *maleItem = new QListWidgetItem("男"); // NOLINT
    maleItem->setData(Qt::UserRole, "male");
    maleItem->setCheckState(Qt::Unchecked);
    
    auto *femaleItem = new QListWidgetItem("女"); // NOLINT
    femaleItem->setData(Qt::UserRole, "female");
    femaleItem->setCheckState(Qt::Unchecked);
    
    // Set the initial state based on the original rule
    if (rule.specific_gender()) {
        if (rule.isFemale) {
            femaleItem->setCheckState(Qt::Checked);
            maleItem->setCheckState(Qt::Unchecked);
        } else {
            maleItem->setCheckState(Qt::Checked);
            femaleItem->setCheckState(Qt::Unchecked);
        }
    } else {
        // If no gender rule was set, leave both unchecked by default
        maleItem->setCheckState(Qt::Unchecked);
        femaleItem->setCheckState(Qt::Unchecked);
    }
    maleItem->setData(Qt::UserRole + 1, "gender"); // Mark the type
    femaleItem->setData(Qt::UserRole + 1, "gender"); // Mark the type
    allGenderItems.append(maleItem);
    allGenderItems.append(femaleItem);
    

    
    // Create grade items
    QSet<QString> grades;
    for (const auto &[uuid, cls] : classes.asKeyValueRange()) {
        grades.insert(cls->grade);
    }
    for (const auto &grade : grades) {
        auto *item = new QListWidgetItem(grade); // NOLINT
        item->setCheckState(Qt::Unchecked);
        // Check if this grade is in the original rule
        if (rule.targetGrades.contains(grade)) {
            item->setCheckState(Qt::Checked);
        }
        item->setData(Qt::UserRole + 1, "grade"); // Mark the type
        allGradeItems.append(item);
    }
    
    // Create major items
    const auto &majors = aaims::manager::classes::get_majors();
    for (const auto &[uuid, major] : majors.asKeyValueRange()) {
        auto *item = new QListWidgetItem(major->name); // NOLINT
        item->setData(Qt::UserRole, uuid.toString());
        item->setCheckState(Qt::Unchecked);
        // Check if this major is in the original rule
        if (rule.targetMajors.contains(uuid)) {
            item->setCheckState(Qt::Checked);
        }
        item->setData(Qt::UserRole + 1, "major"); // Mark the type
        allMajorItems.append(item);
    }
    
    // Create student items
    const auto &students = aaims::manager::account::get_working_students();
    for (const auto &[uuid, student] : students.asKeyValueRange()) {
        auto *item = new QListWidgetItem(QString("%1 (%2)").arg(student->name, student->username)); // NOLINT
        item->setData(Qt::UserRole, uuid.toString());
        item->setCheckState(Qt::Unchecked);
        // Check if this student is in the original rule
        if (rule.specificStudents.contains(uuid)) {
            item->setCheckState(Qt::Checked);
        }
        item->setData(Qt::UserRole + 1, "student"); // Mark the type
        allStudentItems.append(item);
    }
}

void AssignmentRuleDialog::showItemsForRuleType(const int ruleType) {
    // Clear the current list
    targetList->clear();
    
    // Add items based on the rule type
    switch (ruleType) {
        case Course::AssignmentRule::DEPARTMENT:
            for (auto *item : allDeptItems) {
                targetList->addItem(item);
            }
            break;
        case Course::AssignmentRule::CLASS:
            for (auto *item : allClassItems) {
                targetList->addItem(item);
            }
            break;
        case Course::AssignmentRule::GENDER:
            for (auto *item : allGenderItems) {
                targetList->addItem(item);
            }
            break;
        case Course::AssignmentRule::GRADE:
            for (auto *item : allGradeItems) {
                targetList->addItem(item);
            }
            break;
        case Course::AssignmentRule::MAJOR:
            for (auto *item : allMajorItems) {
                targetList->addItem(item);
            }
            break;
        case Course::AssignmentRule::SINGLE:
            for (auto *item : allStudentItems) {
                targetList->addItem(item);
            }
            break;
        default:
            break;
    }
    
    // Apply current search filter
    onSearchTextChanged(searchBox->text());
}

void AssignmentRuleDialog::onRuleTypeChanged([[maybe_unused]] int index) {
    const int ruleType = comboRuleType->currentData().toInt();
    showItemsForRuleType(ruleType);
}

void AssignmentRuleDialog::onItemChanged(const QListWidgetItem *item) {
    // Handle gender selection exclusivity
    QString itemType = item->data(Qt::UserRole + 1).toString();
    if (itemType == "gender") {
        // Find the other gender item
        for (auto *genderItem : allGenderItems) {
            if (genderItem != item && genderItem->data(Qt::UserRole + 1).toString() == "gender") {
                // If current item is checked, uncheck the other one
                if (item->checkState() == Qt::Checked) {
                    genderItem->setCheckState(Qt::Unchecked);
                }
                // If current item is unchecked and the other is also unchecked, 
                // allow both to remain unchecked (meaning gender filter is not active)
                break;
            }
        }
    }
}

void AssignmentRuleDialog::onSearchTextChanged(const QString &text) {
    // Filter the target list based on the search text
    for (int i = 0; i < targetList->count(); ++i) {
        QListWidgetItem *item = targetList->item(i);
        bool match = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

void AssignmentRuleDialog::onOkClicked() {
    // Reset the rule status and all target lists
    currentRule.status = 0;
    currentRule.targetDepartments.clear();
    currentRule.targetClasses.clear();
    currentRule.targetGrades.clear();
    currentRule.targetMajors.clear();
    currentRule.specificStudents.clear();
    
    // Process all items across all categories to collect selections
    // Department selections
    bool hasDepartmentSelection = false;
    for (auto *item : allDeptItems) {
        if (item->checkState() == Qt::Checked) {
            hasDepartmentSelection = true;
            currentRule.targetDepartments.append(item->text());
        }
    }
    
    // Class selections
    bool hasClassSelection = false;
    for (auto *item : allClassItems) {
        if (item->checkState() == Qt::Checked) {
            hasClassSelection = true;
            QUuid classUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
            if (classUuid != aaims::model::EMPTY_UUID) {
                currentRule.targetClasses.append(classUuid);
            }
        }
    }
    
    // Gender selections
    bool hasGenderSelection = false;
    bool maleSelected = false;
    bool femaleSelected = false;
    
    for (auto *item : allGenderItems) {
        if (item->checkState() == Qt::Checked) {
            if (item->text() == "男") {
                maleSelected = true;
            } else if (item->text() == "女") {
                femaleSelected = true;
            }
        }
    }
    
    // Only set gender selection if at least one is selected
    if (maleSelected || femaleSelected) {
        hasGenderSelection = true;
        if (femaleSelected) currentRule.isFemale = true;
    }
    
    // Grade selections
    bool hasGradeSelection = false;
    for (const auto *item : allGradeItems) {
        if (item->checkState() == Qt::Checked) {
            hasGradeSelection = true;
            currentRule.targetGrades.append(item->text());
        }
    }
    
    // Major selections
    bool hasMajorSelection = false;
    for (const auto *item : allMajorItems) {
        if (item->checkState() == Qt::Checked) {
            hasMajorSelection = true;
            if (QUuid majorUuid = QUuid::fromString(item->data(Qt::UserRole).toString()); majorUuid != EMPTY_UUID) {
                currentRule.targetMajors.append(majorUuid);
            }
        }
    }
    
    // Student selections
    bool hasStudentSelection = false;
    for (const auto *item : allStudentItems) {
        if (item->checkState() == Qt::Checked) {
            hasStudentSelection = true;
            if (QUuid studentUuid = QUuid::fromString(item->data(Qt::UserRole).toString()); studentUuid != EMPTY_UUID) {
                currentRule.specificStudents.append(studentUuid);
            }
        }
    }

    // Set the status bits for each rule type that has at least one selection
    if (hasDepartmentSelection) {
        currentRule.status |= Course::AssignmentRule::DEPARTMENT;
    }
    if (hasClassSelection) {
        currentRule.status |= Course::AssignmentRule::CLASS;
    }
    if (hasGenderSelection) {
        currentRule.status |= Course::AssignmentRule::GENDER;
    }
    if (hasGradeSelection) {
        currentRule.status |= Course::AssignmentRule::GRADE;
    }
    if (hasMajorSelection) {
        currentRule.status |= Course::AssignmentRule::MAJOR;
    }
    if (hasStudentSelection) {
        currentRule.status |= Course::AssignmentRule::SINGLE;
    }
    
    accept();
}

void AssignmentRuleDialog::onCancelClicked() {
    reject();
}

Course::AssignmentRule AssignmentRuleDialog::getRule() const {
    return currentRule;
}