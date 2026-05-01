// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_ASSIGNMENTROLESLot_H
#define AAIMS_ASSIGNMENTROLESLot_H

#pragma once
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QCheckBox>

#include "../../utils/DataStructures.h"

using namespace aaims::model;

class AssignmentRoleSlot : public QWidget {
    Q_OBJECT

public:
    explicit AssignmentRoleSlot(const Course::AssignmentRule &rule, QWidget *parent = nullptr);

    [[nodiscard]] Course::AssignmentRule toData() const;

signals:
    void removeRequested();

private:
    QHBoxLayout *layout;
    QComboBox *comboRuleType;
    QListWidget *targetList;
    QPushButton *btnRemove;

    QVector<QString> targetDepartments;
    QVector<QUuid> targetClasses;
    QVector<QString> targetGrades;
    QVector<QUuid> targetMajors;
    bool isFemale;
    QVector<QUuid> specificStudents;
};

#endif //AAIMS_ASSIGNMENTROLESLot_H
