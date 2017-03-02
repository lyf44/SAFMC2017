#include "PulsePID.h"

PulsePID::PulsePID(float deltat,float kp,float kd,float ki,int pulse_length,int total_length) : 
								PID(deltat,kp,kd,ki),pulseLength(pulse_length),totalLength(total_length){ 
	pulseCounter = 0;
	temp_pulseLength = 0;
	flag_OneTimeLongPulse = false;
}

float PulsePID::controlOnce(int error){
	float outResult;
	int temp;
	
	if (temp_pulseLength != 0){
		temp = temp_pulseLength;
	}else{
		temp = pulseLength;
	}

	if (pulseCounter == 1){
		outResult = prev_result + K1*error + K2*prev_error + K3*prev_prev_error;
		prev_prev_error = prev_error;
		prev_error = error;
		prev_result = outResult;
	}
	else if (pulseCounter <= temp){
		outResult = prev_result;
	}else{
		outResult = 0.0f;
	}
	
	pulseCounter++;
	if (pulseCounter>totalLength){
		pulseCounter = 1;
		if (flag_OneTimeLongPulse){
			flag_OneTimeLongPulse = false;
			temp_pulseLength = 0;
		}
	}

	return outResult;
}

void PulsePID::setOneTimePulseLength(int pulse_length){
	temp_pulseLength = pulse_length;
	flag_OneTimeLongPulse = true;
}

void PulsePID::setTotalLength(int total_length){
	this->totalLength = total_length;
}

