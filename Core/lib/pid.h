#ifndef PID_H_
#define PID_H_

enum PID_Mode_e
{
	PID_POSITION = 0,
	PID_DELTA
};

struct PID_t
{
	float KP;
	float KI;
	float KD;
	float error[3];
	float error_sum;
	float error_max;
	float fdb;
	float ref;
	float output;
	float outputMax;
	enum PID_Mode_e PID_Mode;
};

#define DEFAULT_PID \
{0.0f,0.0f,0.0f,{0.0f,0.0f,0.0f},0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,PID_POSITION \
}

void PID_Calc(struct PID_t *pid);

#endif
