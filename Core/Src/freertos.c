/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "supervise.h"
#include "fifo.h"
#include "referee.h"
#include "robot.h"
#include "position.h"
#include "shoot.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int flag = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern fifo_s_t referee_fifo;
extern uint8_t referee_fifo_buf[REFEREE_FIFO_BUF_LENGTH];
/* USER CODE END Variables */
/* Definitions for ShootTask */
osThreadId_t ShootTaskHandle;
const osThreadAttr_t ShootTask_attributes = {
  .name = "ShootTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PositionTask */
osThreadId_t PositionTaskHandle;
const osThreadAttr_t PositionTask_attributes = {
  .name = "PositionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UserCmdTask */
osThreadId_t UserCmdTaskHandle;
const osThreadAttr_t UserCmdTask_attributes = {
  .name = "UserCmdTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for RefereeTask */
osThreadId_t RefereeTaskHandle;
const osThreadAttr_t RefereeTask_attributes = {
  .name = "RefereeTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Supervise_Timer */
osTimerId_t Supervise_TimerHandle;
const osTimerAttr_t Supervise_Timer_attributes = {
  .name = "Supervise_Timer"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartShootTask(void *argument);
void StartPositionTask(void *argument);
void StartUserCmdTask(void *argument);
void StartRefereeTask(void *argument);
void SuperviseTimer(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Supervise_Timer */
  Supervise_TimerHandle = osTimerNew(SuperviseTimer, osTimerPeriodic, NULL, &Supervise_Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
    osTimerStart(Supervise_TimerHandle, 2);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ShootTask */
  ShootTaskHandle = osThreadNew(StartShootTask, NULL, &ShootTask_attributes);

  /* creation of PositionTask */
  PositionTaskHandle = osThreadNew(StartPositionTask, NULL, &PositionTask_attributes);

  /* creation of UserCmdTask */
  UserCmdTaskHandle = osThreadNew(StartUserCmdTask, NULL, &UserCmdTask_attributes);

  /* creation of RefereeTask */
  RefereeTaskHandle = osThreadNew(StartRefereeTask, NULL, &RefereeTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartShootTask */
/**
  * @brief  Function implementing the ShootTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartShootTask */
void StartShootTask(void *argument)
{
  /* USER CODE BEGIN StartShootTask */
  /* Infinite loop */
  for(;;)
  {
    ShootParamChange();
    osDelay(2);
  }
  /* USER CODE END StartShootTask */
}

/* USER CODE BEGIN Header_StartPositionTask */
/**
* @brief Function implementing the PositionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPositionTask */
void StartPositionTask(void *argument)
{
  /* USER CODE BEGIN StartPositionTask */
  /* Infinite loop */
  for(;;)
  {
    PositionParamChange();
    Remain35083510FrictiongearSpeed();
    osDelay(2);
  }
  /* USER CODE END StartPositionTask */
}

/* USER CODE BEGIN Header_StartUserCmdTask */
GPIO_PinState a = 0;
/**
* @brief Function implementing the UserCmdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUserCmdTask */
void StartUserCmdTask(void *argument)
{
  /* USER CODE BEGIN StartUserCmdTask */
  DartParamInit();
  /* Infinite loop */
  for(;;)
  {
		//a = HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_11);
    DartStateChange();
    osDelay(1);
  }
  /* USER CODE END StartUserCmdTask */
}

/* USER CODE BEGIN Header_StartRefereeTask */
/**
* @brief Function implementing the RefereeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRefereeTask */
void StartRefereeTask(void *argument)
{
  /* USER CODE BEGIN StartRefereeTask */
	fifo_s_init(&referee_fifo, referee_fifo_buf, REFEREE_FIFO_BUF_LENGTH);
	USART6_Init();
	Referee_Data_Init();
	//InitUI();
  /* Infinite loop */
  for(;;){
	  Referee_Unpack_FIFO_Data();
    osDelay(100);
  }
  /* USER CODE END StartRefereeTask */
}

/* SuperviseTimer function */
void SuperviseTimer(void *argument)
{
  /* USER CODE BEGIN SuperviseTimer */
    //SuperviseTaskHandle();
  /* USER CODE END SuperviseTimer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
