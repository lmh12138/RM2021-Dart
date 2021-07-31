/**
    * @file           : shoot.c
    * @brief          : 飞镖架发射系统
    * @version        : V1.0
    * @Author         : 李鸣航
    * @Date           : 2021-01-23 17:28
		* @LastEditor     : 陆柏臻
    * @LastEditTime   : 2021-07-17 22:36
    * @Note           : 
    *       1.本代码基于STM32F427IIHx开发，使用大疆Robomaster A型开发板，切勿使用其他型号开发板
    *       2.本代码仅适用于Robomaster2021飞镖架使用，不建议用作其他兵种
    * @Copyright(c)   : 哈尔滨工业大学（深圳）南工骁鹰战队 版权所有 Critical HIT copyrighted
    */
#include "shoot.h"
#include "robot.h"
#include "position.h"
#include "math.h"
#include "cmsis_os.h"
#include "tim.h"
#include "referee.h"

extern struct dart_t dart;
extern int auto_flag;
extern int auto_flag_mouse;
int dart_strike_flag = 0;
int change_dart_flag = 0;
int raise_frictiongear_speed_flag = 0;
int letdown_3510frictiongear_flag = 0;
int letdown_3510frictiongear_count = 0;
int raise_3510frictiongear_flag = 0;
int raise_3510frictiongear_count = 0;
int raise_3510frictiongear_waiting_flag = 0;
int speed_error;
int shoot_twice = 0;

/**
    * @brief  : 发射系统状态改变函数
    * @param   {*}
    * @return  {*}
    */
void ShootParamChange(void)
{
  if(dart.work_state == RemoteControl){    
			if(dart.work_state == STOP)
      {
          CanTransmit_1234(&hcan1, 0, 0, 0, 0);
          CanTransmit_5678(&hcan1, 0, 0, 0, 0);
          return;
      }
      else if(dart.strike_state == DEBUG)
      {
          SHOOT_3508_MOTOR1.speed_pid.ref = -dart.strike_speed / (PROPER_MOTOR12 + PROPER_MOTOR34 + PROPER_MOTOR56) * PROPER_MOTOR12;
          SHOOT_3508_MOTOR2.speed_pid.ref = dart.strike_speed / (PROPER_MOTOR12 + PROPER_MOTOR34 + PROPER_MOTOR56) * PROPER_MOTOR12;
          SHOOT_3508_MOTOR3.speed_pid.ref = -dart.strike_speed / (PROPER_MOTOR12 + PROPER_MOTOR34 + PROPER_MOTOR56) * PROPER_MOTOR34;
          SHOOT_3508_MOTOR4.speed_pid.ref = dart.strike_speed / (PROPER_MOTOR12 + PROPER_MOTOR34 + PROPER_MOTOR56) * PROPER_MOTOR34;
          SHOOT_3510_MOTOR1.speed_pid.ref = -dart.strike_speed / (PROPER_MOTOR12 + PROPER_MOTOR34 + PROPER_MOTOR56) * PROPER_MOTOR56;
          SHOOT_3510_MOTOR2.speed_pid.ref = dart.strike_speed / (PROPER_MOTOR12 + PROPER_MOTOR34 + PROPER_MOTOR56) * PROPER_MOTOR56;
          ShootParamCalculate();
          CanTransmit_1234(&hcan1, SHOOT_3510_MOTOR1.speed_pid.output, SHOOT_3510_MOTOR2.speed_pid.output, 
                                                            SHOOT_3508_MOTOR3.speed_pid.output, SHOOT_3508_MOTOR4.speed_pid.output);
          CanTransmit_5678(&hcan1, SHOOT_3508_MOTOR1.speed_pid.output, SHOOT_3508_MOTOR2.speed_pid.output, 0, 0);
      }
      else if(dart.strike_state == OUTPOST)
      {
          if(raise_frictiongear_speed_flag && dart.auto_strike == AUTO_READY && dart.dart_count > 0)
          {
              dart_strike_flag = AutoShoot();
          }
          if(dart.auto_strike == AUTO_STOP)
          {
              Remain2006TwoFrictiongearPosition();
          }
      }
    }
    else if(dart.work_state == MouseKeyControl){
        if(raise_frictiongear_speed_flag && dart.auto_strike == AUTO_READY && dart.dart_count > 0)
        {
            dart_strike_flag = AutoShoot();
        }
        if(dart.auto_strike == AUTO_STOP)
        {
            Remain2006TwoFrictiongearPosition();
        }
    }
}

/**
    * @brief  : 发射系统状态计算函数
    * @param   {*}
    * @return  {*}
    */
