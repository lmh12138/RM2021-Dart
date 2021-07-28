#include "position.h"
#include "robot.h"
#include "math.h"
#include "shoot.h"

extern struct dart_t dart;

double transform = 10000 / (1684 - 1024);

void PositionCtrl_Remote(void)
{
    #ifdef SERVO
		static int tmp = 0;
		if(Remote.rc.ch2 - CHx_BIAS > 0)tmp = 1;
		else if(Remote.rc.ch2 - CHx_BIAS < 0)tmp = -1;
		else{
			dart.dart_count += tmp;
			tmp = 0;
		}
    //dart.dart_roll.RollSpeed = 0.05f * (float)(Remote.rc.ch2 - CHx_BIAS);
    #endif

    #ifndef SERVO
    dart.dart_roll.RollSpeed = 4.0f * (float)(Remote.rc.ch2 - CHx_BIAS);
    #endif
    dart.level_speed = -8.0f * (float)(Remote.rc.ch0 - CHx_BIAS);
    dart.translation_speed = -16.0f * (float)(Remote.rc.ch4 - CHx_BIAS);
    dart.height_speed = fabs(transform * (float)(Remote.rc.ch1 - CHx_BIAS));
    if(Remote.rc.ch1 < CHx_BIAS)
    {
        dart.act_state = ACTDOWN;
    } 
    else
    {
        dart.act_state = ACTUP;
    }
}

void PositionCtrl_MouseKey(void)
{
    ;
}


