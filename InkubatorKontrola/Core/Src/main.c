/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define VAR_DECLS 1
#include "variables.h"
#include "functions.h"
#include "BMP280.h"
#include  <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "unit_tests.h"
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;

/* USER CODE BEGIN PV */
float Kp=14400.00;			//Kp PID kontrolera 1400 OK stavljen 2400 test
float Ki=200.2;				//Ki PID kontrolera-7.2 OK stavljen 10.2 test
float Kd=1.30;				//Kd PID kontrolera
char lcd_string[100];		//string za ispisivanje  na LCD

TIME time; //object struct u koji se upisuju podaci iz stringa
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static int counter_test=0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

#define TIME_FOR_COMPLETION			30240u
	uint8_t startDay,startMonth,startHour,StartMinute;															//start day,month,hour,minute
	uint8_t currentDay,currentMonth,currentHour,currentMinute,previousHour,previousDay,previousMinute;			//current and previous month,day,hour, minute

 	static uint8_t  remainingDays=0;
 	static uint16_t	remainingHours=0;
 	static uint8_t 	currentDayCnt=0;
 	static uint16_t	remainingMinutes=0;
 	static uint8_t	curentHrsCnt=0;
 	static uint8_t	currentMinCnt=0;
	bool incubationStarted=FALSE;
	bool incubationFinished=FALSE;
	bool stepperControlActive=FALSE;
	static bool daySts=TRUE;
	static bool hourSts=TRUE;
	static bool minSts=TRUE;
	int adrFound=0;
	int RHcnt=0;
	volatile static uint8_t menu_cnt;

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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
HAL_TIM_Base_Start_IT(&htim2);
HAL_TIM_Base_Start_IT(&htim5);
//lcd initialization and backlight
glcd_init();
glcd_enable_backlight(ENABLE);
//check to see if there is connection on I2C bus
adrFound=find_I2C_deviceAddress();

for(int i=176;i<255;i++)
	{
		if(HAL_I2C_IsDeviceReady(&hi2c1,i,1,10)==HAL_OK)
		{
			break;
		}

	}
