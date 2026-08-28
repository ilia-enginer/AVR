












важно!!!!!!
- в функции static void MX_RTC_Init(void) закомментировать установку времени и даты…
//if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
//{
//  Error_Handler();
//}
//if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
//{
//  Error_Handler();
//}