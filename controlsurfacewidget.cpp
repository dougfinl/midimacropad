#include "controlsurfacewidget.h"

#include <QSvgWidget>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>
#include <QPainter>

struct ControlSurfaceWidgetItem {
    QRectF refGeometry;
    QPushButton *control;
};

ControlSurfaceWidget::ControlSurfaceWidget(QWidget *parent, float refWidth, float refHeight)
    : QWidget(parent)
    , m_bgRenderer(nullptr)
{
    m_refSize = QSize(refWidth, refHeight);
    m_refAspect = (float) refHeight / refWidth;
}

ControlSurfaceWidget::~ControlSurfaceWidget()
{
    delete m_bgRenderer;
}

void ControlSurfaceWidget::setBackgroundSvg(const QString &file)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(file);
    if (type.name() != "image/svg+xml") {
        qDebug("[ERROR] file is not an SVG");
        return;
    }

    if (m_bgRenderer)
        delete m_bgRenderer;

    // Read the SVG from file
    QFile svgFile(file);
    if (!svgFile.open(QIODevice::ReadOnly)) {
        qDebug("[ERROR] could not read SVG file");
        return;
    }
    m_bgRenderer = new QSvgRenderer(svgFile.readAll(), this);
    svgFile.close();
    m_bgRenderer->setAspectRatioMode(Qt::AspectRatioMode::IgnoreAspectRatio);
}

void ControlSurfaceWidget::addControl(float refX, float refY, float refWidth, float refHeight)
{
    ControlSurfaceWidgetItem item = {
        .refGeometry = QRectF(refX, refY, refWidth, refHeight),
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
    float xOffset = 0;
    float yOffset = 0;

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
        const QRectF g = item.refGeometry;

        int x = qRound(g.x() * scaleFactor + xOffset);
        int y = qRound(g.y() * scaleFactor + yOffset);
        newGeom.setX(x);
        newGeom.setY(y);

        int width = qRound(g.width() * scaleFactor);
        int height = qRound(g.height() * scaleFactor);
        newGeom.setWidth(width);
        newGeom.setHeight(height);

        item.control->setGeometry(newGeom);
    }

    // Set the background's bounds
    m_bgRect = QRectF(xOffset, yOffset, m_refSize.width() * scaleFactor, m_refSize.height() * scaleFactor);
}

void ControlSurfaceWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_bgRenderer) {
        QPainter painter(this);
        m_bgRenderer->render(&painter, m_bgRect);
    }
}
