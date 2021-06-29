/**
  ******************************************************************************
  * 文件名        : remote.c
	* 文件描述      ：PID控制算法
  * 创建时间      ：2019.11.9
	* 作者          ：刘文熠
  *-----------------------------------------------------------------------------
  * 最近修改时间  ：2019.11.9
	* 修改人        ：刘文熠
  ******************************************************************************
  * 1.本代码基于STMF427IIT6开发，编译环境为Keil 5，基于FreeRTOS进行开发。
  * 2.本代码只适用于RoboMaster机器人，不建议用于其他用途
	* 3.本代码包含大量中文注释，请以UTF-8编码格式打开
	* 4.本代码最终解释权归哈尔滨工业大学（深圳）南工骁鹰战队Critical HIT所有
	* 
	* Copyright (c) 哈尔滨工业大学（深圳）南工骁鹰战队Critical HIT 版权所有
  ******************************************************************************
  */
	
#include "pid.h"
/**
	* @brief PID计算函数，位置式和增量式合在一起
	* @param PID结构体
	* @retval None
	*/
void PID_Calc(struct PID_t *pid)
{
	pid->error[2] = pid->error[1];       //上上次误差
	pid->error[1] = pid->error[0];       //上次误差
	pid->error[0] = pid->ref - pid->fdb; //本次误差
	
	if(pid->PID_Mode == PID_POSITION)    //位置式PID
	{
		pid->error_sum += pid->error[0];       //积分上限判断
		if(pid->error_sum > pid->error_max) pid->error_sum = pid->error_max;
		if(pid->error_sum < -pid->error_max) pid->error_sum = -pid->error_max;
		
		pid->output = pid->KP*pid->error[0] + pid->KI*pid->error_sum+pid->KD*(pid->error[0]-pid->error[1]);
	}
	
	else if(pid->PID_Mode == PID_DELTA)  //增量式PID
	{
		pid->output += pid->KP*(pid->error[0]-pid->error[1]) + pid->KI*(pid->error[0]-2.0f*pid->error[1]+pid->error[2]) + pid->KI*pid->error[0];
	}
	
	/* 输出上限 */
	if(pid->output > pid->outputMax) pid->output = pid->outputMax;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax;
}
