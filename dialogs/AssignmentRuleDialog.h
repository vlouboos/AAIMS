// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ASSIGNMENTRULEDIALOG_H
#define AAIMS_ASSIGNMENTRULEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

#include "../utils/DataStructures.h"

using namespace aaims::model;

class AssignmentRuleDialog : public QDialog {
    Q_OBJECT

public:
    explicit AssignmentRuleDialog(const Course::AssignmentRule &rule, QWidget *parent = nullptr);

    Course::AssignmentRule getRule() const;

private slots:
    void onRuleTypeChanged(int index);

    void onSearchTextChanged(const QString &text);

    void onItemChanged(const QListWidgetItem *item);

    void onOkClicked();

    void onCancelClicked();

private:
    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QComboBox *comboRuleType;
    QLineEdit *searchBox;
    QListWidget *targetList;
    QLabel *infoLabel;
    QHBoxLayout *btnLayout;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    
    Course::AssignmentRule currentRule;
    
    // Store all items for all rule types
    QList<QListWidgetItem*> allDeptItems;
    QList<QListWidgetItem*> allClassItems;
    QList<QListWidgetItem*> allGenderItems;
    QList<QListWidgetItem*> allGradeItems;
    QList<QListWidgetItem*> allMajorItems;
    QList<QListWidgetItem*> allStudentItems;

    void setupUI(const Course::AssignmentRule &rule);
    void initializeAllItems(const Course::AssignmentRule &rule);
    void showItemsForRuleType(int ruleType);
};

#endif //AAIMS_ASSIGNMENTRULEDIALOG_H