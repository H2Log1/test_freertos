/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Emm_V5.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t btn_counter = 0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Btn_Task */
osThreadId_t Btn_TaskHandle;
const osThreadAttr_t Btn_Task_attributes = {
  .name = "Btn_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LED_Task */
osThreadId_t LED_TaskHandle;
const osThreadAttr_t LED_Task_attributes = {
  .name = "LED_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Motor_Task */
osThreadId_t Motor_TaskHandle;
const osThreadAttr_t Motor_Task_attributes = {
  .name = "Motor_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Btn_Queue */
osMessageQueueId_t Btn_QueueHandle;
const osMessageQueueAttr_t Btn_Queue_attributes = {
  .name = "Btn_Queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartBtn_Task(void *argument);
void StartLED_Task(void *argument);
void StartMotor_Task(void *argument);

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

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Btn_Queue */
  Btn_QueueHandle = osMessageQueueNew (16, sizeof(uint32_t), &Btn_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Btn_Task */
  Btn_TaskHandle = osThreadNew(StartBtn_Task, NULL, &Btn_Task_attributes);

  /* creation of LED_Task */
  LED_TaskHandle = osThreadNew(StartLED_Task, NULL, &LED_Task_attributes);

  /* creation of Motor_Task */
  Motor_TaskHandle = osThreadNew(StartMotor_Task, NULL, &Motor_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartBtn_Task */
/**
  * @brief  Function implementing the Btn_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartBtn_Task */
void StartBtn_Task(void *argument)
{
  /* USER CODE BEGIN StartBtn_Task */

  /* Infinite loop */
  for(;;)
  {
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET) 
    {
      osDelay(10); // 防抖延时
      if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET)
      {
        btn_counter++;
        osMessageQueuePut(Btn_QueueHandle, &btn_counter, NULL, osWaitForever);
      }
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET)
      {
        osDelay(10); // 等待按键释放
      }
    }
    else
    {
      osDelay(10);
    }
  }
  /* USER CODE END StartBtn_Task */
}

/* USER CODE BEGIN Header_StartLED_Task */
/**
* @brief Function implementing the LED_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLED_Task */
void StartLED_Task(void *argument)
{
  /* USER CODE BEGIN StartLED_Task */

  /* Infinite loop */
  for(;;)
  {
    // osMessageQueueGet(Btn_QueueHandle, &btn_counter, NULL, osWaitForever);
    // if (btn_counter % 2 == 0) {
    //   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    // } 
    // else 
    // {
    //   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    // }
    osDelay(1);
  }
  /* USER CODE END StartLED_Task */
}

/* USER CODE BEGIN Header_StartMotor_Task */
/**
* @brief Function implementing the Motor_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMotor_Task */
void StartMotor_Task(void *argument)
{
  /* USER CODE BEGIN StartMotor_Task */
  uint16_t motor_vel_init = 500;
  Emm_V5_En_Control(0x01, true, false);
  osDelay(10);
  Emm_V5_Vel_Control(0x01, 0x00, motor_vel_init, 50, false);
  osDelay(10);
  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(Btn_QueueHandle, &btn_counter, NULL, osWaitForever);
    if (btn_counter % 2 == 0)
    {
      Emm_V5_Vel_Control(0x01, 0x00, motor_vel_init, 50, false);
    }
    else
    {
      Emm_V5_Vel_Control(0x01, 0x01, 0, 50, false);
    }
    osDelay(10);
  }
  /* USER CODE END StartMotor_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

