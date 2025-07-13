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
#include "dash.h"
#include "buttons.h"
#include "utils.h"
#include "grIDs.h"
#include "msgIDs.h"
#include "fdcan.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern volatile DashInfo globalStatus;

osThreadId_t lvglTickHandle;
const osThreadAttr_t lvglTick_attributes = {
  .name = "lvglTick",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4 * 1024,
};

osThreadId_t lvglTimerHandle;
const osThreadAttr_t lvglTimer_attributes = {
  .name = "lvglTimer",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4 * 1024,
};

osThreadId_t dashStatusMsgHandle;
const osThreadAttr_t dashStatusMsgAttributes = {
  .name = "dashStatusMsg",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 4 * 1024,
};

osThreadId_t pollButtonStateHandle;
const osThreadAttr_t pollButtonStateAttributes = {
  .name = "pollButtonState",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4 * 1024,
};

osThreadId_t updateButtonColorsHandle;
const osThreadAttr_t updateButtonColorsAttributes = {
  .name = "updateButtonColors",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 4 * 1024,
};

osThreadId_t clearDebugMsgHandle;
const osThreadAttr_t clearDebugMsgAttributes = {
  .name = "clearDebugMsg",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 4 * 1024,
};

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
void sendDashStatusMsg(void* args);
void testLvglBufferVehicleSpeed(void* args);
void clearDebugMsg(void* args);
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
 // readPin = osThreadNew(HAL_GPIO_ReadPin(GPIOA, ) )
  /* add threads, ... */
  lvglTickHandle = osThreadNew(LVGLTick, NULL, &lvglTick_attributes);
  lvglTimerHandle = osThreadNew(LVGLTimer, NULL, &lvglTimer_attributes);
  dashStatusMsgHandle = osThreadNew(sendDashStatusMsg, NULL, &dashStatusMsgAttributes);
  pollButtonStateHandle = osThreadNew(pollButtonState, NULL, &pollButtonStateAttributes);
  updateButtonColorsHandle = osThreadNew(updateButtonColors, NULL, &updateButtonColorsAttributes);
  clearDebugMsgHandle = osThreadNew(clearDebugMsg, NULL, &clearDebugMsgAttributes);
  /* comment out testLVGLBufferWriteHandle when testing with CAN */
  // testLVGLBufferWriteHandle = osThreadNew(testLvglBufferVehicleSpeed, NULL, &defaultTask_attributes);

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
    osDelay(1);
  }
  UNUSED(argument);
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* LVGL timer for tasks */
void LVGLTimer(void *argument)
{
    /*
    one function of lv_timer_handler() is to check for parts of the UI that have become "invalidated"
    invalidated means a portion of the UI that changed
    if a portion of the screen is marked as invalidated, LVGL will redraw those parts of the screen
    */
  for(;;)
  {
    lv_timer_handler();

    if (recievedNewInformationPleaseRefresh)
    {
        recievedNewInformationPleaseRefresh = false;
        updateDataFromCAN();
        updateDebugMsg();
    }

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

void sendDashStatusMsg(void* args)
{
    for(;;)
    {
        #ifdef ADVANCED_LOGGING
        LOGOMATIC("Sending Dash Status msg\n");
        #endif

        writeMessage(MSG_DASH_STATUS, GR_ECU, (uint8_t*)&globalStatus.dashStatusMsg, 3);
        osDelay(DASH_STATUS_MSG_DELAY);
    }

    UNUSED(args);
}

void clearDebugMsg(void* args)
{
    uint32_t lastUpdated = HAL_GetTick();

    for (;;)
    {
        if (globalStatus.debugMessage[0] != '\0' && lastUpdated == 0)
        {
            LOGOMATIC("Starting timer for clearing debug message\n");
            lastUpdated = HAL_GetTick();
        }
        else if (HAL_GetTick() - lastUpdated > DISPLAY_DEBUG_MESSAGE_TIME_MS)
        {
            LOGOMATIC("Clearing debug message\n");
            globalStatus.debugMessage[0] = '\0';
            lastUpdated = 0;
        }

        osDelay(200);
    }
    UNUSED(args);
}

void testLvglBufferVehicleSpeed(void* args)
{
  for (;;)
  {
      LOGOMATIC("\n!!!THIS SHOULD NOT BE RUNNING IN PROD!!!\n\n");
      globalStatus.vehicleSpeed += 1;
      osDelay(1000);
  }
  UNUSED(args);
}
/* USER CODE END Application */
