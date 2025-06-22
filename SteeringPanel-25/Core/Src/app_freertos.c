/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lvgl/lvgl.h"
#include "encoders.h"
#include "steering.h"
#include "fdcan.h"
#include "steering.h"
#include "gui.h"
#include "CANdler.h"
#include "msgIDs.h"
#include "grIDs.h"
#include "utils.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ENCODER_POLL_DELAY 40 / 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t lvglTickHandle;
const osThreadAttr_t lvglTick_attributes = {
  .name = "lvglTick",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 4* 1024
};

osThreadId_t lvglTimerHandle;
const osThreadAttr_t lvglTimer_attributes = {
  .name = "lvglTimer",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4* 1024
};

osThreadId_t pinPollHandle;
const osThreadAttr_t pinPoll_attributes = {
  .name = "pinPoll",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4* 1024
};

osThreadId_t statusHeartbeatHandle;
const osThreadAttr_t statusHeartbeat_attributes = {
  .name = "statusHeartbeat",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 4* 1024
};

bool thisIsMine = true;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void LVGLTimer(void *argument);
void LVGLTick(void *argument);
void PinPoll(void *argument);
void StatusHeartbeat(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName);

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName)
{
  UNUSED(xTask);
  UNUSED(pcTaskName);
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

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

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  lvglTickHandle = osThreadNew(LVGLTick, NULL, &lvglTick_attributes);
  lvglTimerHandle = osThreadNew(LVGLTimer, NULL, &lvglTimer_attributes);
  pinPollHandle = osThreadNew(PinPoll, NULL, &pinPoll_attributes);
  statusHeartbeatHandle = osThreadNew(StatusHeartbeat, NULL, &statusHeartbeat_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
  }
  UNUSED(argument);
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* LVGL timer for tasks */
void LVGLTimer(void *argument)
{
  for(;;)
  {
    if (refreshScreen()) lv_obj_invalidate(lv_screen_active());
    
    lv_timer_handler();
    osDelay(10);
  }
  UNUSED(argument);
}
/* LVGL tick source */
void LVGLTick(void *argument)
{
  for(;;)
  {
    lv_tick_inc(10);
    osDelay(10);
  }
  UNUSED(argument);
}

/* poll pins */
void PinPoll(void *argument)
{
  for(;;)
  {
    pollEncoderStatus(ENC_CURRENT);
    osDelay(ENCODER_POLL_DELAY);

    pollEncoderStatus(ENC_TORQUE);
    osDelay(ENCODER_POLL_DELAY);

    pollEncoderStatus(ENC_REGEN);
    osDelay(ENCODER_POLL_DELAY);

    pollButtonStatus();
    osDelay(ENCODER_POLL_DELAY);
  }
  UNUSED(argument);
}

/* Status heartbeat*/
void StatusHeartbeat(void *argument)
{
  for(;;)
  {
    writeToECU(MSG_STEERING_STATUS, (uint8_t*)&outgoingData.steeringStatusMsg, 2);
    osDelay(200);
  }
  UNUSED(argument);
}
/* USER CODE END Application */

