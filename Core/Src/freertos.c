/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "usart.h"
#include <string.h>
#include "snake.h"
#include "kb.h"
#include "buzzer.h"

extern void KB_Test(void);
extern void I2C1_MutexInit(void);
/* USER CODE END Includes */
static const uint32_t megalovania_melody[] = {
    N_D3, N_D3, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_C3, N_C3, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_B2, N_B2, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_AS2, N_AS2, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_D3, N_D3, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_C3, N_C3, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_B2, N_B2, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_AS2, N_AS2, N_D4, N_A3, 0, N_GS3, N_G3, N_F3, N_D3, N_F3, N_G3, N_D4, N_D4, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_C4, N_C4, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_B3, N_B3, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_AS3, N_AS3, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_D4, N_D4, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_C4, N_C4, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_B3, N_B3, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_AS3, N_AS3, N_D5, N_A4, 0, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, N_F4, N_F4, N_F4, N_F4, N_F4, N_D4, N_D4, N_D4, N_F4, N_F4, N_F4, N_G4, N_GS4, N_G4, N_F4, N_D4, N_F4, N_G4, 0, N_F4, N_F4, N_F4, N_G4, N_GS4, N_A4, N_C5, N_A4, N_D5, N_D5, N_D5, N_A4, N_D5, N_C5, N_F4, N_F4, N_F4, N_F4, N_F4, N_D4, N_D4, N_D4, N_F4, N_F4, N_F4, N_F4, N_D4, N_F4, N_E4, N_D4, N_C4, 0, N_G4, N_E4, N_D4, N_D4, N_D4, N_D4, N_F3, N_G3, N_AS3, N_C4, N_D4, N_F4, N_C5, 0, N_F4, N_D4, N_F4, N_G4, N_GS4, N_G4, N_F4, N_D4, N_GS4, N_G4, N_F4, N_D4, N_F4, N_F4, N_F4, N_GS4, N_A4, N_C5, N_A4, N_GS4, N_G4, N_F4, N_D4, N_E4, N_F4, N_G4, N_A4, N_C5, N_CS5, N_GS4, N_GS4, N_G4, N_F4, N_G4, N_F3, N_G3, N_A3, N_F4, N_E4, N_D4, N_E4, N_F4, N_G4, N_E4, N_A4, N_A4, N_G4, N_F4, N_DS4, N_CS4, N_DS4, 0, N_F4, N_D4, N_F4, N_G4, N_GS4, N_G4, N_F4, N_D4, N_GS4, N_G4, N_F4, N_D4, N_F4, N_F4, N_F4, N_GS4, N_A4, N_C5, N_A4, N_GS4, N_G4, N_F4, N_D4, N_E4, N_F4, N_G4, N_A4, N_C5, N_CS5, N_GS4, N_GS4, N_G4, N_F4, N_G4, N_F3, N_G3, N_A3, N_F4, N_E4, N_D4, N_E4, N_F4, N_G4, N_E4, N_A4, N_A4, N_G4, N_F4, N_DS4, N_CS4, N_DS4
};

static const uint32_t megalovania_delays[] = {
    16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 16, 16, 8, 6, 32, 8, 8, 8, 16, 16, 16, 8, 16, 8, 8, 8, 8, 4, 16, 8, 16, 8, 8, 8, 16, 16, 16, 16, 16, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 8, 16, 16, 16, 2, 8, 16, 8, 8, 8, 8, 4, 16, 8, 16, 8, 8, 8, 8, 8, 16, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, 15, 8, 8, 2, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 2, 16, 8, 16, 8, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8,  8, 8, 16, 16, 16, 2, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 2, 8, 8, 8, 8, 2, 2, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 2, 16, 8, 16, 8, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8,  8, 8, 16, 16, 16, 2, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 2, 8, 8, 8, 8, 2, 1
};



static const uint32_t zelda_melody[] = {
    N_AS4, 0, 0, N_AS4, N_AS4, N_AS4, N_AS4, N_AS4, 0, N_GS4, N_AS4, 0, 0, N_AS4, N_AS4, N_AS4, N_AS4, N_AS4, 0, N_GS4, N_AS4, 0, 0, N_AS4, N_AS4, N_AS4, N_AS4, N_AS4, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_AS4, N_F3, N_F3, 0, N_AS4, N_AS4, N_C5, N_D5, N_DS5, N_F5, 0, N_F5, N_F5, N_FS5, N_GS5, N_AS5, 0, N_AS5, N_AS5, N_AS5, N_GS5, N_FS5, N_GS5, 0, N_FS5, N_F5, N_F5, N_DS5, N_DS5, N_F5, N_FS5, N_F5, N_DS5, N_CS5, N_CS5, N_DS5, N_F5, N_DS5, N_CS5, N_C5, N_C5, N_D5, N_E5, N_G5, N_F5, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_AS4, N_F3, N_F3, 0, N_AS4, N_AS4, N_C5, N_D5, N_DS5, N_F5, 0, N_F5, N_F5, N_FS5, N_GS5, N_AS5, 0, N_CS6, N_C6, N_A5, 0, N_F5, N_FS5, 0, N_AS5, N_A5, N_F5, 0, N_F5, N_FS5, 0, N_AS5, N_A5, N_F5, 0, N_D5, N_DS5, 0, N_FS5, N_F5, N_CS5, 0, N_AS4, N_C5, N_C5, N_D5, N_E5, 0, N_G5, N_F5, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_AS4, N_F3, N_F3, 0, N_AS4, N_AS4, N_C5, N_D5, N_DS5, N_F5, 0, N_F5, N_F5, N_FS5, N_GS5, N_AS5, 0, N_AS5, N_AS5, N_AS5, N_GS5, N_FS5, N_GS5, 0, N_FS5, N_F5, N_F5, N_DS5, N_DS5, N_F5, N_FS5, N_F5, N_DS5, N_CS5, N_CS5, N_DS5, N_F5, N_DS5, N_CS5, N_C5, N_C5, N_D5, N_E5, N_G5, N_F5, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_AS4, N_F3, N_F3, 0, N_AS4, N_AS4, N_C5, N_D5, N_DS5, N_F5, 0, N_F5, N_F5, N_FS5, N_GS5, N_AS5, 0, N_CS6, N_C6, N_A5, 0, N_F5, N_FS5, 0, N_AS5, N_A5, N_F5, 0, N_F5, N_FS5, 0, N_AS5, N_A5, N_F5, 0, N_D5, N_DS5, 0, N_FS5, N_F5, N_CS5, 0, N_AS4, N_C5, N_C5, N_D5, N_E5, 0, N_G5, N_F5, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3, N_F3
};

