/**
    * @file           : robot.c
    * @brief          : 飞镖架整体初始化及状态改变
    * @version        : V1.0
    * @Author         : 李鸣航
    * @Date           : 2021-01-22 23:17
    * @LastEditTime   : 2021-05-09 21:50
    * @Note           : 
    *       1.本代码基于STM32F427IIHx开发，使用大疆Robomaster A型开发板，切勿使用其他型号开发板
    *       2.本代码仅适用于Robomaster2021飞镖架使用，不建议用作其他兵种
    * @Copyright(c)   : 哈尔滨工业大学（深圳）南工骁鹰战队 版权所有 Critical HIT copyrighted
    */

#include "robot.h"
#include <string.h>
#include "tim.h"
#include "cmsis_os.h"
#include "referee.h"

struct dart_t dart;
extern struct DT7Remote_t Remote;
extern ext_dart_client_cmd_t dart_client_cmd_t;
extern uint16_t last_operate_launch_cmd_time;

int auto_flag = 0;

/**
    * @brief  : 飞镖架参数初始化
    * @param   {*}
    * @return  {*}
    */
void DartParamInit(void)
{
    dart.dart_roll.RollBiasAngle = ROLL_BIAS_ANGLE;     //中心初始化
    dart.strike_speed = 0;
    dart.dart_count = 4;
    memset(&SHOOT_3508_MOTOR1, 0, sizeof(SHOOT_3508_MOTOR1));
    memset(&SHOOT_3508_MOTOR2, 0, sizeof(SHOOT_3508_MOTOR2));
    memset(&SHOOT_3508_MOTOR3, 0, sizeof(SHOOT_3508_MOTOR3));
    memset(&SHOOT_3508_MOTOR4, 0, sizeof(SHOOT_3508_MOTOR4));
    memset(&SHOOT_3510_MOTOR1, 0, sizeof(SHOOT_3510_MOTOR1));
    memset(&SHOOT_3510_MOTOR2, 0, sizeof(SHOOT_3510_MOTOR2));
    memset(&POSITION_2006_MOTOR1, 0, sizeof(POSITION_2006_MOTOR1));
    memset(&POSITION_2006_MOTOR2, 0, sizeof(POSITION_2006_MOTOR2));
    memset(&POSITION_3508_MOTOR, 0, sizeof(POSITION_3508_MOTOR));

    MotorParamInitFW(&SHOOT_3508_MOTOR1, 2.0, 1.2, 0.015, 0.01, 500, 75, 700, 131072, 16384, 0,0,0,0,0,0,0,0,0);
    MotorParamInitFW(&SHOOT_3508_MOTOR2, 2.0, 1.2, 0.015, 0.01, 500, 75, 700, 131072, 16384, 0,0,0,0,0,0,0,0,0);
    MotorParamInitFW(&SHOOT_3508_MOTOR3, 2.0, 1.2, 0.015, 0.01, 500, 75, 700, 131072, 16384, 0,0,0,0,0,0,0,0,0);
    MotorParamInitFW(&SHOOT_3508_MOTOR4, 2.0, 1.2, 0.015, 0.01, 500, 75, 700, 131072, 16384, 0,0,0,0,0,0,0,0,0);
    MotorParamInitFW(&SHOOT_3510_MOTOR1, 2.0, 1.2, 0.015, 0.01, 500, 75, 700, 131072, 16384, 0,0,0,0,0,0,0,0,0);
    MotorParamInitFW(&SHOOT_3510_MOTOR2, 2.0, 1.2, 0.015, 0.01, 500, 75, 700, 131072, 16384, 0,0,0,0,0,0,0,0,0);
    MotorParamInit(&POSITION_2006_MOTOR1, 20,0,0,0,8000,  0,0,0,0,0);
    MotorParamInit(&POSITION_2006_MOTOR2, 5,0,0,0,8000,  0.1,0,0,5000,5000);
    MotorParamInit(&POSITION_3508_MOTOR,  5,0,0,2048,16000,  0.4,0,0,5000,10000);
    
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
		TIM5 -> CCR4 = 590;
    ActParamInit();
}

