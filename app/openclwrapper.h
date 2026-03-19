#ifndef OPENCLWRAPPER_H
#define OPENCLWRAPPER_H

#include <CL/opencl.hpp>

#include <boid.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct params {

    cl_int distance_mod;
    cl_int vel_mod;
    cl_int mass_mod;
    cl_int wind_x;
    cl_int wind_y;
    cl_int vel_lim;
    cl_int world_height;
    cl_int world_width;
};

class OpenCLWrapper
{
public:
    OpenCLWrapper();

    std::string getPlatformInfo();
    void advance_boid_states(std::vector<boid_simple>& boids, int platform, int device);
    void updateSimulationParamters(int distance_mod, int vel_mod, int mass_mod, int wind_x, int wind_y, int vel_lim, int world_width, int world_height);

private:

    bool buildOpenCLKernel(int platform, int device);

    inline void checkErr(cl_int err, const char * name)
    {
        if (err != CL_SUCCESS) {
            std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    cl_mem cl_boid_buf;
    cl_mem cl_params;
    cl_mem cl_num_boids;

    cl_kernel kernel;
    cl_program program;

    int last_platform;
    int last_device;

    params sim_params;

    cl_context context;
    cl_platform_id platforms[32];
    cl_device_id devices[32];
    std::stringstream output;
    cl_uint num_platforms;
    cl_command_queue commandQueue;

    bool initialized_context;
};

#endif // OPENCLWRAPPER_H
