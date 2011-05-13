/*
 * NullRunner.cpp
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "NullRunner.h"

namespace cryo {

namespace runners {

NullRunner::NullRunner( std::string  filename) {
	sourceString = filename;
	if ( this->loadKernelSource(sourceString) == true){
		if ( this->loadKernel(kernelSource) == false){
			std::cout<<"NullRunner::NullRunner: "<<"ERROR: Compiling kernel"<<std::endl;
		}
	}else{
		std::cout<<"NullRunner::NullRunner: "<<"ERROR: Loading kernel"<<std::endl;
	}
}

NullRunner::~NullRunner() {
	// TODO Auto-generated destructor stub
}

}

}
