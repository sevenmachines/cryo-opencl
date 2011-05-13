/*
 * OpenCLPrint.h
 *
 *  Created on: 2 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef OPENCLPRINT_H_
#define OPENCLPRINT_H_

#include <ostream>
#include "CL/cl.hpp"
#include <string>
#include <map>

std::ostream & operator<<(std::ostream & os, const cl::Platform & platform);
std::ostream & operator<<(std::ostream & os, const std::vector<cl::Platform> & platforms);
std::ostream & operator<<(std::ostream & os, const cl::Device & device);
std::ostream & operator<<(std::ostream & os, const std::vector<cl::Device> & devices);
std::ostream & operator<<(std::ostream & os, const cl::Context & context);
std::ostream & operator<<(std::ostream & os, const cl::CommandQueue & queue);
std::ostream & operator<<(std::ostream & os, const cl::Kernel & kernel);
std::ostream & operator<<(std::ostream & os, const cl::Program & program);

class OpenCLPrint {
	static std::map<std::string, std::string> createCLDefMap();
	static std::map<std::string, std::string> defMap;
};
#endif /* OPENCLPRINT_H_ */
