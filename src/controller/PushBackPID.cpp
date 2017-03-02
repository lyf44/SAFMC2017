#include "PushBackPID.h"
#include "math.h"
#include <stdlib.h>

PushBackPID::PushBackPID(float deltat,float kp,float kd,float ki) : 
								PID(deltat,kp,kd,ki){ }

float PushBackPID::controlOnce(int error){
	float outResult;

	/* control at first*/
	if (fabs(prev_result)<0.0001){
		outResult = Kp * error;
		prev_result = outResult;
	}
	/*maintain result until error decreases*/
	else if (abs(error)>=abs(prev_error)){
		outResult = prev_result;
	}
	/*if error decreases, give zero command instantly*/
	else{
		outResult = 0.0f;
		prev_result = 0.0f;
	}
	/* copy error anyway*/
	prev_error = error;
	
	return outResult;
}

	
