// This file is part of AAIMS.
// You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
// See https://spdx.org/licenses/BSD-3-Clause.html

#include <QPainterPath>
#include "DistributionRing.h"

DistributionRing::DistributionRing(QWidget *parent) : QWidget(parent) {
    setMinimumSize(220, 220);
    setMouseTracking(true);
}

void DistributionRing::setData(const <DistributionItem> &i) {
    items = i;
    totalCount = 0;
    for (const auto &item: i) {
        totalCount += item.count;
    }
    update();
}

void DistributionRing::paintEvent([[maybe_unused]] QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF totalRect = rect();
    const qreal side = qMin(totalRect.width(), totalRect.height());
    QRectF pieRect((totalRect.width() - side) / 2, (totalRect.height() - side) / 2, side, side);
    pieRect.adjust(ringWidth / 2.0, ringWidth / 2.0, -ringWidth / 2.0, -ringWidth / 2.0);

    if (totalCount <= 0 || items.empty()) {
        const QPen emptyPen(QColor(0xf1f5f9), ringWidth, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(emptyPen);
        painter.drawArc(pieRect, 0, 360 * 16);
        drawCenterText(painter, pieRect);
        return;
    }

    angleRanges.clear();
    int currentAngle = 90 * 16;
    for (int i = 0; i < items.size(); ++i) {
        const auto &item = items[i];
        if (item.count <= 0) continue;
        const int spanAngle = -((item.count * 360 * 16) / totalCount);

        QColor barColor = item.color;
        if (hoveredIndex == i) barColor = barColor.darker(110);

        QPen barPen(barColor, ringWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(barPen);
        painter.drawArc(pieRect, currentAngle, spanAngle);

        angleRanges.push_back({currentAngle, spanAngle, item});
        currentAngle += spanAngle;
    }
    drawCenterText(painter, pieRect);
}

void DistributionRing::drawCenterText(QPainter &painter, const QRectF &rect) const {
    painter.setPen(QColor(0x0f172a));

    const QFont valueFont("Inter", 32, QFont::Bold);
    painter.setFont(valueFont);
    QRectF valueRect = rect;
    valueRect.adjust(0, -30, 0, 0);
    painter.drawText(valueRect, Qt::AlignCenter, QString::number(totalCount));

    painter.setPen(QColor(0x64748b));
    const QFont labelFont("Inter", 12, QFont::Medium);
    painter.setFont(labelFont);
    QRectF labelRect = rect;
    labelRect.adjust(0, 30, 0, 0);
    painter.drawText(labelRect, Qt::AlignCenter, "个人账号数");
}

void DistributionRing::mouseMoveEvent(QMouseEvent *event) {
    if (totalCount <= 0 || angleRanges.empty()) return;
    const QPointF center = rect().center();
    const QPointF mousePos = event->position();
    const qreal dist = QLineF(center, mousePos).length();

    const qreal side = qMin(rect().width(), rect().height());
    const qreal outerRadius = side / 2.0;

    if (const qreal innerRadius = outerRadius - ringWidth; dist < innerRadius || dist > outerRadius) {
        if (hoveredIndex != -1) {
            hoveredIndex = -1;
            QToolTip::hideText();
            update();
        }
        return;
    }

    const qreal dx = mousePos.x() - center.x();
    const qreal dy = mousePos.y() - center.y();

    const qreal angleRad = qAtan2(-dy, dx);
    qreal angleDeg = qRadiansToDegrees(angleRad);

    if (angleDeg < 0) angleDeg += 360.0;

    int newHoveredIndex = -1;
    for (int i = 0; i < angleRanges.size(); ++i) {
        const auto &range = angleRanges[i];

        const double startDeg = range.startAngle / 16.0;
        const double spanDeg = range.spanAngle / 16.0;
        const double endDeg = startDeg + spanDeg;

        double ccwStart = std::fmod(endDeg, 360.0);
        if (ccwStart < 0) ccwStart += 360.0;

        double ccwEnd = std::fmod(startDeg, 360.0);
        if (ccwEnd < 0) ccwEnd += 360.0;

        bool inRange = false;
        if (ccwStart <= ccwEnd) {
            inRange = angleDeg >= ccwStart && angleDeg <= ccwEnd;
        } else {
            inRange = angleDeg >= ccwStart || angleDeg <= ccwEnd;
        }

        if (inRange) {
            newHoveredIndex = i;
            break;
        }
    }

    if (newHoveredIndex != hoveredIndex) {
        hoveredIndex = newHoveredIndex;
        update();

        if (hoveredIndex != -1) {
            const auto &[label, count, color] = angleRanges[hoveredIndex].item;
            const double percent = static_cast<double>(count) / totalCount * 100.0;

            // Use CSS to make it 非常地 beautiful
            const QString tooltipStr = QString(
                        "<div style='margin: 4px;'>"
                        "<b style='font-size: 14px; color: %1;'>%2</b><br/>"
                        "<span style='color: #cbd5e1;'>人数:</span> <b style='color: white;'>%3</b><br/>"
                        "<span style='color: #cbd5e1;'>占比:</span> <b style='color: white;'>%4%</b>"
                        "</div>"
                    ).arg(color.name())
                    .arg(label)
                    .arg(count)
                    .arg(QString::number(percent, 'f', 1));

            QToolTip::showText(event->globalPosition().toPoint(), tooltipStr, this);
        } else {
            QToolTip::hideText();
        }
    }
}
