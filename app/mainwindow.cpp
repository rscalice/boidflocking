#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <random>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    use_opencl(false),
    ui(new Ui::MainWindow),
    platform_dialog(this)
{
    ui->setupUi(this);
    glwidget = new MyOpenGLWidget(this);
    glwidget->move(ui->groupBox->width() + 20,ui->menuBar->height());

    connect(&timer, SIGNAL(timeout()),this, SLOT(update_sim()));
    connect(ui->actionSee_Platform_Info, SIGNAL(triggered(bool)), this, SLOT(openPlatformWindow()));
    connect(ui->actionVertical_Sync, SIGNAL(triggered(bool)), this, SLOT(toggleVSync(bool)));
    connect(ui->startSimBtn, SIGNAL(clicked()), this, SLOT(startSim()));
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(adjustZoomOfWorld(int)));

    connect(ui->cpuEngine, SIGNAL(clicked(bool)), this, SLOT(toggleToCPUEngine(bool)));
    connect(ui->openCLEngine, SIGNAL(clicked(bool)), this, SLOT(toggleToOpenCLEngine(bool)));

    connect(ui->updateSimulationBtn, SIGNAL(clicked()), this, SLOT(updateSimulation()));

    ui->zoomSlider->setValue(100);
    updateSimulation();
    timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openPlatformWindow()
{
    platform_dialog.setPlatformInfo(QString::fromStdString(open_cl_engine.getPlatformInfo()));
    platform_dialog.show();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    int total_width = ui->centralWidget->width() - ui->groupBox->width() + 20;
    int total_height = ui->centralWidget->height();

    glwidget->resize(total_width, total_height);

    glwidget->resizeWorld(glwidget->width(), glwidget->height());
    updateSimulation();
}

void MainWindow::update_sim()
{
    // Pass new boid positions to renderer
    glwidget->update_boid_positions(boids);

    // Render - draw boids
    glwidget->update();

    if(sim.is_initialized)
    {
        if(use_opencl)
        {

            bool valid = false;
            int platform = ui->platformText->text().toInt(&valid);

            if(!valid)
            {
                platform = 0;
            }

            int device = ui->deviceText->text().toInt(&valid);

            if(!valid)
            {
                device = 0;
            }

            // Update boid states gpu
            open_cl_engine.advance_boid_states(boids, platform, device);
        }
        else
        {
            // Update boid states cpu
            sim.advance_boid_states_cpu(boids);
        }
    }
}

void MainWindow::toggleVSync(bool toggle)
{
    //QOpenGL fmt;

    //if(!toggle)
   // {
   //     fmt.setSwapInterval( 0 );
   // }

    //glwidget->hide();
   // glwidget->setFormat(fmt);
   // glwidget->show();
}

void MainWindow::startSim()
{
    sim.resetWorld(boids);

    glwidget->setRadius(ui->boidRadius->value());
    sim.initialize(boids, ui->totalBoids->value(), glwidget->width() / zoom, glwidget->height() / zoom,
                   ui->boidRadius->value());

    open_cl_engine.updateSimulationParamters(ui->distanceText->value(), ui->velocityText->value(), ui->centerMassText->value(),
                                             ui->windXText->value(), ui->windYText->value(), ui->velocityLimitText->value(),
                                             glwidget->width() / zoom, glwidget->width() / zoom);

}

void MainWindow::adjustZoomOfWorld(int value)
{
    zoom = value / 100.0;
    glwidget->setZoom(zoom);
    glwidget->resizeWorld(glwidget->width(), glwidget->height());
    updateSimulation();
}

void MainWindow::toggleToCPUEngine(bool selected)
{
    if(selected)
    {
        ui->groupBox_5->setEnabled(false);
        use_opencl = false;
    }
}

void MainWindow::toggleToOpenCLEngine(bool selected)
{
    if(selected)
    {
        ui->groupBox_5->setEnabled(true);
        use_opencl = true;
    }
}

void MainWindow::updateSimulation()
{

    sim.updateSimulationParamters(ui->distanceText->value(), ui->velocityText->value(), ui->centerMassText->value(),
                                  ui->windXText->value(), ui->windYText->value(), ui->velocityLimitText->value(),
                                  glwidget->width() / zoom, glwidget->width() / zoom);

    open_cl_engine.updateSimulationParamters(ui->distanceText->value(), ui->velocityText->value(), ui->centerMassText->value(),
                                             ui->windXText->value(), ui->windYText->value(), ui->velocityLimitText->value(),
                                             glwidget->width() / zoom, glwidget->width() / zoom);

}
