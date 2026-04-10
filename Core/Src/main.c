/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <llcc68.h>
#include <RYLR689.h>
#include <llcc68_hal.h>

/* stuff I included before
 * #include <string.h>
 * #include <stdbool.h>
 * #include <stdint.h>
 * #include <stdio.h>
 * #include <rylr689.h>
 */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define PIN_LOW(port, pin)   		HAL_GPIO_WritePin((port), (pin), GPIO_PIN_RESET)
#define PIN_HIGH(port, pin)  		HAL_GPIO_WritePin((port), (pin), GPIO_PIN_SET)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  //--------------------------------------Initialization Functions-------------------------------------//

  /* RF_TX_Init();
  llcc68_chip_status_t llcc68_status;
  llcc68_get_status(0, &llcc68_status);
  if(llcc68_status.chip_mode != LLCC68_CHIP_MODE_STBY_RC)	{RF_ERROR();}
  llcc68_check_cmd_status();

  //uint8_t reg_val[5] = {0};
  //uint8_t status = GetStatusRYLR689(reg_val);
  //	  	  __NOP();



   	  	  //SetPacketTypeRYLR689(0x01); //0x01=lora (pg. 82 datasheet)
   //uint8_t packet_type = GetPacketTypeRYLR689(reg_val);
   llcc68_pkt_type_t pkt_type;
   llcc68_get_pkt_type(0, &pkt_type);

   llcc68_check_cmd_status();
   //llcc68_get_status(0, &llcc68_status);

   if(pkt_type != LLCC68_PKT_TYPE_LORA)
   {
	   if(pkt_type == LLCC68_PKT_TYPE_GFSK)
	   {
		   llcc68_set_pkt_type(0, LLCC68_PKT_TYPE_LORA);
	   }
	   else {RF_ERROR();}
   }
   llcc68_check_cmd_status();
   //llcc68_get_status(0, &llcc68_status);


   llcc68_set_rf_freq(0, 915000000);
   //llcc68_get_status(0, &llcc68_status);
   llcc68_check_cmd_status();


   llcc68_pa_cfg_params_t pa_cfg =
      {
   		   0x04,
   		   0x03,  //range from 0x00 to 0x07, 0x07 being highest output power of +22dB
   		   0x00,
   		   0x01
      };
   llcc68_set_pa_cfg(0, &pa_cfg);
   llcc68_check_cmd_status();


   llcc68_set_tx_params(0, 14, LLCC68_RAMP_200_US);
   llcc68_check_cmd_status();


   llcc68_set_buffer_base_address(0, 0x00, 0x00);
   llcc68_check_cmd_status();

   //________________________________Write bata to Tx/Rx Data Buffer_____________________________
   //llcc68_write_buffer(context, offset, buffer, size);

   llcc68_mod_params_lora_t lora_mod_params =
   {
		   LLCC68_LORA_SF9,
		   LLCC68_LORA_BW_125,
		   LLCC68_LORA_CR_4_5,
		   0x00
   };
   llcc68_set_lora_mod_params(0, &lora_mod_params);
   llcc68_check_cmd_status();


   llcc68_pkt_params_lora_t lora_pkt_params =
   {
		   8,
		   LLCC68_LORA_PKT_EXPLICIT,
		   32, 		//Payload length, 32 is arbitrary value, set to max value expected
		   0x01,
		   0x00
   };
   llcc68_set_lora_pkt_params(0, &lora_pkt_params);
   llcc68_check_cmd_status();


   llcc68_irq_mask_t tx_irq_mask=
		   LLCC68_IRQ_TX_DONE |
		   LLCC68_IRQ_TIMEOUT ;
   llcc68_set_dio_irq_params(0, tx_irq_mask, tx_irq_mask, 0x0000, 0x0000);
   llcc68_check_cmd_status();


   llcc68_set_lora_sync_word(0, 0x12);  //0x34 is supposed to public, 0x12 is supposed to be private
   llcc68_check_cmd_status();


   llcc68_set_tx(0, LLCC68_MAX_TIMEOUT_IN_MS);
   llcc68_check_cmd_status();


   //_____________________________________Wait for DIO Flag_______________________________________
   bool RF_DIO = 0;
   while(RF_DIO == 0)
   {
	   RF_DIO = HAL_GPIO_ReadPin(LoRa_INT_1_GPIO_Port, LoRa_INT_1_Pin);
   }
   llcc68_irq_mask_t RF_IRQ = 0;
   llcc68_get_and_clear_irq_status(0, &RF_IRQ);  //there are seperate  get and clear functions
   llcc68_check_cmd_status();
*/

  //HAL_GPIO_WritePin(RFSW_V1_GPIO_Port, RFSW_V1_Pin, 0);
  //HAL_GPIO_WritePin(RFSW_V2_GPIO_Port, RFSW_V2_Pin, 1);

  //RF_TX_Init_and_Send();
 /* RF_TX_Init();

  uint8_t TX_buf[] = //bytes to send to ground board
		   {
				   0x55,
				   0x55, //all these are place holders for testing
				   0x55,
				   0x55
		   };

  RF_TX(TX_buf);*/



  __NOP();


  /*llcc68_rx_buffer_status_t RF_buf_status[] = {0};
  llcc68_get_rx_buffer_status(0, RF_buf_status);
  llcc68_check_cmd_status();

	   //uint8_t rx_buf_length = RF_buf_status->pld_len_in_bytes ;
	   //uint8_t rx_buf_start = RF_buf_status[1];
	   uint8_t Rx_buf[4] ;
	   llcc68_read_buffer(0, 0, Rx_buf, 4);

	__NOP();


RF_RX_Init_and_Receive();*/

  RF_RX_Init();

  uint8_t RX_buf[32];

  RF_RX_receive(RX_buf);




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //turn Led 1 on
	  	  HAL_GPIO_WritePin(leD1_GPIO_Port, leD1_Pin, 1);  //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
	  	  //turn LED 2 off
	  	  HAL_GPIO_WritePin(GPIOA, leD2_Pin, 0);



	  	  //turn LED 2 on
	  	  HAL_GPIO_WritePin(GPIOA, leD2_Pin, 1);

	  	  //wait 0.5s
	  	  HAL_Delay(500);


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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, leD1_Pin|leD2_Pin|SPI1_CS_RF_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RFSW_V1_Pin|RFSW_V2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : leD1_Pin leD2_Pin SPI1_CS_RF_Pin */
  GPIO_InitStruct.Pin = leD1_Pin|leD2_Pin|SPI1_CS_RF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RFSW_V1_Pin RFSW_V2_Pin */
  GPIO_InitStruct.Pin = RFSW_V1_Pin|RFSW_V2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Busy_Pin LoRa_INT_1_Pin */
  GPIO_InitStruct.Pin = Busy_Pin|LoRa_INT_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
