#include "remote_decoder.h"
#include "Filter.h"
#include "Reset.h"
uint8_t rc_data[18];
struct DT7Remote Remote;
uint8_t reset_cnt=0;
// 目前是在bsp_usart里面，接收到串口数据就直接解码，没有遇到问题，之后可以考虑串口中断只接收
void remote_decoder(void)
{
    Remote.last_rc = Remote.rc;
    Remote.last_key = Remote.key;
    Remote.last_mouse = Remote.mouse;

    Remote.rc.ch0 = ((int16_t)rc_data[0] | ((int16_t)rc_data[1] << 8)) & 0x07FF;
    Remote.rc.ch1 = (((int16_t)rc_data[1] >> 3) | ((int16_t)rc_data[2] << 5)) & 0x07FF;
    Remote.rc.ch2 = (((int16_t)rc_data[2] >> 6) | ((int16_t)rc_data[3] << 2) | ((int16_t)rc_data[4] << 10)) & 0x07FF;
    Remote.rc.ch3 = (((int16_t)rc_data[4] >> 1) | ((int16_t)rc_data[5] << 7)) & 0x07FF;
    Remote.rc.ch4 = ((int16_t)rc_data[16] | ((int16_t)rc_data[17] << 8)) & 0x07FF;

    Remote.rc.s1 = ((rc_data[5] >> 4) & 0x000C) >> 2;
    Remote.rc.s2 = ((rc_data[5] >> 4) & 0x0003);

    Remote.mouse.x = ((int16_t)rc_data[6]) | ((int16_t)rc_data[7] << 8);
    Remote.mouse.y = ((int16_t)rc_data[8]) | ((int16_t)rc_data[9] << 8);
    Remote.mouse.z = ((int16_t)rc_data[10]) | ((int16_t)rc_data[11] << 8);
	  Remote.mouse.decorder_x = Ramp_16t(Remote.mouse.x,Remote.last_mouse.x);

    Remote.mouse.press_l = rc_data[12];
    Remote.mouse.press_r = rc_data[13];

    Remote.key.val = ((int16_t)rc_data[14]) | ((int16_t)rc_data[15] << 8);



    if (Remote.rc.s2 == 2)
        Remote.inputmode = RC_Stop;
    else if (Remote.rc.s2 == 3)
        Remote.inputmode = RC_Remote;
    else if (Remote.rc.s2 == 1)
        Remote.inputmode = RC_MouseKey;
		
    //遥控器14ms发一次消息，按住组合键3000毫秒 即计数3000/14=215
    if((Remote.key.val & Key_ECtrl )&& (Remote.key.val & Key_G))
		{
			reset_cnt++;
			if(reset_cnt >=215)
			{
				Remote.inputmode= RC_Stop;
				//重启
				soft_reset();
				
			}
		}
		else 
		{
			reset_cnt=0;//清零
		}
		
    if (Remote.inputmode != RC_Stop)
    {
			  
        if (((Remote.key.val & Key_Z) == (1 << 11) && (Remote.last_key.val & Key_Z) == 0))
            Remote.key.z=!Remote.key.z;
        if (((Remote.key.val & Key_X) == (1 << 12) && (Remote.last_key.val & Key_X) == 0))
            Remote.key.x=!Remote.key.x;
				if(((Remote.key.val & Key_F) == (1 << 9) && (Remote.last_key.val & Key_F) == 0))
            Remote.key.f=!Remote.key.f;
				if(((Remote.key.val & Key_R) == (1 << 8) && (Remote.last_key.val & Key_R) == 0))
            Remote.key.r=!Remote.key.r;
				if(((Remote.key.val & Key_C) == (1 << 13) && (Remote.last_key.val & Key_C) == 0))
            Remote.key.c=!Remote.key.c;
				if(Remote.mouse.press_r&&!Remote.last_mouse.press_r) 
				{
					Remote.mouse.shoot_mode++;
					if(Remote.mouse.shoot_mode>3)Remote.mouse.shoot_mode=0;
				}
    }
}
