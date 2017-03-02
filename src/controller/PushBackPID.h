#ifndef PUSHBACKPID_H
#define PUSGBACKPID_H

#include "PID.h"

class PushBackPID : public PID{
	private:
	public:
		PushBackPID(float deltat,float kp = 1,float kd = 0.0f,float ki = 0.0f);
		float controlOnce(int error);
};		

#endif
