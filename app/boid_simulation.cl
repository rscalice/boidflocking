
// Define auxiliary helper functions for kernel
inline bool isWithinDistanceFrom(float x1,float y1,float x2,float y2, float distance, float radius)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    float dist = sqrt(dx * dx + dy * dy);

    if (dist > distance)
    {
        return false;
    }
    else if (dist < fabs(radius - radius))
    {
        return false;
    }
    else if ((dist == 0) && (radius == radius))
    {
        return true;
    }
    else
    {
        return true;
    }
}

typedef struct  {
    float x_pos;
    float y_pos;
    float x_vel;
    float y_vel;
    int id;
    int radius;
} boid_simple;

typedef struct {
    int distance_mod;
    int vel_mod;
    int mass_mod;
    int wind_x;
    int wind_y;
    int vel_lim;
	int world_height;
	int world_width;
} params;

__kernel void simulate(__global boid_simple *boids, __global int* num_boids, __global params* sim_params, __local boid_simple* local_var)
{
    size_t tid = get_global_id(0);  // id for global memory
    int ti = get_local_id(0);       // id into local memory

    int n = get_global_size(0);     // global memory size
    int nt = get_local_size(0);     // local memory size
    int nb = n/nt;                  // calculates the number of work groups

    float center_mass_x, center_mass_y;
    float repel_x=0, repel_y=0;
    float vel_x =0, vel_y = 0;

//    simple_boid old = boids[tid];
//p = pos_old[gti];
//// -- Optimized ---
    /*for(int jb=0; jb < nb; jb++) { // for each block

        local_var[ti] = boids[jb*nt+ti]; // cache one boid
        barrier(CLK_LOCAL_MEM_FENCE); // wait for others in the group

        for (int j=0; j < nt; j++) {
//            if (j != ti) {
                //boid_simple temp = local_var[j];

                // Rule 1
                center_mass_x += local_var[j].x_pos;
                center_mass_y += local_var[j].y_pos;

  //          }
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }*/

// -- Unoptimized ---
    for (int i=0; i < *num_boids; i++)
    {
        if (i != tid) {

            // Rule 1
            center_mass_x += boids[i].x_pos;
            center_mass_y += boids[i].y_pos;

            // Rule 2
            if (isWithinDistanceFrom(boids[tid].x_pos, boids[tid].y_pos, boids[i].x_pos, boids[i].y_pos, sim_params->distance_mod, boids[tid].radius)){
                repel_x -=  fabs(boids[i].x_pos - boids[tid].x_pos);
                repel_y -=  fabs(boids[i].y_pos - boids[tid].y_pos);
            }

            //Rule 3
            vel_x = vel_x + boids[i].x_vel;
            vel_y = vel_y + boids[i].y_vel;

        }
    }


    // Normalize Rule 1 mass calculation
    center_mass_x /= (*num_boids -1);
    center_mass_y /= (*num_boids -1);

    center_mass_x = (center_mass_x - boids[tid].x_pos) / sim_params->mass_mod; // need to pass in mass modifier (should probably just include a struct with all weights)
    center_mass_y = (center_mass_y - boids[tid].y_pos) / sim_params->mass_mod;


    // Normalize Rule 3 Velocity calculation
    vel_x /= (*num_boids-1);
    vel_y /= (*num_boids-1);

    vel_x = (vel_x - boids[tid].x_vel)/sim_params->vel_mod;
    vel_y = (vel_y - boids[tid].y_vel)/sim_params->vel_mod;

    // Put in barrier to synchronize threads. If threads finish at different times,
    //current tid may use updated boid positions for calculations
//    barrier(CLK_GLOBAL_MEM_FENCE);

    // Update velocities
    boids[tid].x_vel += center_mass_x + repel_x + vel_x;
    boids[tid].y_vel += center_mass_y + repel_y + vel_y;


    // Limit the speed
    float mag = fabs(sqrt((boids[tid].x_vel*boids[tid].x_vel) + (boids[tid].y_vel)*(boids[tid].y_vel)));
    if ( mag > sim_params->vel_lim)
    {
        boids[tid].x_vel = (boids[tid].x_vel / mag) * sim_params->vel_lim;
        boids[tid].y_vel = (boids[tid].y_vel / mag) * sim_params->vel_lim;
    }

    // Update new boid position
    boids[tid].x_pos += boids[tid].x_vel;
    boids[tid].y_pos += boids[tid].y_vel;

    // bound boids to frame
    if (boids[tid].x_pos < 10)
    {
        boids[tid].x_pos = boids[tid].x_pos + (sim_params->world_width - 10);
    }
    else if (boids[tid].x_pos > sim_params->world_width)
    {
        boids[tid].x_pos = boids[tid].x_pos - (sim_params->world_width - 10);
    }

    if (boids[tid].y_pos < 0)
    {
        boids[tid].y_pos  = boids[tid].y_pos + sim_params->world_height;
    }
    else if (boids[tid].y_pos  > sim_params->world_height)
    {
        boids[tid].y_pos = boids[tid].y_pos - sim_params->world_height;
    }

}