static const uint32_t zelda_delays[] = {
    2, 8, 8, 8, 8, 8, 8, 6, 16, 16, 4, 8, 8, 8, 8, 8, 8, 6, 16, 16, 4, 8, 8, 8, 8, 8, 8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 6, 16, 16, 2, 4, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8, 8, 8, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 8, 16, 16, 4, 4, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 6, 16, 16, 2, 4, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8, 8, 8, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 8, 16, 16, 4, 4, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8
};

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

/* USER CODE END Variables */
/* Private variables ---------------------------------------------------------*/
osThreadId Task_SnakeHandle;
osThreadId Task_KbdHandle;
osThreadId Task_LEDHandle;
osMessageQId myQueue01Handle;
osThreadId Task_BuzzerHandle;


/* Private function prototypes -----------------------------------------------*/
void StartTaskSnake(void const * argument);
void StartTaskKeyboard(void const * argument);
void StartTaskLED(void const * argument);
void StartTaskBuzzer(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* Init I2C mutex */
  I2C1_MutexInit();

  /* Create the queue(s) for keyboard events */
  osMessageQDef(myQueue01, 16, uint16_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* Задача 1: Опрос матричной клавиатуры (высокий приоритет, опрос раз в 20 мс) */
  osThreadDef(Task_Kbd, StartTaskKeyboard, osPriorityAboveNormal, 0, 128);
  Task_KbdHandle = osThreadCreate(osThread(Task_Kbd), NULL);

  /* Задача 2: Логика игры "Змейка" и отрисовка OLED (стек 512 слов) */
  osThreadDef(Task_Snake, StartTaskSnake, osPriorityNormal, 0, 512);
  Task_SnakeHandle = osThreadCreate(osThread(Task_Snake), NULL);

  /* Задача 3: Heartbeat светодиод (низкий приоритет) */
  osThreadDef(Task_LED, StartTaskLED, osPriorityLow, 0, 128);
  Task_LEDHandle = osThreadCreate(osThread(Task_LED), NULL);

  osThreadDef(Task_Buzzer, StartTaskBuzzer, osPriorityLow, 0, 256);
  Task_BuzzerHandle = osThreadCreate(osThread(Task_Buzzer), NULL);

}

void StartTaskKeyboard(void const * argument)
{
  char last_key = 0;
  for(;;)
  {
    char key = Scan_Keyboard();
    if (key != 0 && key != last_key) {
      osMessagePut(myQueue01Handle, (uint32_t)key, 0);
    }
    last_key = key;
    osDelay(5);
  }
}


void StartTaskBuzzer(void const * argument)
{
  Buzzer_Init(); // Инициализируем ШИМ таймера TIM2

  for(;;)
  {
    Buzzer_Play(zelda_melody, zelda_delays, sizeof(zelda_melody) / sizeof(uint32_t));
    osDelay(1000);

    Buzzer_Play(megalovania_melody, megalovania_delays, sizeof(megalovania_melody) / sizeof(uint32_t));
    osDelay(1000);
  }
}

/**
  * @brief  Задача логики змейки и отрисовки на экране OLED
  */
void StartTaskSnake(void const * argument)
{
  Snake_Init();

  for(;;)
  {
    uint32_t tick_start = osKernelSysTick();
    uint32_t delay = Snake_GetDelay();
    uint32_t remaining = delay;

    // Ждём ввод с таймаутом — реагируем на кнопки мгновенно,
    // а шаг змейки делаем строго по таймеру
    while (remaining > 0) {
      osEvent evt = osMessageGet(myQueue01Handle, remaining);
      if (evt.status == osEventMessage) {
        Snake_HandleInput((char)evt.value.v);
      }
      uint32_t elapsed = osKernelSysTick() - tick_start;
      if (elapsed >= delay) break;
      remaining = delay - elapsed;
    }

    Snake_Step();
  }
}

/**
  * @brief  Задача индикации работы RTOS (мигание светодиодом)
  */
void StartTaskLED(void const * argument)
{
  for(;;)
  {
    // Мигаем оранжевым светодиодом (PD13)
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
    osDelay(5000);
  }
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
