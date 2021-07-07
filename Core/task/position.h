/**
    * @file           : position.h
    * @brief          : 飞镖架yaw、pitch以及飞镖本身roll轴的位置读取及计算
    * @version        : V1.0.0
    * @Author         : 李鸣航
    * @Date           : 2021-01-22 2:17
    * @LastEditTime   : 2021-06-29 14:38
    * @Note           : 
    * @Copyright(c)   : Critical HIT copyrighted
    */

#ifndef _POSITION_H_
#define _POSITION_H_

#include "motor.h"
#include "bsp_can.h"

/*-----用户端对位置系统的状态控制函数-----*/
void PositionCtrl_Remote(void);
void PositionCtrl_MouseKey(void);

/*-----外设层对状态数据的解析以及发送函数-----*/
void PositionParamChange(void);
void PositionParamCalculate(void);

#endif