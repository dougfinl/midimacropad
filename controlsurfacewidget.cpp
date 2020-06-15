#include "controlsurfacewidget.h"

#include <QSvgWidget>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>
#include <QPainter>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>

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

ControlSurfaceWidget *ControlSurfaceWidget::fromJSONFile(const QString &file)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(file);
    if (type.name() != "application/json") {
        qDebug() << "[ERROR] file does not contain JSON data";
        return nullptr;
    }

    QFile jsonFile(file);
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "[ERROR] failed to open " << file;
        return nullptr;
    }
    QByteArray fileData = jsonFile.readAll();
    jsonFile.close();

    QString workingDir = QFileInfo(jsonFile).dir().absolutePath();

    return ControlSurfaceWidget::fromJSONData(fileData, workingDir);
}

ControlSurfaceWidget *ControlSurfaceWidget::fromJSONData(const QByteArray &data, const QString &workingDir)
{
    ControlSurfaceWidget *surface = nullptr;

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &err);
    if (jsonDoc.isNull()) {
        qDebug() << "[ERROR] failed to parse JSON data";
        return nullptr;
    }

    QJsonObject root = jsonDoc.object();

    QString manufacturer = root["manufacturer"].toString();
    QString model = root["model"].toString();

    qDebug().noquote() << "[INFO] loading controller:" << manufacturer << "-" << model;

    float surfaceWidth = root["width"].toVariant().toFloat();
    float surfaceHeight = root["height"].toVariant().toFloat();
    surface = new ControlSurfaceWidget(nullptr, surfaceWidth, surfaceHeight);

    QString backgroundFileName = root["background"].toString();
    QString backgroundFilePath = QDir(workingDir).absoluteFilePath(backgroundFileName);
    surface->setBackgroundSvg(backgroundFilePath);

    QJsonArray controlsList = root["controls"].toArray();
    for (const auto &controlJSON : controlsList) {
        QJsonObject obj = controlJSON.toObject();

        QString id = obj["id"].toString();
        QString type = obj["type"].toString();

        if (type == "push_button") {
            QJsonObject geometry = obj["geometry"].toObject();
            float x      = geometry["x"].toVariant().toFloat();
            float y      = geometry["y"].toVariant().toFloat();
            float width  = geometry["width"].toVariant().toFloat();
            float height = geometry["height"].toVariant().toFloat();
            surface->addControl(x, y, width, height);
        }
    }

    return surface;
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
