/**
    * @file           : position.c
    * @brief          : 飞镖架yaw、pitch以及飞镖本身roll轴的位置读取及计算
    * @version        : V1.0
    * @Author         : 李鸣航
    * @Date           : 2021-01-22 02:17
    * @LastEditTime   : 2021-05-22 14:53
    * @Note           : 
    * @Copyright(c)   : 哈尔滨工业大学（深圳）南工骁鹰战队版权所有 Critical HIT copyrighted
    */

#include <time.h>
#include "position.h"
#include "robot.h"
#include "shoot.h"

#define REDU_RATIO  172

uint16_t i;
extern struct dart_t dart;
extern struct DT7Remote_t Remote;
extern int auto_flag;

/**
    * @brief  : 位置系统状态改变函数
    * @param   {*}
    * @return  {*}
    */
void PositionParamChange(void)
{
    if(dart.work_state == STOP)
    {
        // CanTransmit_1234(&hcan2, 0, 0, POSITION_3508_MOTOR.speed_pid.output, 0);
        CanTransmit_1234(&hcan2, 0, 0, 0, 0);
        TIMx -> CCRx = 10000 - 1;
        HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x2, GPIO_PIN_RESET);
        return;
    }
    else if(dart.work_state == RemoteControl)
    {
        if(dart.strike_state == DEBUG)
        {
            #ifdef SERVO
						ChangeDart();
            //TIM5 -> CCR4 += dart.dart_roll.RollSpeed;
						
            #endif
            
            #ifndef SERVO
            POSITION_3508_MOTOR.speed_pid.ref = dart.dart_roll.RollSpeed;
            #endif
            
            POSITION_2006_MOTOR1.speed_pid.ref = dart.level_speed;
            POSITION_2006_MOTOR2.speed_pid.ref = dart.translation_speed;
            PositionParamCalculate();
            CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 
                                                    POSITION_3508_MOTOR.speed_pid.output, 0);
            if(dart.act_state == ACTUP)
            {
                HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x1, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x2, GPIO_PIN_SET);
                TIMx -> CCRx = dart.height_speed;
            }
            else if(dart.act_state == ACTDOWN)
            {
                HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x1, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x2, GPIO_PIN_RESET);
                TIMx -> CCRx = dart.height_speed;
            }
        }
        else if(dart.strike_state == OUTPOST)
        {
            ;
        }
        else if(dart.strike_state == BASE)
        {
            ;
        }
    }
    else if(dart.work_state == MouseKeyControl)
    {
        ;
    }
}

/**
    * @brief  : 发射系统状态计算函数
    * @param   {*}
    * @return  {*}
    */
void PositionParamCalculate(void)
{
    // POSITION_3508_MOTOR.position_pid.fdb = (float)POSITION_3508_MOTOR.real_position;
    // PID_Calc(&POSITION_3508_MOTOR.position_pid);
    // POSITION_3508_MOTOR.speed_pid.ref = POSITION_3508_MOTOR.position_pid.output;
    // POSITION_3508_MOTOR.speed_pid.fdb = POSITION_3508_MOTOR.fdbSpeed;
    // PID_Calc(&POSITION_3508_MOTOR.speed_pid);
    POSITION_2006_MOTOR1.speed_pid.fdb = POSITION_2006_MOTOR1.fdbSpeed;
    POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
    POSITION_3508_MOTOR.speed_pid.fdb = POSITION_3508_MOTOR.fdbSpeed;
    PID_Calc(&POSITION_2006_MOTOR1.speed_pid);
    PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
    PID_Calc(&POSITION_3508_MOTOR.speed_pid);
}
