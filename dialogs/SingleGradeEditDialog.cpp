// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include "SingleGradeEditDialog.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QPushButton>
#include <qtconcurrentrun.h>

#include "../managements/AccountManager.h"
#include "../managements/RatingManager.h"

SingleGradeEditDialog::SingleGradeEditDialog(aaims::model::StudentRating::RatingDetail &rating, QWidget *parent)
    : StyledDialog(parent), rating(rating) {
    setWindowTitle("编辑成绩");
    resize(300, 200);
    
    setupUI();
    applyStyles();
}

void SingleGradeEditDialog::setupUI() {
    // Main layout
    mainLayout = new QVBoxLayout(this);
    
    // Form layout for grade inputs
    formLayout = new QFormLayout();
    
    performanceSpin = new QDoubleSpinBox();
    performanceSpin->setRange(0.0, 100.0);
    performanceSpin->setSingleStep(0.1);
    performanceSpin->setValue(rating.performance);
    formLayout->addRow("平时分:", performanceSpin);
    
    scoreSpin = new QDoubleSpinBox();
    scoreSpin->setRange(0.0, 100.0);
    scoreSpin->setSingleStep(0.1);
    scoreSpin->setValue(rating.score);
    formLayout->addRow("期末分:", scoreSpin);
    
    finalScoreSpin = new QDoubleSpinBox();
    finalScoreSpin->setRange(0.0, 100.0);
    finalScoreSpin->setSingleStep(0.1);
    finalScoreSpin->setValue(rating.finalScore);
    formLayout->addRow("总分:", finalScoreSpin);
    
    mainLayout->addLayout(formLayout);
    
    // Button layout
    btnLayout = new QHBoxLayout();
    
    btnOk = new QPushButton("确定");
    btnOk->setObjectName("AddElement");
    connect(btnOk, &QPushButton::clicked, this, &SingleGradeEditDialog::onOkClicked);
    
    btnCancel = new QPushButton("取消");
    btnCancel->setObjectName("AddElement");
    connect(btnCancel, &QPushButton::clicked, this, &SingleGradeEditDialog::onCancelClicked);
    
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(btnLayout);
}

void SingleGradeEditDialog::onOkClicked() {
    auto *pd = new QProgressDialog("正在保存...", nullptr, 0, 0, this); // NOLINT
    pd->setWindowModality(Qt::WindowModal);
    pd->show();
    const auto &future = QtConcurrent::run([this] {
        rating.performance = performanceSpin->value();
        rating.score = scoreSpin->value();
        rating.finalScore = finalScoreSpin->value();
        return aaims::manager::rating::save();
    });
    const auto watcher = new QFutureWatcher<bool>(this); // NOLINT
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, pd, watcher] {
        pd->close();
        pd->deleteLater();
        watcher->deleteLater();
        accept();
    });
    watcher->setFuture(future);
}

void SingleGradeEditDialog::onCancelClicked() {
    reject();
}

aaims::model::StudentRating::RatingDetail SingleGradeEditDialog::getUpdatedRating() const {
    aaims::model::StudentRating::RatingDetail updatedRating = rating;
    updatedRating.performance = performanceSpin->value();
    updatedRating.score = scoreSpin->value();
    updatedRating.finalScore = finalScoreSpin->value();
    return updatedRating;
}