void ShootParamCalculate(void)
{
    SHOOT_3508_MOTOR1.speed_pid.fdb = SHOOT_3508_MOTOR1.fdbSpeed;
    SHOOT_3508_MOTOR2.speed_pid.fdb = SHOOT_3508_MOTOR2.fdbSpeed;
    SHOOT_3508_MOTOR3.speed_pid.fdb = SHOOT_3508_MOTOR3.fdbSpeed;
    SHOOT_3508_MOTOR4.speed_pid.fdb = SHOOT_3508_MOTOR4.fdbSpeed;
    SHOOT_3510_MOTOR1.speed_pid.fdb = SHOOT_3510_MOTOR1.fdbSpeed;
    SHOOT_3510_MOTOR2.speed_pid.fdb = SHOOT_3510_MOTOR2.fdbSpeed;
    PID_Calc(&SHOOT_3508_MOTOR1.speed_pid);
    PID_Calc(&SHOOT_3508_MOTOR2.speed_pid);
    PID_Calc(&SHOOT_3508_MOTOR3.speed_pid);
    PID_Calc(&SHOOT_3508_MOTOR4.speed_pid);
    PID_Calc(&SHOOT_3510_MOTOR1.speed_pid);
    PID_Calc(&SHOOT_3510_MOTOR2.speed_pid);
}

/**
    * @brief  : 飞镖架自动发射函数
    * @param   {*}
    * @return  {1：发射成功；0：发射失败}
    */
int change_dart_time = 0;

int AutoShoot(void)
{
    if(!change_dart_flag)
    {
        change_dart_flag = ChangeDart();
				change_dart_time = 10;
        return 0;
    }
		if(change_dart_time != 0){
			change_dart_time --;
			return 0;
		}
    if(!letdown_3510frictiongear_flag)
    {
        letdown_3510frictiongear_flag = Letdown3510Frictiongear();
        return 0;
    }
    if(!raise_3510frictiongear_flag)
    {
        if(!raise_3510frictiongear_waiting_flag)
        {
            osDelay(8);
            raise_3510frictiongear_waiting_flag = 1;
        }
        raise_3510frictiongear_flag = Raise3510Frictiongear();
        return 0;
    }
    else
    {
        dart.auto_strike = AUTO_STOP;
        dart.dart_count -= 1;
        change_dart_flag = 0;
        letdown_3510frictiongear_flag = 0;
        raise_3510frictiongear_flag = 0;
        raise_3510frictiongear_waiting_flag = 0;
        if(dart.dart_count == 2 && shoot_twice == 0){
            auto_flag = 0;
						auto_flag_mouse = 0;
            shoot_twice = 1;
						dart.auto_speed = AUTO_SPEED_OFF;
        }
        else{
            auto_flag = 1;
						auto_flag_mouse = 1;
        }
        return 1;
    }
}

/**
    * @brief  : 自动发射——开启/关闭动力系统
    * @param   {*}
    * @return  {1：到预设速度；0：未到预设速度}
    */
