#ifndef PID_H
#define PID_H

#include <iostream>

class PID {
	protected:
		float Kp;
		float Kd;
		float Ki;
		float K1;
		float K2;
		float K3;
		int error;
		int prev_error;
		int prev_prev_error;
		float delta_t;
		float prev_result;
	public:
		PID(float deltat,float kp=1,float kd=0.0f,float ki=0.0f);
		void setPID(float Kp, float Kd, float Ki);
		virtual float controlOnce(int error){};
		void reset();
};

#endif
