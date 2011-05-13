/*
 * NullRunner.h
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef NULLRUNNER_H_
#define NULLRUNNER_H_

#include "OpenCLManager.h"

namespace cryo {

namespace runners {

/**
 * Null runner will not allow running the cl code but allows us to instantiate a manager for other uses
 */
class NullRunner : public OpenCLManager{
public:
	NullRunner( std::string filename);
	virtual ~NullRunner();
	virtual bool run(){
		return false;
	}
};

}

}

#endif /* NULLRUNNER_H_ */
