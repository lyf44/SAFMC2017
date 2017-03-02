#include "PID.h"
		
PID::PID(float deltat,float kp,float kd,float ki):Kp(kp),Kd(kd),Ki(ki),delta_t(deltat){			
	K1= Kp+ Ki*delta_t + Kd/delta_t;
    K2 = (-1)*Kp - 2*Kd/delta_t;
    K3 = Kd/delta_t;
	error = 0.;
	prev_error = 0;
	prev_prev_error = 0;
	prev_result = 0.0f;
}

void PID::setPID(float Kp, float Kd, float Ki){
	this->Kp = Kp;
	this->Kd = Kd;
	this->Ki = Ki;
	K1= Kp+ Ki*delta_t + Kd/delta_t;
    K2 = (-1)*Kp - 2*Kd/delta_t;
    K3 = Kd/delta_t;
	reset();
}

void PID::reset(){
	error = 0;
	prev_error = 0;
	prev_prev_error = 0;
	prev_result = 0.0f;
}

