#include "robot.h"
#include "shoot.h"

extern struct dart_t dart;

void ShootCtrl_Remote(void)
{
    #ifdef IS_DIRECT_CTRL
    //直接控制
    dart.strike_speed = DIRECT_CTRL_GAIN * (float)(Remote.rc.ch3 - CHx_BIAS);
    #else
    //增量控制
    dart.strike_speed += INCREMENT_CTRL_GAIN * (float)(Remote.rc.ch3 - CHx_BIAS);
    #endif
}

void ShootCtrl_MouseKey(void)
{
    ;
}