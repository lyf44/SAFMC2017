#ifndef PULSEPID_H
#define PULSEPID_H

#include "PID.h"

class PulsePID : public PID{
	private:
		int pulseCounter;
		int pulseLength;
		int totalLength;
		int temp_pulseLength;
		bool flag_OneTimeLongPulse;
	public:
		PulsePID(float deltat,float kp = 1,float kd = 0.0f,float ki = 0.0f,int pulse_length =3,int total_length=8);
		float controlOnce(int error);
		void setOneTimePulseLength(int pulse_length);
		void setTotalLength(int total_length);
};		

#endif