int RaiseFrictiongearSpeed(void)
{
    SHOOT_3508_MOTOR1.speed_pid.ref = -REFSPEED;
    SHOOT_3508_MOTOR2.speed_pid.ref = REFSPEED;
    SHOOT_3508_MOTOR3.speed_pid.ref = -REFSPEED;
    SHOOT_3508_MOTOR4.speed_pid.ref = REFSPEED;
    SHOOT_3510_MOTOR1.speed_pid.ref = -REFSPEED / 2.0;
    SHOOT_3510_MOTOR2.speed_pid.ref = REFSPEED / 2.0;
    SHOOT_3508_MOTOR1.speed_pid.fdb = SHOOT_3508_MOTOR1.fdbSpeed;
    SHOOT_3508_MOTOR2.speed_pid.fdb = SHOOT_3508_MOTOR2.fdbSpeed;
    SHOOT_3508_MOTOR3.speed_pid.fdb = SHOOT_3508_MOTOR3.fdbSpeed;
    SHOOT_3508_MOTOR4.speed_pid.fdb = SHOOT_3508_MOTOR4.fdbSpeed;
    SHOOT_3510_MOTOR1.speed_pid.fdb = SHOOT_3510_MOTOR1.fdbSpeed;
    SHOOT_3510_MOTOR2.speed_pid.fdb = SHOOT_3510_MOTOR2.fdbSpeed;
    PID_Calc(&SHOOT_3508_MOTOR1.speed_pid);
    PID_Calc(&SHOOT_3508_MOTOR2.speed_pid);
    PID_Calc(&SHOOT_3508_MOTOR3.speed_pid);
    PID_Calc(&SHOOT_3508_MOTOR4.speed_pid);
    PID_Calc(&SHOOT_3510_MOTOR1.speed_pid);
    PID_Calc(&SHOOT_3510_MOTOR2.speed_pid);
    CanTransmit_1234(&hcan1, SHOOT_3510_MOTOR1.speed_pid.output, SHOOT_3510_MOTOR2.speed_pid.output, 
                                                        SHOOT_3508_MOTOR3.speed_pid.output, SHOOT_3508_MOTOR4.speed_pid.output);
    CanTransmit_5678(&hcan1, SHOOT_3508_MOTOR1.speed_pid.output, SHOOT_3508_MOTOR2.speed_pid.output, 0, 0);
    speed_error =
                (fabs(SHOOT_3508_MOTOR1.speed_pid.ref - SHOOT_3508_MOTOR1.speed_pid.fdb) < RAISE_FRICTIONGEAR_SPEED_ERROR) && \
                (fabs(SHOOT_3508_MOTOR2.speed_pid.ref - SHOOT_3508_MOTOR2.speed_pid.fdb) < RAISE_FRICTIONGEAR_SPEED_ERROR) && \
                (fabs(SHOOT_3508_MOTOR3.speed_pid.ref - SHOOT_3508_MOTOR3.speed_pid.fdb) < RAISE_FRICTIONGEAR_SPEED_ERROR) && \
                (fabs(SHOOT_3508_MOTOR4.speed_pid.ref - SHOOT_3508_MOTOR4.speed_pid.fdb) < RAISE_FRICTIONGEAR_SPEED_ERROR) && \
                (fabs(SHOOT_3510_MOTOR1.speed_pid.ref - SHOOT_3510_MOTOR1.speed_pid.fdb) < RAISE_FRICTIONGEAR_SPEED_ERROR) && \
                (fabs(SHOOT_3510_MOTOR2.speed_pid.ref - SHOOT_3510_MOTOR2.speed_pid.fdb) < RAISE_FRICTIONGEAR_SPEED_ERROR);
    if(!speed_error)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
    * @brief  : 自动发射——引导飞镖
    * @param   {*}
    * @return  {1：引导成功；0：引导失败}
    */
int Letdown3510Frictiongear(void)
{
		//使用碰撞开关,高电平触发
		if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_11) != GPIO_PIN_SET){	//没到底
        POSITION_2006_MOTOR2.speed_pid.ref = -LETDOWN_2006_SPEED;
        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
				return 0;
		}
		else{
        POSITION_2006_MOTOR2.speed_pid.ref = 0;
        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
				if(POSITION_2006_MOTOR2.fdbSpeed != 0){	//到底了但电机还没停下来
							return 0;
				}
				else{
							return 1;
				}
		}
//		//向下运行一段时间
//    if(letdown_3510frictiongear_count < LETDOWN_2006_TIME)
//    {
//        POSITION_2006_MOTOR2.speed_pid.ref = -LETDOWN_2006_SPEED;
//        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
//        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
//        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
//        letdown_3510frictiongear_count++;
//        return 0;
//    }
//    else if(POSITION_2006_MOTOR2.fdbSpeed != 0)
//    {
//        POSITION_2006_MOTOR2.speed_pid.ref = 0;
//        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
//        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
//        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
//        return 0;
//    }
//    else
//    {
//        letdown_3510frictiongear_count = 0;
//        return 1;
//    }
}

/**
    * @brief  : 自动发射——结束引导
    * @param   {*}
    * @return  {1：结束成功；0：结束失败}
    */
int Raise3510Frictiongear(void)
{		
	//使用碰撞开关,高电平触发
		if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_10) == GPIO_PIN_RESET){	//没到顶
        POSITION_2006_MOTOR2.speed_pid.ref = RAISE_2006_SPEED;
        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
				return 0;
		}
		else{
        POSITION_2006_MOTOR2.speed_pid.ref = 0;
        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
				if(POSITION_2006_MOTOR2.fdbSpeed != 0){	//到顶了但电机还没停下来
							return 0;
				}
				else{
							return 1;
				}
		}
//		//向上运行一段时间
//    if(raise_3510frictiongear_count < RAISE_2006_TIME)
//    {
//        POSITION_2006_MOTOR2.speed_pid.ref = RAISE_2006_SPEED;
//        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
//        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
//        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
//        raise_3510frictiongear_count++;
//        return 0;
//    }
//    else if(POSITION_2006_MOTOR2.fdbSpeed != 0)
//    {
//        POSITION_2006_MOTOR2.speed_pid.ref = 0;
//        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
//        PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
//        CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
//        return 0;   
//    }
//    else
//    {
//        raise_3510frictiongear_count = 0;
//        return 1;
//    }
}

/**
    * @brief  : 自动发射——重新填充飞镖
    * @param   {*}
    * @return  {1：填充完成；0：填充尚未完成}
    */
