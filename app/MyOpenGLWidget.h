#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QtWidgets>
#include <QtOpenGLWidgets/QtOpenGLWidgets>
#include <vector>
#include "boid.h"

using namespace std;

class MyOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget(QWidget *parent = 0);
    ~MyOpenGLWidget();
    void update_boid_positions(vector<boid_simple> new_boid_states);
    void setRadius(float new_radius);
    void setZoom(float zoom);
    void resizeWorld(int w, int h);
public slots:
    void paintGL();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void writeText(QString text,float x,float y);

private:
    void calculateFrameRate();
    void draw_node(float r, float g, float b, float x, float y, float y_vel,float x_vel, float radius);
    bool doNodesIntersect( float cx0, float cy0, float radius0, float cx1, float cy1, float radius1);

    bool mouse_clicked;
    float trigger_node_x, trigger_node_y;
    float radius;
    vector<boid_simple> boids;
    double timeOfLastRender;
    int framesPerSecond;
    double lastFramesPerSecond;
    float zoom;
    int world_height, world_width;
};

#endif // MYOPENGLWIDGET_H
