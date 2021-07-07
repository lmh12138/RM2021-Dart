#ifndef REMOTE_ENCORDER_H
#define REMOTE_ENCORDER_H

#include "stdint.h"

#define CH0_BIAS 1024
#define CH1_BIAS 1024
#define CH2_BIAS 1024
#define CH3_BIAS 1024
#define CH4_BIAS 1024

#define ON 1
#define OFF 0

#define Key_W 0x0001 //键盘对应的通信协议数据
#define Key_S 0x0002
#define Key_A 0x0004
#define Key_D 0x0008
#define Key_Shift 0x0010
#define Key_ECtrl 0x0020
#define Key_Q 0x0040
#define Key_E 0x0080
#define Key_R 0x0100
#define Key_F 0x0200
#define Key_G 0x0400
#define Key_Z 0x0800
#define Key_X 0x1000
#define Key_C 0x2000
#define Key_V 0x4000
#define Key_B 0x8000

#define SPEED_UP ((Remote.key.val) & (Key_Shift))
#define FORWARD ((Remote.key.val) & (Key_W))
#define BACKWARD ((Remote.key.val) & (Key_S))
#define LEFT ((Remote.key.val) & (Key_A))
#define RIGHT ((Remote.key.val) & (Key_D))
#define AUTO_AIMING (Remote.key.z)
#define ROTATE_RUN (Remote.key.r)
#define SHOOT (Remote.mouse.press_l)
#define SHOOT_SYMBOL (Remote.key.f)
#define IMPEACHMENT (Remote.key.c)
#define FOLLOW_CHASSIS (Remote.key.x)
#define SHOOT_MODE (Remote.mouse.shoot_mode)
#define RESET (Remote.key.reset)

struct Remote //遥控器
{
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    int16_t ch4;
    int8_t s1;
    int8_t last_s1;
    int8_t s2;
    int8_t last_s2;
};

struct Mouse
{
    int16_t x;
    int16_t y;
    int16_t z;
    uint8_t press_l;
    uint8_t press_r;
	int8_t shoot_mode;
	int16_t decorder_x;
	
};

struct Key
{
    uint16_t val;
    int x;
    int z;
	int f;
	int r;
	int c;
	int reset;
};

enum RemoteInputMode_e
{
    RC_Stop,
    RC_Remote,
    RC_MouseKey
};

struct DT7Remote
{
    struct Remote rc;
    struct Remote last_rc;
    struct Mouse mouse;
    struct Mouse last_mouse;
    struct Key key;
    struct Key last_key;
    enum RemoteInputMode_e inputmode;
};

void remote_decoder(void);

#endif
