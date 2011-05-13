/*
 * SimpleTestRunner.cpp
 *
 *  Created on: 3 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "SimpleRunner.h"

namespace cryo {

namespace runners {

SimpleRunner::SimpleRunner() {
	// add out kernel names
	//kernelNames.insert("hello");
	sourceString = "Data/Kernels/simpletest.cl";
	this->loadKernelSource(sourceString);
	this->loadKernel(kernelSource);
	this->compileKernel();

}

SimpleRunner::~SimpleRunner() {
}

void SimpleRunner::setUpBuffer() {
	num = 10;
	float *a = new float[num];
	float *b = new float[num];
	float *c = new float[num];
	for (int i = 0; i < num; i++) {
		a[i] = 1.0f * i;
		b[i] = 1.3f * i;
		c[i] = 0.0f;
		std::cout<<"SimpleRunner::setUpBuffer: "<<"("	<<a[i]<<","<<b[i] <<","<<c[i]<<")"<<std::endl;
	}

	printf("Creating OpenCL arrays\n");
	size_t array_size = sizeof(float) * num;
	//our input arrays

	try {
		cl_a = cl::Buffer(context, CL_MEM_READ_ONLY, array_size, NULL, &error);
		cl_b = cl::Buffer(context, CL_MEM_READ_ONLY, array_size, NULL, &error);
		//our output array
		cl_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, array_size, NULL, &error);
	} catch (cl::Error & er) {
		std::cout << "setUpBuffer: " << "Error: " << er.what() << std::endl;
		std::cout << oclErrorString(er.err()) << std::endl;
	}
	printf("Pushing data to the GPU\n");
	//push our CPU arrays to the GPU
	try {
		error = commandQueue.enqueueWriteBuffer(cl_a, CL_TRUE, 0, array_size, a, NULL, &event);
		error = commandQueue.enqueueWriteBuffer(cl_b, CL_TRUE, 0, array_size, b, NULL, &event);
		error = commandQueue.enqueueWriteBuffer(cl_c, CL_TRUE, 0, array_size, c, NULL, &event);
	} catch (cl::Error & er) {
		std::cout << "setUpBuffer: " << "Error: " << er.what() << std::endl;
		std::cout << oclErrorString(er.err()) << std::endl;
	}

	//set the arguements of our kernel
	boost::shared_ptr< cl::Kernel > kernel= 	 this->getMutableKernel("hello");
	try {
		error = kernel->setArg(0, cl_a);
		error = kernel->setArg(1, cl_b);
		error = kernel->setArg(2, cl_c);
	} catch (cl::Error & er) {
		std::cout << "SimpleRunner::setUpBuffer: " << "" << std::endl;
		std::cout << kernel << std::endl;
		std::cout << "setUpBuffer: " << "Error: " << er.what() << std::endl;
		std::cout << oclErrorString(er.err()) << std::endl;
	}
	//Wait for the command queue to finish these commands before proceeding
	commandQueue.finish();

}
bool SimpleRunner::run() {
	std::cout << "SimpleRunner::run: " << "" << std::endl;
	boost::shared_ptr< cl::Kernel > kernel= 	 this->getMutableKernel("hello");
	//execute the kernel
	error = commandQueue.enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(num), cl::NullRange, NULL, &event);
	///clReleaseEvent(event);
	std::cout << "SimpleRunner::run: " << oclErrorString(error) << std::endl;
	commandQueue.finish();

	//lets check our calculations by reading from the device memory and printing out the results
	float c_done[num];
	error = commandQueue.enqueueReadBuffer(cl_c, CL_TRUE, 0, sizeof(float) * num, &c_done, NULL, &event);
	std::cout << "SimpleRunner::run: " << oclErrorString(error) << std::endl;
	//clReleaseEvent(event);

	for (int i = 0; i < num; i++) {
		std::cout << "SimpleRunner::run: " << "c_done[" << i << "]" << "=" << c_done[i] << std::endl;
	}
	return true;

}

}

}
