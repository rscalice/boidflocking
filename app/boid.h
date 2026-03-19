#ifndef BOID_H
#define BOID_H

#include <math.h>
#include <vector>
#include <CL/opencl.hpp>
using namespace std;

#define MAX_NUM_BOIDS 50000

struct boid_simple {
    cl_float x_pos;
    cl_float y_pos;
    cl_float x_vel;
    cl_float y_vel;
    cl_int id;
    cl_int radius;
};

class state
{
public:
    state(float x = 0.0, float y = 0.0) :
        x(x),
        y(y)
    {
    }

    float x,y;

    state & operator *(const state& rhs)
    {
        x = x * rhs.x;
        y = y * rhs.y;
        return *this;
    }

    state & operator +(const state& rhs)
    {
        x = x + rhs.x;
        y = y + rhs.y;
        return *this;
    }

    state & operator =(const state& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }


    state & operator /(const int& rhs)
    {
        x = x/rhs;
        y = y/rhs;
        return *this;
    }

    state & operator -(const state& rhs)
    {
        x = x - rhs.x;
        y = y - rhs.y;
        return *this;
    }

    bool operator == (const state &rhs) const
    {
        if(x == rhs.x && y == rhs.y)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    bool operator !=(const state& rhs) const
    {
        return !(*this == rhs);
    }

};

class boid
{
public:
    int id;
    state vel;
    state pos;
    float radius;

    boid(int id, float x, float y, float radius) :
        pos(x,y),
        vel(0,0),
        id(id),
        radius(radius)
    {
    }

    bool operator == (const boid &rhs) const
    {
        if(vel == rhs.vel && pos == rhs.pos)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    bool operator !=(const boid& rhs) const
    {
        return !(*this == rhs);
    }

    bool isWithinDistanceFrom( const boid& rhs, float distance)
    {
        float dx = pos.x - rhs.pos.x;
        float dy = pos.y - rhs.pos.y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist > distance)
        {
            return false;
        }
        else if (dist < fabs(radius - rhs.radius))
        {
            return false;
        }
        else if ((dist == 0) && (radius == rhs.radius))
        {
            return true;
        }
        else
        {
            return true;
        }
    }

    void wall_collision(float width, float height)
    {
        if (pos.x < width)
        {
            pos.x = 0;
        }
        else if(pos.x > width)
        {
            pos.x = width;
        }
        if (pos.y < height)
        {
            pos.y = 0;
        }
        else if(pos.y > height)
        {
            pos.y = height;
        }
    }
};

#endif // BOID_H
