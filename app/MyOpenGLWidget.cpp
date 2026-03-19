#include "MyOpenGLWidget.h"

#include <QtGui/QMouseEvent>

#ifdef __linux
#include <GL/glu.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#endif

#include <sstream>
#include <iomanip>
#include <chrono>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    mouse_clicked(false),
    radius(5),
    zoom(1.0),
    trigger_node_x(0),
    trigger_node_y(0),
    timeOfLastRender(0.0),
    framesPerSecond(0),
    lastFramesPerSecond(0)
{
    setMouseTracking(true);
}

MyOpenGLWidget::~MyOpenGLWidget()
{
}

void MyOpenGLWidget::setRadius(float new_radius)
{
    radius = new_radius;
}

void MyOpenGLWidget::setZoom(float z)
{
    zoom = z;
}

void MyOpenGLWidget::initializeGL()
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
}

void MyOpenGLWidget::resizeWorld(int w, int h)
{
    resizeGL(w,h);
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    world_height = h / zoom;
    world_width = w / zoom;
}

bool MyOpenGLWidget::doNodesIntersect( float cx0, float cy0, float radius0, float cx1, float cy1, float radius1)
{
    float dx = cx0 - cx1;
    float dy = cy0 - cy1;
    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist > radius0 + radius1)
    {
        return false;
    }
    else if (dist < std::fabs(radius0 - radius1))
    {
        return false;
    }
    else if ((dist == 0) && (radius0 == radius1))
    {
        return true;
    }
    else
    {
        return true;
    }
}

void MyOpenGLWidget::draw_node(float r, float g, float b, float x, float y, float y_vel,float x_vel, float radius)
{
    /*glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x,(world_height - y));

    for(float angle = 1.0f; angle < 361.0f; angle+= 5.0f)
    {
        float next_x = x + sin(angle) * radius;
        float next_y = (world_height - y) + cos(angle) * radius;
        glVertex2f(next_x,next_y);
    }

    glEnd();*/

    glPushMatrix();
    glTranslatef(x,(world_height - y),0);

    float direction = (std::atan2(x_vel, y_vel) * 57.2958) +180;
    glRotatef(direction,0,0,1);

    glColor3f(0,0,0);
    float border_radius = radius + 2;
    glBegin(GL_TRIANGLES);
    glVertex2i(0, border_radius);
    glVertex2i(-border_radius / 2, -border_radius / 2);
    glVertex2i(border_radius / 2, -border_radius / 2);

    glColor3f(r,g,b);
    glBegin(GL_TRIANGLES);
    glVertex2i(0, radius);
    glVertex2i(-radius / 2, -radius / 2);
    glVertex2i(radius / 2, -radius / 2);

    glEnd();
    glPopMatrix();
}

void MyOpenGLWidget::calculateFrameRate()
{
    auto timeNow = std::chrono::system_clock::now();
    double currentTime = std::chrono::duration_cast<std::chrono::seconds>(timeNow.time_since_epoch()).count();
    currentTime += ((currentTime * 1000.0) - std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch()).count()) / 1000.0;

    ++framesPerSecond;
    if(currentTime - timeOfLastRender > 1.0)
    {
        lastFramesPerSecond = (framesPerSecond / 1.0);
        framesPerSecond = 0;
        timeOfLastRender = currentTime;
    }
}

void MyOpenGLWidget::paintGL()
{
    glClearColor(0.44f, 0.57f, 0.75f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, this->width(), this->height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, world_width, 0, world_height); // set origin to bottom left corner

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (std::vector<boid_simple>::iterator it = boids.begin() ; it != boids.end(); ++it)
    {
        if(doNodesIntersect(it->x_pos, it->y_pos, radius, trigger_node_x, trigger_node_y, radius))
        {
            draw_node(0.0,0.0,1.0f, it->x_pos, it->y_pos, it->y_vel, it->x_vel, radius);
        }
        else
        {
            draw_node(0.86f,0.43f,0.0,it->x_pos, it->y_pos, it->y_vel, it->x_vel, radius);
        }
    }

    if(mouse_clicked)
    {
        mouse_clicked = false;
    }

    //Want this at the end so we calculate properly
    calculateFrameRate();

    std::stringstream output_string;
    output_string << "Frames Per Second: " << std::setprecision(15) << lastFramesPerSecond;
    writeText(QString::fromStdString(output_string.str()), 15, 20);

    output_string.str( std::string() );
    output_string.clear();
    output_string << "Mouse Position: (" << std::fixed << std::setprecision(0) << trigger_node_x << "," << trigger_node_y << ")";
    writeText(QString::fromStdString(output_string.str()), 15, 40);

    output_string.str( std::string() );
    output_string.clear();
    output_string << "Zoom: " << std::fixed << std::setprecision(0) << (zoom * 100.0f) << "%";
    writeText(QString::fromStdString(output_string.str()), 15, 60);

    output_string.str( std::string() );
    output_string.clear();
    output_string << "World Size: " << std::fixed << std::setprecision(0) << world_width << " x " << world_height;
    writeText(QString::fromStdString(output_string.str()), 15, 80);

}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        mouse_clicked = true;
    }
    else
    {
        mouse_clicked = false;
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    trigger_node_x = event->x() / zoom;
    trigger_node_y = event->y() / zoom;
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        event->ignore();
        break;
    default:
        event->ignore();
        break;
    }
}

void MyOpenGLWidget::writeText(QString text,float x,float y)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8));
    painter.drawText(x, y, 250, 20, Qt::AlignLeft, text );
    painter.end();
}

void MyOpenGLWidget::update_boid_positions(vector<boid_simple> new_boid_states)
{
    boids = new_boid_states;
}
