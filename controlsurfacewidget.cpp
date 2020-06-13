#include "controlsurfacewidget.h"

#include <QDebug>

struct ControlSurfaceWidgetItem {
    QRect refGeometry;
    QPushButton *control;
};

ControlSurfaceWidget::ControlSurfaceWidget(QWidget *parent, int refWidth, int refHeight)
    : QWidget(parent)
{
    m_refSize = QSize(refWidth, refHeight);
    m_refAspect = (float) refHeight / refWidth;
}

ControlSurfaceWidget::~ControlSurfaceWidget()
{
}

void ControlSurfaceWidget::addControl(int refX, int refY, int refWidth, int refHeight)
{
    ControlSurfaceWidgetItem item = {
        .refGeometry = QRect(refX, refY, refWidth, refHeight),
        .control = new QPushButton(this)
    };

    m_controls.append(item);
}

void ControlSurfaceWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_controls.empty())
        return;

    const QRect rect = contentsRect();
    const float newAspect = (float) rect.height() / rect.width();

    // Offsets to be applied to every child widget
    int xOffset = 0;
    int yOffset = 0;

    float scaleFactor;
    if (newAspect < m_refAspect) {
        // Pillarboxing - height dictates the overall scale
        scaleFactor = (float) rect.height() / m_refSize.height();
        xOffset = (rect.width() - (m_refSize.width() * scaleFactor)) / 2;
    } else if (newAspect > m_refAspect) {
        // Letterboxing - width dictates the overall scale
        scaleFactor = (float) rect.width() / m_refSize.width();
        yOffset = (rect.height() - (m_refSize.height() * scaleFactor)) / 2;
    } else {
        // No change in aspect ratio
        scaleFactor = (float) rect.height() / m_refSize.height();
    }

    // Apply the new geometry
    for (auto &item: qAsConst(m_controls)) {
        QRect newGeom;
        const QRect g = item.refGeometry;

        int x = g.x() * scaleFactor + xOffset;
        int y = g.y() * scaleFactor + yOffset;
        newGeom.setX(x);
        newGeom.setY(y);

        int width = qRound((float) g.width() * scaleFactor);
        int height = qRound((float) g.height() * scaleFactor);
        newGeom.setWidth(width);
        newGeom.setHeight(height);

        item.control->setGeometry(newGeom);
    }
}
