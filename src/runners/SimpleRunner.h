/*
 * SimpleRunner.h
 *
 *  Created on: 3 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef SIMPLERUNNER_H_
#define SIMPLERUNNER_H_

#include "OpenCLManager.h"

namespace cryo {

namespace runners {

class SimpleRunner : public OpenCLManager{
public:
	SimpleRunner();
	virtual ~SimpleRunner();
	virtual bool run();
	virtual void setUpBuffer();

protected:
	cl::Buffer cl_a;
	        cl::Buffer cl_b;
	        cl::Buffer cl_c;
	        int num;    //the size of our arrays
};

}

}

#endif /* SIMPLETESTRUNNER_H_ */
