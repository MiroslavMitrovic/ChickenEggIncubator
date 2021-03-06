/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
SPI_HandleTypeDef hspi1;
#include <string.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RESET_SPI_Pin GPIO_PIN_2
#define RESET_SPI_GPIO_Port GPIOE
#define CS_SPI_Pin GPIO_PIN_3
#define CS_SPI_GPIO_Port GPIOE
#define SPI_LED_Pin GPIO_PIN_4
#define SPI_LED_GPIO_Port GPIOE
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define OTG_FS_PowerSwitchOn_Pin GPIO_PIN_0
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOC
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define DC_SPI_Pin GPIO_PIN_6
#define DC_SPI_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define RedButton_Pin GPIO_PIN_14
#define RedButton_GPIO_Port GPIOE
#define RedButton_EXTI_IRQn EXTI15_10_IRQn
#define GreenButton_Pin GPIO_PIN_15
#define GreenButton_GPIO_Port GPIOE
#define GreenButton_EXTI_IRQn EXTI15_10_IRQn
#define ShowDatePin_Pin GPIO_PIN_12
#define ShowDatePin_GPIO_Port GPIOB
#define FanPin_Pin GPIO_PIN_13
#define FanPin_GPIO_Port GPIOB
#define StepperMotorPin4_Pin GPIO_PIN_14
#define StepperMotorPin4_GPIO_Port GPIOB
#define StepperMotorPin3_Pin GPIO_PIN_15
#define StepperMotorPin3_GPIO_Port GPIOB
#define StepperMotorPin2_Pin GPIO_PIN_8
#define StepperMotorPin2_GPIO_Port GPIOD
#define StepperMotorPin1_Pin GPIO_PIN_9
#define StepperMotorPin1_GPIO_Port GPIOD
#define zeroCrossing_Pin GPIO_PIN_10
#define zeroCrossing_GPIO_Port GPIOD
#define zeroCrossing_EXTI_IRQn EXTI15_10_IRQn
#define FiringPin_Pin GPIO_PIN_11
#define FiringPin_GPIO_Port GPIOD
#define LD4_Pin GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOD
#define LD5_Pin GPIO_PIN_14
#define LD5_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_15
#define LD6_GPIO_Port GPIOD
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define Audio_RST_Pin GPIO_PIN_4
#define Audio_RST_GPIO_Port GPIOD
#define OTG_FS_OverCurrent_Pin GPIO_PIN_5
#define OTG_FS_OverCurrent_GPIO_Port GPIOD
#define MEMS_INT2_Pin GPIO_PIN_1
#define MEMS_INT2_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define SETPOINT_TEMP 									39 		//goal temperature
#define SETPOINT_HUM  									65 		//goal humidity
#define DHT12_ADDRESS_I2C 								0xB8	//ADDRESS DHT12 SENSOR
#define DS3231_ADDRESS_I2C								0xD0	//ADDRESS DS3231 RTC
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
