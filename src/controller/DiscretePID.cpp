#include "DiscretePID.h"

DiscretePID::DiscretePID(float deltat,float kp,float kd,float ki) : 
								PID(deltat,kp,kd,ki){ }


float DiscretePID::controlOnce(int error){
	float outResult;
	outResult = prev_result + K1*error + K2*prev_error + K3*prev_prev_error;
	prev_prev_error = prev_error;
	prev_error = error;
	prev_result = outResult;
	return outResult;
}

