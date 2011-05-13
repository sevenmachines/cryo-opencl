/*
 * OpenCLManager.h
 *
 *  Created on: 3 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef OPENCLMANAGER_H_
#define OPENCLMANAGER_H_
#define __CL_ENABLE_EXCEPTIONS

#include "OpenCLPrint.h"
#include "CL/cl.hpp"
#include <CL/cl.h>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace cryo {

/**
 * The OpenCLManager class deals with the bulk of setting up and tearing down
 * opencl. Subclasses load program source in their constructor and then
 * should implement the run() method to actually do calculations.
 */
class OpenCLManager {
public:
	OpenCLManager();
	virtual ~OpenCLManager();
	virtual bool run()=0;

	virtual bool loadKernelSource(const std::string ifstring);

	virtual const char* oclErrorString(cl_int error);

	virtual bool loadKernel(std::string kernelstring);
	virtual bool compileKernel(std::string buildoptions="");

	//getters
	/**
	 * Get kernel by kernel name
	 */
	boost::shared_ptr<cl::Kernel> getMutableKernel(const std::string & name);
	virtual std::string getKernelSource();
	const std::vector<cl::Platform> & getPlatforms() const ;
	const std::vector<cl::Device> & getDevices() const;
	const cl::Device & getActiveDevice() const;
	const cl::Context & getContext() const;
	const std::map<std::string, boost::shared_ptr<cl::Kernel> > & getKernels() const ;
	const cl::CommandQueue & getCommandQueue() const ;
	const cl::Program & getProgram() const ;

	void parseFunctionNames(const std::string & source);
	std::ostream & printKernelNames(std::ostream & os);
	std::ostream & printKernelSource(std::ostream & os);
	std::ostream & printCLError(std::ostream & os, const cl::Error & er);

protected:
	std::string kernelSource;
	std::string sourceString;
	std::set<std::string> kernelNames;

	bool platform_id_success;
	bool device_id_success;
	bool kernel_compilation_success;

	std::vector<cl::Platform> platforms;

	std::vector<cl::Device> devices;
	std::vector<cl::Device>::const_iterator activeDevice;

	cl::Context context;

	std::map<std::string, boost::shared_ptr<cl::Kernel> > kernels;
	cl::CommandQueue commandQueue;
	cl::Program program;

	cl_int error;
	cl::Event event;

	bool initialise();

};

}
#endif /* OPENCLMANAGER_H_ */
