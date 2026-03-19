#ifndef BOIDSIM_H
#define BOIDSIM_H

#include "boid.h"
#include <vector>

using namespace std;

class BoidSim
{

    int world_width, world_height;

public:
    BoidSim();

    void initialize(vector<boid_simple>& boids, int num_boids, int width, int height, float radius);           // functon for settinging boid start positions
    void resetWorld(vector<boid_simple>& boids);

    void advance_boid_states_cpu(vector<boid_simple>& boids);
    void updateSimulationParamters(int distance_mod, int vel_mod, int mass_mod, int wind_x, int wind_y, int vel_lim, int world_width, int world_height);


    state rule1_cpu(const boid_simple & b, vector<boid_simple> boids);
    state rule2_cpu(const boid_simple & b, vector<boid_simple> boids);
    state rule3_cpu(const boid_simple & b, vector<boid_simple> boids);
    state bound_postion(const boid_simple & b);
    state strong_wind();
    state tend_to_place(const boid_simple & b);
    bool isWithinDistanceFrom( const boid_simple & rhs, const boid_simple b, float distance);
    void limit_speed(boid_simple & b);

    int distance_modifier;
    int velocity_modifier;
    int mass_modifier;
    int wind_x;
    int wind_y;
    int velocity_limit;

    bool is_initialized;

};

#endif // BOIDSIM_H
