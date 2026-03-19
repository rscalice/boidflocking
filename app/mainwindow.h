#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

#include "boid.h"
#include "boidsim.h"
#include "MyOpenGLWidget.h"
#include "platforminfodialog.h"
#include "openclwrapper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void openPlatformWindow();
    void toggleVSync(bool toggle);
    void update_sim();
    void startSim();    
    void adjustZoomOfWorld(int);
    void toggleToCPUEngine(bool selected);
    void toggleToOpenCLEngine(bool selected);
    void updateSimulation();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    vector<boid_simple> boids;
    void resetSim();

    Ui::MainWindow *ui;
    PlatformInfoDialog platform_dialog;

    OpenCLWrapper open_cl_engine;

    MyOpenGLWidget *glwidget;
    QTimer timer;
    float zoom;
    BoidSim sim;
    bool use_opencl;
};

#endif // MAINWINDOW_H
