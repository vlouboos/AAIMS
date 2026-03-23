// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#ifndef AAIMS_DISTRIBUTIONBAR_H
#define AAIMS_DISTRIBUTIONBAR_H

#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>

struct DistributionItem {
    QString label;
    long long count;
    QColor color;
};

class DistributionRing : public QWidget {
    Q_OBJECT
public:
    explicit DistributionRing(QWidget *parent = nullptr);

    void setData(const QList<DistributionItem> &i);

protected:
    void paintEvent(QPaintEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void drawCenterText(QPainter &painter, const QRectF &rect) const;

private:
    struct AngleRange {
        int startAngle;
        int spanAngle;
        DistributionItem item;
    };

    QList<DistributionItem> items;
    int totalCount = 0;
    int ringWidth = 20;
    int hoveredIndex = -1;
    QList<AngleRange> angleRanges;
};


#endif //AAIMS_DISTRIBUTIONBAR_H