/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "controls.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MSG_FLAG_MSK 0x00000001U
#define MSG_BUFF_MAX_SIZE 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart4;

/* Definitions for uartWriteTask */
osThreadId_t uartWriteTaskHandle;
const osThreadAttr_t uartWriteTask_attributes = {
  .name = "uartWriteTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for uartReadTask */
osThreadId_t uartReadTaskHandle;
const osThreadAttr_t uartReadTask_attributes = {
  .name = "uartReadTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for controlTask */
osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for uartIn */
osMessageQueueId_t uartInHandle;
const osMessageQueueAttr_t uartIn_attributes = {
  .name = "uartIn"
};
/* Definitions for commandQueue */
osMessageQueueId_t commandQueueHandle;
const osMessageQueueAttr_t commandQueue_attributes = {
  .name = "commandQueue"
};
/* Definitions for feedbackQueue */
osMessageQueueId_t feedbackQueueHandle;
const osMessageQueueAttr_t feedbackQueue_attributes = {
  .name = "feedbackQueue"
};
/* Definitions for launchTimer */
osTimerId_t launchTimerHandle;
const osTimerAttr_t launchTimer_attributes = {
  .name = "launchTimer"
};
/* Definitions for instrumentationReadTimer */
osTimerId_t instrumentationReadTimerHandle;
const osTimerAttr_t instrumentationReadTimer_attributes = {
  .name = "instrumentationReadTimer"
};
/* USER CODE BEGIN PV */

uint8_t sequence_number = 0;

const uint16_t pins[4] = {
  GPIO_PIN_0,
  GPIO_PIN_1,
  GPIO_PIN_2,
  GPIO_PIN_3
};

const launchAction_t launchSequence[5] = {
  {IGNIT, OPEN, 0},
  {IGNIT, CLOSE, 1000U},
  {MEV, OPEN, 10U},
  {MEV, CLOSE, 500U},
  {OX, OPEN, 1000U}
};


volatile action_t valveStates[4] = {
  CLOSE, 
  CLOSE, 
  CLOSE, 
  CLOSE
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
void uartWrite(void *argument);
void uartRead(void *argument);
void valveControl(void *argument);
void launchTimerCallback(void *argument);
void instrumentationRead(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t rx_buff = 0;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart4, &rx_buff, 1);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of launchTimer */
  launchTimerHandle = osTimerNew(launchTimerCallback, osTimerOnce, NULL, &launchTimer_attributes);

  /* creation of instrumentationReadTimer */
  instrumentationReadTimerHandle = osTimerNew(instrumentationRead, osTimerPeriodic, NULL, &instrumentationReadTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of uartIn */
  uartInHandle = osMessageQueueNew (32, sizeof(uint8_t), &uartIn_attributes);

  /* creation of commandQueue */
  commandQueueHandle = osMessageQueueNew (16, sizeof(commandPacket_t), &commandQueue_attributes);

  /* creation of feedbackQueue */
  feedbackQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &feedbackQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of uartWriteTask */
  uartWriteTaskHandle = osThreadNew(uartWrite, NULL, &uartWriteTask_attributes);

  /* creation of uartReadTask */
  uartReadTaskHandle = osThreadNew(uartRead, NULL, &uartReadTask_attributes);

  /* creation of controlTask */
  controlTaskHandle = osThreadNew(valveControl, NULL, &controlTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == UART4)
  {
    if(osMessageQueueGetSpace(uartInHandle) > 0)
    {
      osMessageQueuePut(uartInHandle, &rx_buff, 0, 0);
    }
    HAL_UART_Receive_IT(&huart4, &rx_buff, 1);
  }
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_uartWrite */
/**
* @brief Function implementing the uartWriteTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uartWrite */
void uartWrite(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_uartRead */
/**
* @brief Function implementing the uartReadTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uartRead */
void uartRead(void *argument)
{
  /* USER CODE BEGIN uartRead */
  /* Infinite loop */
  uint8_t message[MSG_BUFF_MAX_SIZE];
  uint8_t in_char;
  uint8_t size = 0;
  commandPacket_t command;

  for(;;)
  {
    if(osMessageQueueGet(uartInHandle, &in_char, NULL, osWaitForever) == osOK)
    {
      message[size] = in_char;
      size++;
      if(in_char == '\n' || size == MSG_BUFF_MAX_SIZE)
      {
        message[size - 1] = '\0';
        command = string_to_command(message);
        osMessageQueuePut(commandQueueHandle, &command, 0, 0);
        size = 0;
      }
    }
  }
  /* USER CODE END uartRead */
}

/* USER CODE BEGIN Header_valveControl */
/**
* @brief Function implementing the controlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_valveControl */
void valveControl(void *argument)
{
  /* USER CODE BEGIN valveControl */
  /* Infinite loop */
  commandPacket_t command;

  for(;;)
  {
    if(osMessageQueueGet(commandQueueHandle, &command, NULL, osWaitForever) == osOK)
    {
      switch (command.id)
      {
        case ABORT:
          break;
        case LAUNCH:
          HAL_GPIO_WritePin(GPIOC, pins[launchSequence[0].valve], launchSequence[0].action);
          sequence_number++;
          osTimerStart(launchTimerHandle, launchSequence[sequence_number].time);
          break;
        case MOVE:
          break;
        default:
          break;
      }
    }
  }
  /* USER CODE END valveControl */
}

/* launchTimerCallback function */
void launchTimerCallback(void *argument)
{
  /* USER CODE BEGIN launchTimerCallback */
  HAL_GPIO_WritePin(GPIOC, pins[launchSequence[sequence_number].valve], launchSequence[sequence_number].action);
  sequence_number++;
  if(sequence_number < 5)
    osTimerStart(launchTimerHandle, launchSequence[sequence_number].time);
  /* USER CODE END launchTimerCallback */
}

/* instrumentationRead function */
void instrumentationRead(void *argument)
{
  /* USER CODE BEGIN instrumentationRead */
  
  /* USER CODE END instrumentationRead */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
