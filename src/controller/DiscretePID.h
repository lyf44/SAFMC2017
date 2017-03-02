#ifndef DiscretePID_H
#define DiscretePID_H

#include "PID.h"

class DiscretePID : public PID{
	public:
		DiscretePID(float deltat,float kp = 1,float kd = 0.0f,float ki = 0.0f);
		float controlOnce(int error);
};		

#endif
