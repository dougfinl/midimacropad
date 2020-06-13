#include "mainwindow.h"
#include "controlsurfacewidget.h"

#include <QApplication>
#include <QPushButton>
#include <QBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    centralWidget->setLayout(layout);

    ControlSurfaceWidget *surface = new ControlSurfaceWidget(centralWidget, 236, 199);
    layout->addWidget(surface);

    ///////////////////////////////////////////////////////////
    /// Example layout - AKAI Professional APCmini
    ///////////////////////////////////////////////////////////
    // Main button grid
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            float x = 8 + 25 * j;
            float y = 20.5 + 13.5 * i;
            surface->addControl(x, y, 20, 10);
        }
    }

    // Buttons - right column
    for (int i=0; i<8; i++) {
        float y = 20.5 + 13.5 * i;
        surface->addControl(214, y, 9, 9);
    }

    // Buttons - bottom row
    for (int i=0; i<8; i++) {
        float x = 13 + 25 * i;
        surface->addControl(x, 130, 9, 9);
    }

    // Bottom-right (shift) button
    surface->addControl(214.5, 129.5, 10, 10);

    // Faders
    for (int i=0; i<9; i++) {
        float x = 9 + 25 * i;
        surface->addControl(x, 145, 19, 50);
    }

    w.setCentralWidget(centralWidget);
    w.show();

    return a.exec();
}
