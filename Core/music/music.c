#include <string.h>
#include "tim.h"
#include "dac.h"
#include "music.h"

#define		BUFFER_SIZE		(1024)

extern unsigned data[26022];
uint8_t Buffer0[BUFFER_SIZE] = {0};
uint8_t Buffer1[BUFFER_SIZE] = {0};


void Music_Player(void)
{
	uint32_t DataLength = 0;
	uint8_t* DataAddress = NULL;

#if 1
	DataLength = sizeof(data) - 0x2c;
	DataAddress = (unsigned char *)(data + 0x2c);
	memset(Buffer0, 0, BUFFER_SIZE);
	memset(Buffer1, 0, BUFFER_SIZE);
	HAL_TIM_Base_Start(&htim6);

	while(1)
	{
		if(DataLength >= BUFFER_SIZE)
		{
			memcpy(Buffer0, DataAddress, BUFFER_SIZE);
			DataLength -= BUFFER_SIZE;
			DataAddress += BUFFER_SIZE;
			HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)Buffer0, BUFFER_SIZE, DAC_ALIGN_8B_R);
			while(HAL_DAC_GetState(&hdac) != HAL_DAC_STATE_READY);
		}
		else break;
	}
	HAL_TIM_Base_Stop(&htim6);
	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
#else
	DataLength = sizeof(data) - 0x2c;
	DataAddress = (uint8_t *)data + 0x2c;
	HAL_TIM_Base_Start(&htim6);

	while(1)
	{
		if(DataLength >= 0xFFFF)
		{
			HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)DataAddress, 0xFFFF, DAC_ALIGN_8B_R);
			while(HAL_DAC_GetState(&hdac) != HAL_DAC_STATE_READY);
			DataLength -= 0xFFFF;
			DataAddress += 0xFFFF;
		}
		else break;
	}
	HAL_TIM_Base_Stop(&htim6);
	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
#endif
}
// extern uint32_t DataLength;
// extern uint8_t* DataAddress;

// #define	BUFFER_SIZE	1024
 
// uint8_t Buffer0[BUFFER_SIZE] = {0};
 
// void Music_Player(void)
// {
// 	uint32_t DataLength = 0;
// 	uint8_t* DataAddress = NULL;
 
// 	DataLength = sizeof(data) - 0x2c;
// 	DataAddress = (unsigned char *)(data + 0x2c);
// 	memset(Buffer0, 0, BUFFER_SIZE);
// 	HAL_TIM_Base_Start(&htim6);
 
// 	while(1)
// 	{
// 		if(DataLength >= BUFFER_SIZE)
// 		{
// 			memcpy(Buffer0, DataAddress, BUFFER_SIZE);
// 			DataLength -= BUFFER_SIZE;
// 			DataAddress += BUFFER_SIZE;
// 			HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)Buffer0, BUFFER_SIZE, DAC_ALIGN_8B_R);
// 			while(HAL_DAC_GetState(&hdac) != HAL_DAC_STATE_READY);
// 		}
// 		else break;
// 	}
// 	HAL_TIM_Base_Stop(&htim6);
// 	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
// }
 
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
// 	if(htim->Instance == TIM6)
// 	{
// 		TIM2->CCR1 = *DataAddress;
// 		DataLength--;
// 		DataAddress++;
// 		if(DataLength == 0)
// 		{
// 			  HAL_TIM_Base_Stop_IT(&htim6);
// 			  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
// 		}
// 	}
// }

