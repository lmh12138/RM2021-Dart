/**
    * @file           : robot.h
    * @brief          : 飞镖架整体初始化及状态改变
    * @version        : V1.0
    * @Author         : 李鸣航
    * @Date           : 2021-01-22 23:17
    * @LastEditTime   : 2021-06-29 14:39
    * @Note           : 
    *       1.本代码基于STM32F427IIHx开发，使用大疆Robomaster A型开发板，切勿使用其他型号开发板
    *       2.本代码仅适用于Robomaster2021飞镖架使用，不建议用作其他兵种
    * @Copyright(c)   : Critical HIT copyrighted
    */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "motor.h"
#include "bsp_can.h"
#include "remote.h"
#include "position.h"
#include "shoot.h"
#include "supervise.h"
#include "referee.h"

extern struct CAN_Motor can1_motor_1;
extern struct CAN_Motor can1_motor_2;
extern struct CAN_Motor can1_motor_3;
extern struct CAN_Motor can1_motor_4;
extern struct CAN_Motor can1_motor_5;
extern struct CAN_Motor can1_motor_6;
extern struct CAN_Motor can1_motor_7;
extern struct CAN_Motor can1_motor_9;

extern struct CAN_Motor can2_motor_1;
extern struct CAN_Motor can2_motor_2;
extern struct CAN_Motor can2_motor_3;
extern struct CAN_Motor can2_motor_4;
extern struct CAN_Motor can2_motor_5;
extern struct CAN_Motor can2_motor_6;
extern struct CAN_Motor can2_motor_7;
extern struct CAN_Motor can2_motor_9;

/*-----发射系统的六个电机-----*/
#define SHOOT_3510_MOTOR1 can1_motor_1
#define SHOOT_3510_MOTOR2 can1_motor_2
#define SHOOT_3508_MOTOR3 can1_motor_3
#define SHOOT_3508_MOTOR4 can1_motor_4
#define SHOOT_3508_MOTOR1 can1_motor_5
#define SHOOT_3508_MOTOR2 can1_motor_6

/*-----位置系统的三个电机-----*/
#define POSITION_2006_MOTOR1 can2_motor_1
#define POSITION_2006_MOTOR2 can2_motor_2
#define POSITION_3508_MOTOR can2_motor_3

/*-----电动推杆使用的TIM时钟口以及两个GPIO输出的口-----*/
#define TIMx TIM5
#define htimx htim5
#define TIM_CHANNEL_x TIM_CHANNEL_3
#define CCRx CCR3
#define GPIOx GPIOF
#define GPIO_PIN_x1 GPIO_PIN_0
#define GPIO_PIN_x2 GPIO_PIN_1

/*-----飞镖架roll轴初始中心角度数据（自行测量）-----*/
#define ROLL_BIAS_ANGLE 728

/*-----控制模式-----*/
enum work_state_e
{
    STOP,
    RemoteControl,
    MouseKeyControl
};

/*-----飞镖架打击状态-----*/
enum strike_state_e
{
    DEBUG,
    OUTPOST,
    BASE
};

/*-----飞镖架自动打击进程-----*/
enum auto_strike_e
{
    AUTO_STOP,
    AUTO_READY
};

/*-----飞镖架自动加速进程-----*/
enum auto_speed_e
{
    AUTO_SPEED_OFF, //关闭摩擦轮
    AUTO_SPEED_ON   //开启摩擦轮
};

/*-----电动推杆升降模式-----*/
enum act_state_e
{
    ACTUP,
    ACTDOWN
};

/*-----飞镖架roll轴电机参数-----*/
struct dart_roll_t
{
    float RollAngle;
    float RollBiasAngle;
    float RollSpeed;
};

/*-----飞镖架状态结构体-----*/
struct dart_t
{
    //模式
    enum work_state_e work_state;
    enum work_state_e last_work_state;
    enum strike_state_e strike_state;
    enum act_state_e act_state;
    enum auto_strike_e auto_strike;
    enum auto_speed_e auto_speed;
    
    //单套摩擦轮转速
    float friction_speed;

    //飞镖射出速度
    float strike_speed;

    //位置参数
    int position_3508_bias;
    int position_2006_2_bias;
    struct dart_roll_t dart_roll;
    float height_angle;
    float level_angle;
    float level_speed;
    float translation_speed;
    float height_speed;
    
    //剩余飞镖量
    int dart_count;
};

/*-----电动推杆初始化-----*/
extern void ActParamInit(void);

/*-----飞镖架初始化-----*/
extern void DartParamInit(void);

/*-----用户端对飞镖架整体状态的改变-----*/
extern void DartStateChange(void);


#endif
