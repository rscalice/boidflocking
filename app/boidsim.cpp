#include "boidsim.h"
#include "boid.h"
#include <vector>
#include <random>
#include <iostream>

BoidSim::BoidSim() :
    is_initialized(false)
{
}

void BoidSim::initialize(vector<boid_simple>& boids, int num_boids, int width, int height, float radius)
{
    world_width = width;
    world_height = height;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> width_dis(0,  width);
    std::uniform_real_distribution<> height_dis(0, height);

    boids.clear();
    // initialize boids positions
    for (int i = 0; i < num_boids; i++)
    {
        boid_simple temp;
        temp.id = i;
        temp.radius = radius;
        temp.x_pos = (float) width_dis(gen);
        temp.y_pos = (float) height_dis(gen);
        temp.x_vel = 0;
        temp.y_vel = 0;
        boids.push_back(temp);
    }

    is_initialized = true;
}

void BoidSim::advance_boid_states_cpu(vector<boid_simple>& boids)
{
    for(std::vector<boid_simple>::iterator it = boids.begin(); it != boids.end(); ++it)
    {
        state vel_sum;

        // Rule 1 - attraction force, center of mass neighboring boids
        state v1 = rule1_cpu(*it, boids);

        // Rule 2 - repel force from other boids
        state v2 = rule2_cpu(*it, boids);

        // Rule 3 - center of mass neighboring boids
        state v3 = rule3_cpu(*it, boids);

        state v4 = bound_postion(*it);

        state v5 = strong_wind();

        state v6 = tend_to_place(*it);

        // Add velocity vectors
//        it->vel = it->vel + v1 + v2 + v3 + v4 + v5 + v6;
        it->x_vel = it->x_vel + v1.x + v2.x + v3.x + v4.x + v5.x + v6.x;
        it->y_vel = it->y_vel + v1.y + v2.y + v3.y + v4.y + v5.y + v6.y;

        // limit the speed
        limit_speed(*it);

        it->x_pos = it->x_pos + it->x_vel;
        it->y_pos = it->y_pos + it->y_vel;

        if (it->x_pos < 10)
        {
            it->x_pos = it->x_pos + (world_width - 10);
        }
        else if (it->x_pos > world_width)
        {
            it->x_pos = it->x_pos -  (world_width - 10);
        }

        if (it->y_pos < 0)
        {
            it->y_pos = it->y_pos + world_height;
        }
        else if (it->y_pos > world_height)
        {
            it->y_pos = it->y_pos -  world_height;
        }
    }
}

state BoidSim::rule1_cpu(const boid_simple & b, vector<boid_simple> boids) {

    state center_mass;

    for(std::vector<boid_simple>::iterator it = boids.begin(); it != boids.end(); ++it)
    {
        if (it->id != b.id)
        {
            center_mass.x = center_mass.x + it->x_pos;
            center_mass.y = center_mass.y + it->y_pos;

        }
    }

    center_mass = center_mass / ((int) boids.size() - 1 );

    center_mass.x = (center_mass.x - b.x_pos) / mass_modifier;
    center_mass.y = (center_mass.y - b.y_pos) / mass_modifier;


    return center_mass;
}

state BoidSim::rule2_cpu(const boid_simple & b, vector<boid_simple> boids)
{
    state c;

    for(std::vector<boid_simple>::iterator it = boids.begin(); it != boids.end(); ++it)
    {
        if(it->id != b.id)
        {
            if (isWithinDistanceFrom(b, *it, (float) distance_modifier))
            {
                float new_x = fabs(it->x_pos - b.x_pos);
                float new_y = fabs(it->y_pos - b.y_pos);

                c.x -= new_x;
                c.y -= new_y;
            }
        }
    }
    return c;
}


state BoidSim::rule3_cpu(const boid_simple & b, vector<boid_simple> boids)
{
    state pv;

    for(std::vector<boid_simple>::iterator it = boids.begin(); it != boids.end(); ++it)
    {
        if (it->id != b.id)
        {
            // this was set to b.vel which means we were adding
            // input boid velocity back to itself...how did this ever work
            pv.x = pv.x + it->x_vel; //b.vel;
            pv.y = pv.y + it->y_vel;
        }
    }

    pv = pv / ( (int) boids.size() - 1);

    pv.x = (pv.x - b.x_vel)/velocity_modifier;
    pv.y = (pv.y - b.y_vel)/velocity_modifier;


    //return (pv - b.vel) / velocity_modifier;
    return pv;
}

state BoidSim::bound_postion(const boid_simple & b)
{
    state v;
    float focus = 10;
    if (b.x_pos < 10)
    {
        v.x = focus;
    }
    else if (b.x_pos >= (world_width - 10))
    {
        v.x = -focus;
    }

    if (b.y_pos < 10)
    {
        v.y = focus;
    }
    else if (b.y_pos >= (world_height- 10))
    {
        v.y = -focus;
    }

    return v;
}

void BoidSim::limit_speed(boid_simple & b)
{
    float mag = fabs(sqrt((b.x_vel)*(b.x_vel) + (b.y_vel)*(b.y_vel)));
    if ( mag > velocity_limit)
    {
        b.x_vel = b.x_vel / mag * velocity_limit;
        b.y_vel = b.y_vel / mag * velocity_limit;
    }
}

state BoidSim::strong_wind()
{
    state wind;
    wind.x = (float) wind_x;
    wind.y = (float) wind_y;

    return wind;
}

state BoidSim::tend_to_place(const boid_simple & b)
{
    state place;
    place.x = (float) (world_height / 2.0f);
    place.y = (float) (world_width  / 2.0f);

    place.x = (place.x - b.x_pos)/100;
    place.y = (place.y - b.y_pos)/100;
    //return (place - b.pos) / 100;
    return place;
}

void BoidSim::resetWorld(vector<boid_simple>& boids)
{
    is_initialized = false;
    boids.clear();
    world_height = 0;
    world_width = 0;
}

void BoidSim::updateSimulationParamters(int distance_mod, int vel_mod, int mass_mod, int wind_x, int wind_y, int vel_lim, int world_width, int world_height)
{
    this->wind_x = wind_x;
    this->wind_y = wind_y;
    this->velocity_modifier = vel_mod;
    this->distance_modifier = distance_mod;
    this->mass_modifier = mass_mod;
    this->velocity_limit = vel_lim;
    this->world_width = world_width;
    this->world_height = world_height;
}

bool BoidSim::isWithinDistanceFrom( const boid_simple & rhs, const boid_simple b, float distance)
{
    float dx = b.x_pos - rhs.x_pos;
    float dy = b.y_pos - rhs.y_pos;
    double dist = sqrt(dx * dx + dy * dy);

    if (dist > distance)
    {
        return false;
    }
    else if (dist < fabs((float) b.radius - (float) rhs.radius))
    {
        return false;
    }
    else if ((dist == 0) && (b.radius == rhs.radius))
    {
        return true;
    }
    else
    {
        return true;
    }
}
