/**
    * @file           : shoot.h
    * @brief          : 飞镖架的六个摩擦轮组成的发射系统
    * @version        : V1.0
    * @Author         : 李鸣航
    * @Date           : 2021-01-23 17:28
    * @LastEditTime   : 2021-05-23 21:07
    * @Note           : 
    *       1.本代码基于STM32F427IIHx开发，使用大疆Robomaster A型开发板，切勿使用其他型号开发板
    *       2.本代码仅适用于Robomaster2021飞镖架使用，不建议用作其他兵种
    * @Copyright(c)   : 哈尔滨工业大学（深圳）南工骁鹰战队 版权所有 Critical HIT copyrighted
    */
#ifndef _SHOOT_H_
#define _SHOOT_H_

#include "motor.h"
#include "bsp_can.h"

/*-----飞镖初始数量-----*/
#define DART_BIAS_COUNT 4

/*-----电机模式下一整圈的编码器值以及一次应该转的编码器值*/
#define WHOLE (8192 * 3591 / 187.0)
#define DELTA (8192 * 3591 / 187.0 / 4.0)

/*-----自动发射时的一些预期参数-----*/
#define REFSPEED 6000                       //摩擦轮预期转速
#define LETDOWN_2006_SPEED 2000             //2006下降速度
#define LETDOWN_2006_TIME 192               //2006下降控制时间
#define RAISE_2006_SPEED 2000               //2006上升速度
#define RAISE_2006_TIME 255               //2006上升控制时间
#define CHANGE_DART_ERROR 1000              //电机模式下填充飞镖时的预期误差值
#define RAISE_FRICTIONGEAR_SPEED_ERROR 50   //摩擦轮预期误差值

/*-----舵机模式下一次应该转的CCR值-----*/
#define DELTA_SERVO 666.667

/*-----舵机模式下四发飞镖对应的位置（注意是剩余几发）-----*/
#define DART_POSITION_1 2500 //580
#define DART_POSITION_2 1850
#define DART_POSITION_3 1215
#define DART_POSITION_4 590

/*-----两种控制模式的增益-----*/
#define DIRECT_CTRL_GAIN 22.0f
#define INCREMENT_CTRL_GAIN 5.0f

/*-----默认控制模式，可通过注释宏定义改变控制模式-----*/
#define IS_DIRECT_CTRL

/*-----飞镖架三套摩擦轮对速度影响权重（自行测量）-----*/
#define PROPER_MOTOR12 0.4
#define PROPER_MOTOR34 0.4
#define PROPER_MOTOR56 0.2

/*-----舵机/电机模式切换-----*/
#define SERVO

/*-----用户端对发射系统的状态控制函数-----*/
void ShootCtrl_Remote(void);
void ShootCtrl_MouseKey(void);

/*-----外设层对状态数据的解析以及发送函数-----*/
void ShootParamChange(void);
void ShootParamCalculate(void);

/*-----自动发射的所有函数-----*/
int AutoShoot(void);                            //总函数
int ChangeDart(void);                           //填充飞镖
int RaiseFrictiongearSpeed(void);               //摩擦轮加速
int Letdown3510Frictiongear(void);              //降下引导电机
int Raise3510Frictiongear(void);                //升起引导电机
void Remain2006TwoFrictiongearPosition(void);   //保持2006位置
void Remain35083510FrictiongearSpeed(void);     //保持摩擦轮速度

#endif
