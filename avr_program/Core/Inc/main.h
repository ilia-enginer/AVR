/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CHECK_EXT_POWER_Pin GPIO_PIN_0
#define CHECK_EXT_POWER_GPIO_Port GPIOA
#define CHECK_V_BAT_Pin GPIO_PIN_1
#define CHECK_V_BAT_GPIO_Port GPIOA
#define CHECK_MOTOR_ACTIVE_Pin GPIO_PIN_2
#define CHECK_MOTOR_ACTIVE_GPIO_Port GPIOA
#define CHECK_RELE_STARTER_Pin GPIO_PIN_3
#define CHECK_RELE_STARTER_GPIO_Port GPIOA
#define CHARGE_ON_OFF_Pin GPIO_PIN_4
#define CHARGE_ON_OFF_GPIO_Port GPIOA
#define TOUCH_CS_Pin GPIO_PIN_0
#define TOUCH_CS_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_1
#define TFT_CS_GPIO_Port GPIOB
#define IRQ_Pin GPIO_PIN_2
#define IRQ_GPIO_Port GPIOB
#define TFT_RST_Pin GPIO_PIN_10
#define TFT_RST_GPIO_Port GPIOB
#define TFT_DC_Pin GPIO_PIN_12
#define TFT_DC_GPIO_Port GPIOB
#define SD_CS_Pin GPIO_PIN_8
#define SD_CS_GPIO_Port GPIOA
#define RELE_OBSH_Pin GPIO_PIN_3
#define RELE_OBSH_GPIO_Port GPIOB
#define RELE_ZAJIG_Pin GPIO_PIN_4
#define RELE_ZAJIG_GPIO_Port GPIOB
#define RELE_STARTER_Pin GPIO_PIN_5
#define RELE_STARTER_GPIO_Port GPIOB
#define RELE_PODSOS_Pin GPIO_PIN_6
#define RELE_PODSOS_GPIO_Port GPIOB
#define RELE_SOST_Pin GPIO_PIN_7
#define RELE_SOST_GPIO_Port GPIOB
#define RELE_SOSTB8_Pin GPIO_PIN_8
#define RELE_SOSTB8_GPIO_Port GPIOB
#define RELE_SOSTB9_Pin GPIO_PIN_9
#define RELE_SOSTB9_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
