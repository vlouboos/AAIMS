// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_CLASSADDCOURSEDIALOG_H
#define AAIMS_CLASSADDCOURSEDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class ClassAddCourseDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit ClassAddCourseDialog(const QList<QUuid> &currentCourses, QWidget *parent = nullptr);

    [[nodiscard]] QList<QUuid> getAddedCourses() const;

private slots:
    void filterCourses();

    void onAddButtonClicked();

    void onDeleteButtonClicked(const QUuid &uuid);

private:
    QVBoxLayout *mainLayout;
    QLineEdit *searchEdit;
    QListWidget *availableList;
    QPushButton *btnAdd;
    QScrollArea *addedArea;
    QWidget *addedWidget;
    QVBoxLayout *addedLayout;
    QHBoxLayout *btnLayout;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QList<QUuid> addedCourses;
    QList<QUuid> currentCourses;
    QMap<QUuid, QHBoxLayout *> addedLayouts;
    QHash<QUuid, std::shared_ptr<aaims::model::Course> > allCourses;

    void populateAvailable();

    void addToAddedList(const QUuid &uuid);
};

#endif // AAIMS_CLASSADDCOURSEDIALOG_H
