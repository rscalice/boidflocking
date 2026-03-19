#include "openclwrapper.h"

#include <QMessageBox>
#include <fstream>

OpenCLWrapper::OpenCLWrapper() :
    initialized_context(false)
{
    clGetPlatformIDs(32, platforms, &num_platforms);

    for (unsigned int i = 0; i < num_platforms; i++)
    {
        output << "Platform: " << i << std::endl;

        char vendor[1024];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, NULL);
        cl_uint num_devices;
        cl_device_id devices[32];
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, sizeof(devices), devices, &num_devices);

        output << "\tPlatform Vendor: " << vendor << std::endl;
        output << "\tNumber of devices: " << num_devices << std::endl;

        for (unsigned int j = 0; j < num_devices; j++)
        {
            //scan in device information
            char deviceName[1024];
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);

            clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);

            cl_uint numberOfCores;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(numberOfCores), &numberOfCores, NULL);

            cl_long amountOfMemory;
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(amountOfMemory), &amountOfMemory, NULL);

            cl_uint clockFreq;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFreq), &clockFreq, NULL);

            cl_ulong maxAlocatableMem;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxAlocatableMem), &maxAlocatableMem, NULL);

            cl_ulong localMem;
            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMem), &localMem, NULL);

            cl_bool	available;
            clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(available), &available, NULL);

            output << "\tDevice: " << j << std::endl;
            output << "\t\tName: " << deviceName << std::endl;
            output << "\t\tVendor: " << vendor << std::endl;
            output << "\t\tAvailable: " << (available ? "Yes" : "No") << std::endl;
            output << "\t\tCompute Units: " << numberOfCores << std::endl;
            output << "\t\tClock Frequency: " << clockFreq << std::endl;
            output << "\t\tGlobal Memory: " << (amountOfMemory / 1048576) << std::endl;
            output << "\t\tMax Allocateable Memory: " << (maxAlocatableMem / 1048576) << std::endl;
            output << "\t\tLocal Memory: " << localMem << std::endl;
        }
    }
}

void OpenCLWrapper::advance_boid_states(std::vector<boid_simple>& boids, int platform, int device)
{

    if(initialized_context && ((last_platform != platform) || (last_device != device)))
    {
        initialized_context = false;
    }

    if(buildOpenCLKernel(platform, device))
    {
        int num_boids = boids.size();

        cl_int errNum;

        // Allocate the memory buffers
        cl_boid_buf = clCreateBuffer(context,
                                     CL_MEM_READ_WRITE,
                                     sizeof(boid_simple) * boids.size(),
                                     NULL,
                                     NULL);


        cl_params = clCreateBuffer(context,
                                     CL_MEM_READ_ONLY,
                                     sizeof(params),
                                     NULL,
                                     NULL);

        cl_num_boids = clCreateBuffer( context,
                                        CL_MEM_READ_ONLY,
                                        sizeof(int),
                                        NULL,
                                        &errNum);

        // Write data to buffers
        clEnqueueWriteBuffer(commandQueue, cl_num_boids, CL_TRUE, 0, sizeof(int), &num_boids, 0, NULL, NULL);
        clEnqueueWriteBuffer(commandQueue, cl_boid_buf, CL_TRUE, 0, sizeof(boid_simple) * boids.size(), boids.data(), 0, NULL, NULL);
        clEnqueueWriteBuffer(commandQueue, cl_params, CL_TRUE, 0, sizeof(params), &sim_params, 0, NULL, NULL);

        // Set the kernel arguments
        errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&cl_boid_buf);
        errNum = clSetKernelArg(kernel, 1, sizeof(cl_mem), &cl_num_boids);
        errNum = clSetKernelArg(kernel, 2, sizeof(cl_mem), &cl_params);
        errNum = clSetKernelArg(kernel, 3, 32 * sizeof(boid_simple), NULL);

        checkErr(errNum, "clSetKernelArg(cl_boid_buf)");

        size_t globalWorkSize[1] = { boids.size() };
        size_t localWorkSize[1] = { 1 };

        // Queue the kernel up for execution across the array
        cl_event event;
        errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize,
                                        0, NULL, &event);
        clWaitForEvents(1, &event);

        errNum = clEnqueueReadBuffer(commandQueue, cl_boid_buf, CL_TRUE, 0, sizeof(boid_simple)*boids.size(), boids.data()/*boid_buf*/, 0, NULL, NULL);

        clReleaseMemObject(cl_boid_buf);

    }
}

bool OpenCLWrapper::buildOpenCLKernel(int platform, int device)
{
    if(!initialized_context)
    {
        std::cout << "Initializing OpenCL" << std::endl;
        cl_int errNum;

        cl_uint num_devices;
        clGetDeviceIDs(platforms[platform], CL_DEVICE_TYPE_ALL, sizeof(devices), devices, &num_devices);

        // Next, create an OpenCL context on the selected platform.
        cl_context_properties contextProperties[] =
        {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)platforms[platform],
            0
        };

        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
        if (errNum != CL_SUCCESS)
        {
            context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                              NULL, NULL, &errNum);
            if (errNum != CL_SUCCESS)
            {
                std::cerr << "Failed to create an OpenCL GPU or CPU context." << std::endl;
                return false;
            }
        }

        checkErr(errNum, "clCreateContext");

        std::ifstream srcFile("boid_simulation.cl");
        checkErr(srcFile.is_open() ? CL_SUCCESS : -1, "reading boid_simulation.cl");

        std::string kernel_source = std::string(std::istreambuf_iterator<char>(srcFile), std::istreambuf_iterator<char>());

        const char * kernel_src = kernel_source.c_str();
        size_t length = kernel_source.length();

        // Create program from source
        program = clCreateProgramWithSource( context, 1, &kernel_src, &length, &errNum);

        checkErr(errNum, "clCreateProgramWithSource");

        // Build program
        errNum = clBuildProgram( program, 0, NULL, NULL, NULL, NULL);

        if (errNum != CL_SUCCESS)
        {
            // Determine the reason for the error
            char buildLog[16384];
            clGetProgramBuildInfo( program, devices[device], CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);

            std::cerr << "Error in kernel: " << std::endl;
            std::cerr << buildLog;
            checkErr(errNum, "clBuildProgram");
            return false;
        }

        // Create kernel object
        kernel = clCreateKernel( program, "simulate", &errNum);
        checkErr(errNum, "clCreateKernel");

        commandQueue = clCreateCommandQueue( context, devices[device], 0, &errNum);
        checkErr(errNum, "clCreateCommandQueue");

        last_device = device;
        last_platform = platform;
        initialized_context = true;
    }

    return true;
}

std::string OpenCLWrapper::getPlatformInfo()
{
    return output.str();
}

void OpenCLWrapper::updateSimulationParamters(int distance_mod, int vel_mod, int mass_mod, int wind_x, int wind_y, int vel_lim, int world_width, int world_height){

    this->sim_params.wind_x = wind_x;
    this->sim_params.wind_y = wind_y;
    this->sim_params.vel_mod = vel_mod;
    this->sim_params.distance_mod = distance_mod;
    this->sim_params.mass_mod = mass_mod;
    this->sim_params.vel_lim = vel_lim;
    this->sim_params.world_height = world_height;
    this->sim_params.world_width = world_width;

}

