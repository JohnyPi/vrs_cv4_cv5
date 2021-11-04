#include "main.h"
#include "assignment.h"

void SystemClock_Config(void);

uint8_t switch_state = 0;


int main(void)
{

   /*Default system setup*/
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   SystemClock_Config();

   NVIC_SetPriority(EXTI4_IRQn, 2); // priority setup
   NVIC_EnableIRQ(EXTI4_IRQn); // interrupt setup

   //set EXTI source PB4/
   SYSCFG->EXTICR[1] &= ~(0xFU);
   SYSCFG->EXTICR[1] |= (0x1U);

   //Enable interrupt from EXTI line 4
   EXTI->IMR |= EXTI_IMR_MR4;

   //Set EXTI trigger to falling edge
   EXTI->RTSR &= ~(EXTI_IMR_MR4);
   EXTI->FTSR |= EXTI_IMR_MR4;

   //GPIOB enable
   RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

   //GPIO PB4 input configuration
   GPIOB->MODER &= ~(GPIO_MODER_MODER4);
   GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR4);
   GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_4);
   GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4);
   GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR4_0);

   //GPIOA enable
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

   //GPIO PA4 output configuration
   GPIOA->MODER &= ~(GPIO_MODER_MODER4);
   GPIOA->MODER |= GPIO_MODER_MODER4_0;
   GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4);
   GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR4);
   GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4);


  while (1)
  {
	  if(switch_state)
	  {
		  GPIOA->BSRR |= GPIO_BSRR_BS_4;
		  for(uint16_t i=0; i<0xFF00; i++){}
		  GPIOA->BRR |= GPIO_BRR_BR_4;
		  for(uint16_t i=0; i<0xFF00; i++){}
	  }
	  else
	  {
		  GPIOA->BRR |= GPIO_BRR_BR_4;
	  }
  }

}

void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}


uint8_t checkButtonState(GPIO_TypeDef* PORT, uint8_t PIN, uint8_t edge, uint8_t samples_window, uint8_t samples_required)
{
		uint8_t button_state = 0, timeout = 0;

			while(button_state < samples_required && timeout < samples_window)
			{
				if(!(PORT->IDR & (edge << PIN))/*LL_GPIO_IsInputPinSet(PORT, PIN)*/)
				{
					button_state += 1;
				}
				else
				{
					button_state = 0;
				}

				timeout += 1;
				LL_mDelay(1);
			}

			if((button_state >= samples_required) && (timeout <= samples_window))
			{
				return 1;
			}
			else
			{
				return 0;
			}

}


void EXTI4_IRQHandler(void)
{
	if(checkButtonState(GPIO_PORT_BUTTON,
						GPIO_PIN_BUTTON,
						BUTTON_EXTI_TRIGGER,
						BUTTON_EXTI_SAMPLES_WINDOW,
						BUTTON_EXTI_SAMPLES_REQUIRED))
	{
		switch_state ^= 1;
	}

	/* Clear EXTI4 pending register flag */
	EXTI->PR |= (EXTI_PR_PIF4);

}

/* USER CODE BEGIN 4 */

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
