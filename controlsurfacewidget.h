#ifndef CONTROLSURFACEWIDGET_H
#define CONTROLSURFACEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QSvgRenderer>

struct ControlSurfaceWidgetItem;

class ControlSurfaceWidget : public QWidget
{
    Q_OBJECT
public:
    static ControlSurfaceWidget *fromJSONFile(const QString &file);
    static ControlSurfaceWidget *fromJSONData(const QByteArray &json, const QString &workingDir = "");

    explicit ControlSurfaceWidget(QWidget *parent = nullptr, float refWidth = 100.f, float refHeight = 100.f);
    ~ControlSurfaceWidget();

    void setBackgroundSvg(const QString &file);
    void addControl(float refX, float refY, float refWidth, float refHeight);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QVector<ControlSurfaceWidgetItem> m_controls;
    QSizeF m_refSize;
    float m_refAspect;
    QSvgRenderer *m_bgRenderer;
    QRectF m_bgRect;
};

#endif // CONTROLSURFACEWIDGET_H