int ChangeDart(void)
{
    //舵机版
    #ifdef SERVO
        switch (dart.dart_count)
        {
        case 4:
            TIM5 -> CCR4 = DART_POSITION_4;
            //osDelay(100);
            break;

        case 3:
            TIM5 -> CCR4 = DART_POSITION_3;
            //osDelay(100);
            break;

        case 2:
            TIM5 -> CCR4 = DART_POSITION_2;
            //osDelay(100);
            break;

        case 1:
            TIM5 -> CCR4 = DART_POSITION_1;
            //osDelay(100);
            break;
        }
        return 1;
    #endif

    //电机版
    #ifndef SERVO
        POSITION_3508_MOTOR.position_pid.ref = dart.position_3508_bias + (DART_BIAS_COUNT - dart.dart_count) * DELTA;
        POSITION_2006_MOTOR2.speed_pid.ref = 0;
        if(POSITION_3508_MOTOR.position_pid.ref > WHOLE)
        {
            POSITION_3508_MOTOR.position_pid.ref -= WHOLE;
        }
        else if(POSITION_3508_MOTOR.position_pid.ref < -WHOLE)
        {
            POSITION_3508_MOTOR.position_pid.ref += WHOLE;
        }
        POSITION_3508_MOTOR.position_pid.fdb = (float)POSITION_3508_MOTOR.real_position;
        POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
        if( fabs(POSITION_3508_MOTOR.position_pid.ref - POSITION_3508_MOTOR.position_pid.fdb) <= CHANGE_DART_ERROR && POSITION_3508_MOTOR.fdbSpeed != 0)
        {
            POSITION_3508_MOTOR.speed_pid.ref = 0;
            POSITION_3508_MOTOR.speed_pid.fdb = POSITION_3508_MOTOR.fdbSpeed;
            PID_Calc(&POSITION_3508_MOTOR.speed_pid);
            PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
            CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, POSITION_3508_MOTOR.speed_pid.output, 0);
            return 0;
        }
        else if( fabs(POSITION_3508_MOTOR.position_pid.ref - POSITION_3508_MOTOR.position_pid.fdb) > CHANGE_DART_ERROR)
        {
            PID_Calc(&POSITION_3508_MOTOR.position_pid);
            POSITION_3508_MOTOR.speed_pid.ref = POSITION_3508_MOTOR.position_pid.output;
            POSITION_3508_MOTOR.speed_pid.fdb = POSITION_3508_MOTOR.fdbSpeed;
            PID_Calc(&POSITION_3508_MOTOR.speed_pid);
            PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
            CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, POSITION_3508_MOTOR.speed_pid.output, 0);
            return 0;
        }
        else
        {
            return 1;
        }
    #endif
}

/**
    * @brief  : 自动发射——保持引导系统位置不变
    * @param   {*}
    * @return  {*}
    */
void Remain2006TwoFrictiongearPosition(void)
{
    POSITION_2006_MOTOR2.speed_pid.ref = 0;
    POSITION_2006_MOTOR2.speed_pid.fdb = POSITION_2006_MOTOR2.fdbSpeed;
    PID_Calc(&POSITION_2006_MOTOR2.speed_pid);
    CanTransmit_1234(&hcan2, 0, POSITION_2006_MOTOR2.speed_pid.output, 0, 0);
}

/**
    * @brief  : 自动发射——保持动力系统速度
    * @param   {*}
    * @return  {*}
    */
void Remain35083510FrictiongearSpeed(void)
{
    if(dart.work_state == STOP)
    {
        CanTransmit_1234(&hcan1, 0, 0, 0, 0);
        CanTransmit_5678(&hcan1, 0, 0, 0, 0);
    }
    else if(dart.strike_state != DEBUG && dart.auto_speed == AUTO_SPEED_ON && dart.dart_count > 0)
    {
        raise_frictiongear_speed_flag = RaiseFrictiongearSpeed();
    }
    else if(dart.dart_count == 0)
    {
        SHOOT_3508_MOTOR1.speed_pid.ref = 0;
        SHOOT_3508_MOTOR2.speed_pid.ref = 0;
        SHOOT_3508_MOTOR3.speed_pid.ref = 0;
        SHOOT_3508_MOTOR4.speed_pid.ref = 0;
        SHOOT_3510_MOTOR1.speed_pid.ref = 0;
        SHOOT_3510_MOTOR2.speed_pid.ref = 0;
        ShootParamCalculate();
        CanTransmit_1234(&hcan1, SHOOT_3510_MOTOR1.speed_pid.output, SHOOT_3510_MOTOR2.speed_pid.output, 
                                                        SHOOT_3508_MOTOR3.speed_pid.output, SHOOT_3508_MOTOR4.speed_pid.output);
        CanTransmit_5678(&hcan1, SHOOT_3508_MOTOR1.speed_pid.output, SHOOT_3508_MOTOR2.speed_pid.output, 0, 0);
    }
}
