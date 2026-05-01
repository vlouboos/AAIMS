// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_STUDENTGRADEPAGE_H
#define AAIMS_STUDENTGRADEPAGE_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QWidget>

class StudentGradePage : public QWidget {
    Q_OBJECT

public:
    explicit StudentGradePage(QWidget *parent = nullptr);

private slots:
    void onQueryClicked();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QVBoxLayout *titleContainer;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QComboBox *semesterCombo;
    QPushButton *queryButton;
    QTableWidget *gradesTable;
    
    void setupUI();
    void loadSemesters();
};

#endif //AAIMS_STUDENTGRADEPAGE_H