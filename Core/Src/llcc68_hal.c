/**
 * @file      llcc68_hal.h
 *
 * @brief     Hardware Abstraction Layer for LLCC68
 *

 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <main.h>
#include <llcc68_hal.h>
#include <llcc68.h>

#include <stdint.h>
#include <stdbool.h>

extern SPI_HandleTypeDef hspi1;

/* Might needs some of these, might not
#include <stddef.h>
#include "llcc68.h"
#include "llcc68_hal.h"
#include "llcc68_regs.h"
*/

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */
#ifndef PIN_LOW
#define PIN_LOW(port, pin)   		HAL_GPIO_WritePin((port), (pin), GPIO_PIN_RESET)
#endif

#ifndef PIN_HIGH
#define PIN_HIGH(port, pin)  		HAL_GPIO_WritePin((port), (pin), GPIO_PIN_SET)
#endif

#define RF_NSS_LOW 					HAL_GPIO_WritePin(SPI1_CS_RF_GPIO_Port, SPI1_CS_RF_Pin, RESET)
#define RF_NSS_HIGH 				HAL_GPIO_WritePin(SPI1_CS_RF_GPIO_Port, SPI1_CS_RF_Pin, SET)


/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */



/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */



/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

void RF_ERROR(void)
{
	while(1)
	{
		HAL_GPIO_WritePin(leD2_GPIO_Port, leD2_Pin, 0);
		HAL_Delay(200);
		HAL_GPIO_WritePin(leD2_GPIO_Port, leD2_Pin, 1);
		HAL_Delay(200);
	}
}


void llcc68_check_cmd_status(void)
{
	llcc68_chip_status_t llcc68_status;
	  llcc68_get_status(0, &llcc68_status);
	  if(llcc68_status.cmd_status == LLCC68_CMD_STATUS_CMD_TIMEOUT ||
		 llcc68_status.cmd_status == LLCC68_CMD_STATUS_CMD_PROCESS_ERROR ||
		 llcc68_status.cmd_status == LLCC68_CMD_STATUS_CMD_EXEC_FAILURE )
	  {
		  RF_ERROR();
	  }
}


bool llcc68_wait_while_busy_hal(uint32_t timeout_ms)
{
	uint32_t t0 = HAL_GetTick();
	  while (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_0) == GPIO_PIN_SET) // PH0 BUSY
	  {
	    if ((HAL_GetTick() - t0) >= timeout_ms) return false;
	  }
	  return true;
}

void RF_TX_Init_and_Send(void)
{
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


	   /*llcc68_pa_cfg_params_t pa_cfg =
	   {
			   .pa_duty_cycle = 0x04,
			   .hp_max 		  = 0x03,  //range from 0x00 to 0x07, 0x07 being highest output power of +22dB
			   .device_sel 	  = 0x00,
			   .pa_lut		  = 0x01
	   };*/
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


	   /***************************Write bata to Tx/Rx Data Buffer************************/
	   uint8_t TX_buf[] =
	   {
			   0x00,
			   0x01,
			   0x10,
			   0x22
	   };
	   uint8_t TX_offset = 0;
	   llcc68_write_buffer(0, TX_offset, TX_buf, sizeof(TX_buf));
	   llcc68_check_cmd_status();


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


	   /***********************************Wait for DIO Flag************************/
	   bool RF_DIO = 0;
	   while(RF_DIO == 0)
	   {
		   RF_DIO = HAL_GPIO_ReadPin(LoRa_INT_1_GPIO_Port, LoRa_INT_1_Pin);
	   }
	   llcc68_irq_mask_t RF_IRQ = 0;
	   llcc68_get_and_clear_irq_status(0, &RF_IRQ);  //there are seperate  get and clear functions
	   llcc68_check_cmd_status();
}


/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

llcc68_hal_status_t llcc68_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )
{

/* My code, no error return, probably returns wrong status
	uint8_t status[data_length]; //= {0};

	llcc68_wait_while_busy_hal(100);

	PIN_LOW(SPI1_CS_RF_GPIO_Port, SPI1_CS_RF_Pin);
	 	  __NOP(); //for (volatile int i = 0; i < 200; i++) __NOP();

	HAL_SPI_Transmit(&hspi1, command, command_length, 100);
	HAL_SPI_TransmitReceive(&hspi1, data, status, data_length, 100);


	PIN_HIGH(SPI1_CS_RF_GPIO_Port, SPI1_CS_RF_Pin);

	return status[data_length - 1]; */

	llcc68_wait_while_busy_hal(100);
	RF_NSS_LOW;

	if (HAL_SPI_Transmit(&hspi1, (uint8_t*)command, command_length, 100) != HAL_OK) { RF_NSS_HIGH; return LLCC68_HAL_STATUS_ERROR; }
	    if ((data != NULL) && (data_length > 0))
	    {
	        if (HAL_SPI_Transmit(&hspi1, (uint8_t*)data, data_length, 100) != HAL_OK) { RF_NSS_HIGH; return LLCC68_HAL_STATUS_ERROR; }
	    }
	    RF_NSS_HIGH;
	    return LLCC68_HAL_STATUS_OK;



}

llcc68_hal_status_t llcc68_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{

	llcc68_wait_while_busy_hal(100);
	RF_NSS_LOW;

	if (HAL_SPI_Transmit(&hspi1, (uint8_t*)command, command_length, 100) != HAL_OK)  { RF_NSS_HIGH; return LLCC68_HAL_STATUS_ERROR; }
	if (HAL_SPI_Receive (&hspi1, data, data_length, 100) != HAL_OK)                  { RF_NSS_HIGH; return LLCC68_HAL_STATUS_ERROR; }

	RF_NSS_HIGH;
	return LLCC68_HAL_STATUS_OK;
}

llcc68_hal_status_t llcc68_hal_reset( const void* context )
{
	//This does nothing because reset line is not attached
	__NOP();
	return LLCC68_HAL_STATUS_ERROR;
}

llcc68_hal_status_t llcc68_hal_wakeup( const void* context )
{
	/* Wakeup by sending a dummy byte with NSS low */
	    uint8_t nop = 0x00;
	    RF_NSS_LOW;
	    HAL_SPI_Transmit(&hspi1, &nop, 1, 100);
	    RF_NSS_HIGH;
	    llcc68_wait_while_busy_hal(100);
	    return LLCC68_HAL_STATUS_OK;
}


/* --- EOF ------------------------------------------------------------------ */
