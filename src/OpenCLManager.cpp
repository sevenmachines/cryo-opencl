/*
 * OpenCLManager.cpp
 *
 *  Created on: 5 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "OpenCLManager.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <sstream>

namespace cryo {

OpenCLManager::OpenCLManager() {
	initialise();
}
OpenCLManager::~OpenCLManager() {
}
std::string OpenCLManager::getKernelSource() {
	return kernelSource;
}

boost::shared_ptr<cl::Kernel> OpenCLManager::getMutableKernel(const std::string & name) {
	std::map<std::string, boost::shared_ptr<cl::Kernel> >::iterator it_found = kernels.find(name);
	boost::shared_ptr<cl::Kernel> kernel;
	if (it_found != kernels.end()) {
		kernel = it_found->second;
	}
	return kernel;
}
const std::vector<cl::Platform> & OpenCLManager::getPlatforms() const {
	return platforms;
}
const std::vector<cl::Device> & OpenCLManager::getDevices() const {
	return devices;
}
const cl::Device & OpenCLManager::getActiveDevice() const {
	return *activeDevice;
}
const cl::Context & OpenCLManager::getContext() const {
	return context;
}
const std::map<std::string, boost::shared_ptr<cl::Kernel> > & OpenCLManager::getKernels() const {
	return kernels;
}
const cl::CommandQueue & OpenCLManager::getCommandQueue() const {
	return commandQueue;
}
const cl::Program & OpenCLManager::getProgram() const {
	return program;
}

const char* OpenCLManager::oclErrorString(cl_int error) {
	static const char* errorString[] = { "CL_SUCCESS", "CL_DEVICE_NOT_FOUND", "CL_DEVICE_NOT_AVAILABLE",
			"CL_COMPILER_NOT_AVAILABLE", "CL_MEM_OBJECT_ALLOCATION_FAILURE", "CL_OUT_OF_RESOURCES",
			"CL_OUT_OF_HOST_MEMORY", "CL_PROFILING_INFO_NOT_AVAILABLE", "CL_MEM_COPY_OVERLAP",
			"CL_IMAGE_FORMAT_MISMATCH", "CL_IMAGE_FORMAT_NOT_SUPPORTED", "CL_BUILD_PROGRAM_FAILURE", "CL_MAP_FAILURE",
			"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "CL_INVALID_VALUE",
			"CL_INVALID_DEVICE_TYPE", "CL_INVALID_PLATFORM", "CL_INVALID_DEVICE", "CL_INVALID_CONTEXT",
			"CL_INVALID_QUEUE_PROPERTIES", "CL_INVALID_COMMAND_QUEUE", "CL_INVALID_HOST_PTR", "CL_INVALID_MEM_OBJECT",
			"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR", "CL_INVALID_IMAGE_SIZE", "CL_INVALID_SAMPLER", "CL_INVALID_BINARY",
			"CL_INVALID_BUILD_OPTIONS", "CL_INVALID_PROGRAM", "CL_INVALID_PROGRAM_EXECUTABLE",
			"CL_INVALID_KERNEL_NAME", "CL_INVALID_KERNEL_DEFINITION", "CL_INVALID_KERNEL", "CL_INVALID_ARG_INDEX",
			"CL_INVALID_ARG_VALUE", "CL_INVALID_ARG_SIZE", "CL_INVALID_KERNEL_ARGS", "CL_INVALID_WORK_DIMENSION",
			"CL_INVALID_WORK_GROUP_SIZE", "CL_INVALID_WORK_ITEM_SIZE", "CL_INVALID_GLOBAL_OFFSET",
			"CL_INVALID_EVENT_WAIT_LIST", "CL_INVALID_EVENT", "CL_INVALID_OPERATION", "CL_INVALID_GL_OBJECT",
			"CL_INVALID_BUFFER_SIZE", "CL_INVALID_MIP_LEVEL", "CL_INVALID_GLOBAL_WORK_SIZE", };

	const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

	const int index = -error;

	return (index >= 0 && index < errorCount) ? errorString[index] : "Unspecified Error";
}

bool OpenCLManager::initialise() {
	// Get platforms
	std::cout << "Getting Platforms... ";
	try {
		error = cl::Platform::get(&platforms);
		std::cout << "found: " << platforms.size() << std::endl;
		std::cout << platforms << std::endl;
	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;

	}
	std::cout << std::endl;

	std::cout << "Creating context... ";
	try {
		// Get context
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0](), 0 };

		context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
		std::cout << "done!" << std::endl;
		std::cout << context << std::endl;

	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;

	}
	std::cout << std::endl;

	// Get devices
	std::cout << "Getting devices... ";
	try {
		devices = context.getInfo<CL_CONTEXT_DEVICES> ();
		activeDevice = devices.begin(); // Just use first just now
		std::cout << "found: " << devices.size() << std::endl;
		std::cout <<  devices << std::endl;
	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;

	}
	std::cout << std::endl;
	// Create command queue
	std::cout << "Creating command queue...";
	try {
		commandQueue = cl::CommandQueue(context, *activeDevice, 0, &error);
		std::cout << "done!" << std::endl;
	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;

	}
	//std::cout<<commandQueue<<std::endl;
	std::cout << std::endl;

	return true;
}

bool OpenCLManager::loadKernelSource(const std::string ifstring) {
	std::ifstream ifs(ifstring.c_str());
	if (ifs != false) {
		std::stringstream buffer;
		std::string line;
		std::vector<std::string> parsed_funcs;
		while (getline(ifs, line)) {
			buffer << line << std::endl;
			parseFunctionNames(line);
		}
		kernelSource = buffer.str();
		ifs.close();

		//	std::cout << "loadKernelSource: Loaded ##########" << std::endl;
		//	std::cout << kernelSource << std::endl;
		//	std::cout << "##########################" << std::endl;

		std::cout << "Found Kernel Names: ";
		printKernelNames(std::cout);
		std::cout << std::endl << std::endl;
	} else {
		std::cout << "OpenCLRunner::loadKernelSource: " << "Error: Could not open file: " << ifstring << std::endl;
		return false;
	}
	return true;
}

bool OpenCLManager::compileKernel(std::string buildoptions) {
	try {
		std::cout << "Compiling program...";
		error = program.build(devices, buildoptions.c_str());
		std::cout << "done!" << std::endl;
		std::string build_status;
		std::string build_options;
		std::string build_log;
		// forall in devices
		{
			std::cout << std::endl;
			std::vector<cl::Device>::const_iterator it_devices = devices.begin();
			const std::vector<cl::Device>::const_iterator it_devices_end = devices.end();
			while (it_devices != it_devices_end) {
				std::cout << "\tDevice: " << *it_devices << std::endl;
				program.getBuildInfo(*it_devices, (cl_program_build_info) CL_PROGRAM_BUILD_OPTIONS, &build_options);
				program.getBuildInfo(*it_devices, (cl_program_build_info) CL_PROGRAM_BUILD_LOG, &build_log);
				std::cout << "\tBuild Options: " << build_options << std::endl;
				std::cout << "\tBuild Log: " <<std::endl;
				std::cout<<build_log << std::endl;
				++it_devices;
			}
		}
	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;

		std::string build_status;
		std::string build_options;
		std::string build_log;
		// forall in devices
		{
			std::cout << std::endl;
			std::vector<cl::Device>::const_iterator it_devices = devices.begin();
			const std::vector<cl::Device>::const_iterator it_devices_end = devices.end();
			while (it_devices != it_devices_end) {
				std::cout << "\tDevice: " << *it_devices << std::endl;
				program.getBuildInfo(*it_devices, (cl_program_build_info) CL_PROGRAM_BUILD_OPTIONS, &build_options);
				program.getBuildInfo(*it_devices, (cl_program_build_info) CL_PROGRAM_BUILD_LOG, &build_log);
				std::cout << "\tBuild Options: " << build_options << std::endl;
				std::cout << "\tBuild Log: " <<std::endl;
				std::cout<< build_log << std::endl;
				++it_devices;
			}
		}
		this->printKernelSource(std::cout);
		return false;
	}
	std::cout << std::endl;

	try {
		std::cout << "Creating kernels...";
		// forall in kernelNames
		{
			std::set<std::string>::const_iterator it_kernelNames = kernelNames.begin();
			const std::set<std::string>::const_iterator it_kernelNames_end = kernelNames.end();
			while (it_kernelNames != it_kernelNames_end) {
				boost::shared_ptr<cl::Kernel> kernel(new cl::Kernel(program, it_kernelNames->c_str(), &error));
				kernels[*it_kernelNames] = kernel;

				++it_kernelNames;
			}
		}
		std::cout << "done." << std::endl;
	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;

		return false;
	}
	std::cout << std::endl;
	return true;
}

bool OpenCLManager::loadKernel(std::string kernelstring) {
	std::cout << "Loading kernel...";
	//	std::cout << kernelstring << std::cout;
	try {
		cl::Program::Sources source(1, std::make_pair(kernelstring.c_str(), kernelstring.size()));
		program = cl::Program(context, source);
		std::cout << "done!" << std::endl;
		//	std::cout << kernelstring << std::endl;
	} catch (cl::Error & er) {
		std::cout << printCLError(std::cout, er) << std::endl;
		return false;
	}
	std::cout << std::endl;
	return true;
}

std::ostream & OpenCLManager::printKernelSource(std::ostream & os) {
	boost::char_separator<char> sep("\n");
	boost::tokenizer<boost::char_separator<char> > tok(kernelSource, sep);
	int lineno = 1;
	for (boost::tokenizer<boost::char_separator<char> >::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
		os << lineno << ": " << *beg << "\n";
		++lineno;
	}

	return os;
}
void OpenCLManager::parseFunctionNames(const std::string & line) {
	//std::cout << "OpenCLManager::parseFunctionNames: found: " << line.find("__kernel")<<":"<<std::string::npos << " : " << line << std::endl;
	if (line.find("__kernel") != std::string::npos && (line.find("__attribute") == std::string::npos)) {
		size_t start = line.find("(");
		std::string sub = line.substr(0, start);
		//std::cout << "OpenCLManager::parseFunctionNames: " << "substring" << start << ":" << sub << std::endl;
		//std::cout << "OpenCLManager::parseFunctionNames: " << line << std::endl;
		// try to extract function name
		{
			boost::tokenizer<> tok(sub);
			int count = 0;
			const int NAME_INDEX = 2;
			// forall in tok
			{
				boost::tokenizer<>::const_iterator it_tok = tok.begin();
				const boost::tokenizer<>::const_iterator it_tok_end = tok.end();
				while (it_tok != it_tok_end && count < NAME_INDEX) {
					++count;
					//std::cout << "OpenCLManager::parseFunctionNames: " << "count:" << count << std::endl;
					++it_tok;
				}
				std::string kernname = *it_tok;
				boost::trim(kernname);
				kernelNames.insert(kernname);

			}
		}
	}
}

std::ostream & OpenCLManager::printKernelNames(std::ostream & os) {
	// forall in kernelNames
	{
		os << "{ ";
		std::set<std::string>::const_iterator it_kernelNames = kernelNames.begin();
		const std::set<std::string>::const_iterator it_kernelNames_end = kernelNames.end();
		while (it_kernelNames != it_kernelNames_end) {
			os << *it_kernelNames << " ";
			++it_kernelNames;
		}
		os << " }";
	}
	return os;
}

std::ostream & OpenCLManager::printCLError(std::ostream & os, const cl::Error & er) {
	os << "ERROR! " << std::endl;
	os << "\t" << er.what() << std::endl;
	os << "\t" << oclErrorString(er.err());
	return os;
}

}//NAMESPACE
