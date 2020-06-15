#include "mainwindow.h"
#include "controlsurfacewidget.h"

#include <QApplication>
#include <QPushButton>
#include <QBoxLayout>
#include <QStatusBar>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.statusBar()->hide();

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    centralWidget->setLayout(layout);

    ControlSurfaceWidget *surface = ControlSurfaceWidget::fromJSONFile("assets/akai_apcmini.json");
    if (surface)
        layout->addWidget(surface);

    w.setCentralWidget(centralWidget);
    w.show();

    return a.exec();
}
