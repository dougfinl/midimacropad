#ifndef CONTROLSURFACEWIDGET_H
#define CONTROLSURFACEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPushButton>

struct ControlSurfaceWidgetItem;

class ControlSurfaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControlSurfaceWidget(QWidget *parent = nullptr, int refWidth = 100, int refHeight = 100);
    ~ControlSurfaceWidget();

    void addControl(int refX, int refY, int refWidth, int refHeight);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QVector<ControlSurfaceWidgetItem> m_controls;
    QSize m_refSize;
    float m_refAspect;
};

#endif // CONTROLSURFACEWIDGET_H