//BME280 initialization
BMP280_init(0x57,0x48,0x05);// osrs_t 010 x2, osrs_p 16 101, mode normal 11 // standby time 500ms 100, filter 16 100, SPI DIS 0
BMP280_calc_values();
//Get Time and date values from DS3231 RTC that is connected on I2C bus
getTimeDate_DS3231(DS3231_ADDRESS_I2C);
delay_ms(50);
//Intro menu
start_menu_1();
delay_ms(2000);
//Menu for choosing options;
start_menu_2();
glcd_clear_buffer();
//show_date_and_time();
//delay_ms(2000);
//show_tempAndHumidity();
//delay_ms(2000);
//incub_menu_1(22,22,22);
//delay_ms(2000);
//incub_menu_2(23,23,23);
//start_menu_3();
//glcd_test_circles();
//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);//Startuje NVIC interrupta za zero crossing
//setTime_DS3231(DS3231_ADDRESS_I2C, 0, 15 ,22 , 0, 13, 12, 20);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {



		  	  //incub_menu_2(22,22,22);	//set default value
		  	 // delay_ms(2000);
		  	  HAL_Delay(100);

		  	  show_tempAndHumidity();


			  //delay_ms(500);
			  getTimeDate_DS3231(DS3231_ADDRESS_I2C);	//extract current time
			  //show_tempAndHumidity();




	  //initialiaziaton of variables
      //extracts current time into variables
	  currentDay=time.date;
	  currentMonth=time.month;
	  currentHour=time.hours;
	  currentMinute=time.minutes;
	  uint8_t InkubStatus=*(ui8_ptrInkubStatus);

	  static uint8_t counter=0;
	  if(TRUE==InkubStatus)
	 {
		  //vadi vreme pocetka inkubacije
		  if(0>=counter)
		  {
			  startDay=time.date;
			  startMonth=time.month;
			  startHour=time.hours;
			  StartMinute=time.minutes;
		  }
		  else
		  {
			  //do nothing
		  }

		  PidKorekcija=PID_control(SETPOINT_TEMP, Kp,Ki,Kd,temperature);
		  HAL_GPIO_WritePin(FanPin_GPIO_Port, FanPin_Pin, GPIO_PIN_SET);//Turn ON FAN

		  counter++;
		  if(255==counter)
		  {
			  counter=1; //counter reset when it overflows
		  }
		  //TODO MM to add calculation for time remaining,
		  //days comparesescent
		  if(TRUE==daySts)
		  {
			  previousDay=currentDay;
			  currentDayCnt=0;
			  daySts=false;
		  }
		  else if(previousDay != currentDay)
		  {
			  currentDayCnt++;
			  previousDay=currentDay;
		  }
		  //hours comparesescent
		  if(TRUE==hourSts)
		  {
			  previousHour=currentHour;
			  curentHrsCnt=0;
			  hourSts=FALSE;
		  }
		 else if(previousHour != currentHour)
		 {
			 curentHrsCnt++;
			 previousHour=currentHour;
			 stepperControlActive=TRUE;
		  }
		  //minutes comparesescent
 		  if(TRUE==minSts)
 		  {
 			  previousMinute=currentMinute;
 			  currentMinCnt=0;
 			  minSts=FALSE;
 		  }
 		 else if(previousMinute != currentMinute)
 		 {
 			currentMinCnt++;
 			previousMinute=currentMinute;
 		  }




 		  	  counter_test++;
 		  	  //Days remaining
			  remainingDays=(TIME_FOR_COMPLETION/1440)-currentDayCnt;
			  //Hours remaining
			  remainingHours=(TIME_FOR_COMPLETION/60)-curentHrsCnt;
			  remainingHours%=24;		//hours for 21 days
			  //Minutes remaining
			  remainingMinutes=TIME_FOR_COMPLETION-currentMinCnt;
			  remainingMinutes%=60;	  //minutes for 21 days



			  if(0==remainingDays)
			  {
			  	currentDayCnt=0; //reset days counter


			  	if(0==remainingHours)
			  	{

			  		if(0==remainingMinutes)
			  		{
			  			incubationFinished=TRUE; //zavrsena inkubacija
			  		}
			  		else
			  		{
			  			//do nothing
			  		}

			  	}
			  	else
			  	{
			  		//do nothing
			  	}


			  }
			  else
			  {
				  //do nothing
			  }


			  delay_ms(1);



	  }
	  else if(true==incubationFinished)
	  {
		  counter=0; 						//returns counter to zero, to start incubation from begining
		  incubationStarted=FALSE;		   //puts incubation status into false, because incubation is not active
		  HAL_GPIO_WritePin(FanPin_GPIO_Port, FanPin_Pin, GPIO_PIN_RESET);//Turn OFF FAN10
	  }



		delay_ms(1);

		  //Turning of eggs at each hour in time of incubation for 19 days
		if(19>currentDayCnt)
		  {
			  if(TRUE==stepperControlActive)
			  {
				  stepperMotorControlFD(5);			//turns the eggs
			   	  stepperControlActive=FALSE;		//resets the state for stepper motor ctivation
			  }



		   }

	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#undef TIME_FOR_COMPLETION
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 59999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 59;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 0xfffffff;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, RESET_SPI_Pin|CS_SPI_Pin|SPI_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DC_SPI_GPIO_Port, DC_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, FanPin_Pin|StepperMotorPin4_Pin|StepperMotorPin3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, StepperMotorPin2_Pin|StepperMotorPin1_Pin|FiringPin_Pin|LD4_Pin
                          |LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RESET_SPI_Pin CS_SPI_Pin SPI_LED_Pin */
  GPIO_InitStruct.Pin = RESET_SPI_Pin|CS_SPI_Pin|SPI_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DC_SPI_Pin */
  GPIO_InitStruct.Pin = DC_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DC_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BOOT1_Pin ShowDatePin_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin|ShowDatePin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : RedButton_Pin GreenButton_Pin */
  GPIO_InitStruct.Pin = RedButton_Pin|GreenButton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : FanPin_Pin StepperMotorPin4_Pin StepperMotorPin3_Pin */
  GPIO_InitStruct.Pin = FanPin_Pin|StepperMotorPin4_Pin|StepperMotorPin3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : StepperMotorPin2_Pin StepperMotorPin1_Pin FiringPin_Pin LD4_Pin
                           LD3_Pin LD5_Pin LD6_Pin Audio_RST_Pin */
  GPIO_InitStruct.Pin = StepperMotorPin2_Pin|StepperMotorPin1_Pin|FiringPin_Pin|LD4_Pin
                          |LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : zeroCrossing_Pin */
  GPIO_InitStruct.Pin = zeroCrossing_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(zeroCrossing_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void delay_us(unsigned long delay_us)
{
	__HAL_TIM_SET_COUNTER(&htim5,0);
 	while(__HAL_TIM_GET_COUNTER(&htim5)<delay_us);
}
void delay_ms(unsigned long delay_ms)
{		//htim2.Init.Period razlika u odnosu na delay_us
		//__HAL_TIM_SET_COUNTER(&htim2,0);
		count_TIM2=0;
		while(count_TIM2<delay_ms);

}
unsigned long millis(void)
{

	unsigned long temp;
	temp= (unsigned long) HAL_GetTick();
	 return temp;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
	if(GPIO_Pin==zeroCrossing_Pin)
	{
		zero_croosing=1;//true
	}
	else
	{
		__NOP();
	}
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