/**
    * @brief  : 电动推杆初始化
    * @param   {*}
    * @return  {*}
    */
void ActParamInit(void)
{
    HAL_TIM_PWM_Start(&htimx, TIM_CHANNEL_x);
    TIMx -> CCRx = 10000 - 1;
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x2, GPIO_PIN_RESET);
}

/**
    * @brief  : 飞镖架状态改变
    * @param   {*}
    * @return  {*}
    */
void DartStateChange(void)
{
    dart.last_work_state = dart.work_state;
    if(dart.position_3508_bias == 0)
    {
        dart.position_3508_bias = POSITION_3508_MOTOR.fdbPosition;
    }
    if(dart.position_2006_2_bias == 0)
    {
        dart.position_2006_2_bias = POSITION_2006_MOTOR2.fdbPosition;
    }
    if(dart.last_work_state == STOP)
    {
        dart.dart_roll.RollAngle = ROLL_BIAS_ANGLE;
    }

    if(Remote.inputmode == RC_Stop || Is_Error(1 << RC_INDEX))
    {
        dart.work_state = STOP;
        SHOOT_3508_MOTOR1.speed_pid.output = 0;
        SHOOT_3508_MOTOR2.speed_pid.output = 0;
        SHOOT_3508_MOTOR3.speed_pid.output = 0;
        SHOOT_3508_MOTOR4.speed_pid.output = 0;
        SHOOT_3510_MOTOR1.speed_pid.output = 0;
        SHOOT_3510_MOTOR2.speed_pid.output = 0;
        POSITION_2006_MOTOR1.speed_pid.output = 0;
        POSITION_2006_MOTOR2.speed_pid.output = 0;
        POSITION_3508_MOTOR.speed_pid.output = 0;
        return;
    }
    else if(Remote.inputmode == RC_Remote)
    { 
        dart.work_state = RemoteControl;
    }
    else if(Remote.inputmode == RC_MouseKey)
    {
        dart.work_state = MouseKeyControl;
    }

    if(dart.work_state == RemoteControl)
    {    
				dart.last_strike_state = dart.strike_state;
        if(Remote.rc.s1 == 1) //左上角switch->上
        {
            dart.strike_state = DEBUG;
						if(dart.last_strike_state != DEBUG){
							dart.dart_count = 4;
						}
        }
        else if(Remote.rc.s1 == 3) //左上角switch->中
        {
            dart.strike_state = OUTPOST;
            //ch1打到最上再回中且ch2在中位，开始执行自动发射任务
            if(Remote.rc.ch1 == 1684){
              if(Remote.rc.ch2 != 1024){ //ch2不在中位，什么都不做
                  dart.auto_speed = AUTO_SPEED_OFF;
                  auto_flag = 0;
              }
              else{   //ch2在中位
                  if(auto_flag == 0){   //ch1:中->上
                      auto_flag = 1;
                  }
              }
            }
            else if(Remote.rc.ch1 == 1024 && auto_flag == 1){   //ch1:上->中
              auto_flag = 0;
              if(dart.auto_strike == AUTO_STOP){
                  dart.auto_strike = AUTO_READY;    //可以发射
                  dart.auto_speed = AUTO_SPEED_ON;  //启动摩擦轮
              }
            }
        }
        else  //左上switch->下
        {
            dart.strike_state = BASE;
        }
        ShootCtrl_Remote();
        PositionCtrl_Remote();
    }
    else if(dart.work_state == MouseKeyControl)
    {
        if(dart_client_cmd_t.dart_launch_opening_status == 0 && dart_client_cmd_t.operate_launch_cmd_time != last_operate_launch_cmd_time){
            dart.auto_strike = AUTO_READY;    //可以发射
            dart.auto_speed = AUTO_SPEED_ON;  //启动摩擦轮
        }
        ShootCtrl_MouseKey();
        PositionCtrl_MouseKey();
    }
}

