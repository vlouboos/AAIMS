// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_FILTERPROXYMODEL_H
#define AAIMS_FILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FilterProxyModel : public QSortFilterProxyModel {
public:
    explicit FilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {
    }

protected:
    [[nodiscard]] bool filterAcceptsColumn(const int source_column, const QModelIndex &source_parent) const override {
        const QString filterText = filterRegularExpression().pattern();
        if (filterText.isEmpty()) return true;

        return std::ranges::any_of(std::vector{0, 1}, [this, source_column, source_parent, filterText](const int &column) {
            const QModelIndex index = sourceModel()->index(source_column, column, source_parent);
            if (const QString data = sourceModel()->data(index).toString(); data.contains(filterText, Qt::CaseInsensitive)) {
                return true;
            }
            return false;
        });
    }
};
#endif //AAIMS_FILTERPROXYMODEL_H
