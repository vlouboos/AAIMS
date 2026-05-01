// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_SINGLEGRADEEDITDIALOG_H
#define AAIMS_SINGLEGRADEEDITDIALOG_H

#include <QFormLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "StyledDialog.h"
#include "../utils/DataStructures.h"

class SingleGradeEditDialog : public StyledDialog {
    Q_OBJECT

public:
    explicit SingleGradeEditDialog(aaims::model::StudentRating::RatingDetail &rating, QWidget *parent = nullptr);

    aaims::model::StudentRating::RatingDetail getUpdatedRating() const;

private slots:
    void onOkClicked();

    void onCancelClicked();

private:
    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QDoubleSpinBox *performanceSpin;
    QDoubleSpinBox *scoreSpin;
    QDoubleSpinBox *finalScoreSpin;

    QHBoxLayout *btnLayout;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    aaims::model::StudentRating::RatingDetail &rating;
    void setupUI();
};

#endif //AAIMS_SINGLEGRADEEDITDIALOG_H
