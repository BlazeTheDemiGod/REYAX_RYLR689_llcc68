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

//Infinite loop, is called in llcc68_check_cmd_status()
//ONLY use during testing
void RF_ERROR(void)
{
	for(uint8_t i = 0 ; i <10 ; i++) //while(1)
	{
		HAL_GPIO_WritePin(leD2_GPIO_Port, leD2_Pin, 0);
		HAL_Delay(200);
		HAL_GPIO_WritePin(leD2_GPIO_Port, leD2_Pin, 1);
		HAL_Delay(200);
	}
}

// Function that checks the status of the previous SPI command
//If error occurs it enters RF_ERROR loop
//ONLY use during testing
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


//Function that waits until busy bit of RF is low
//This function is blocking and only passes once busy pin is high or timeout occurs
bool llcc68_wait_while_busy_hal(uint32_t timeout_ms)
{
	uint32_t t0 = HAL_GetTick();
	  while (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_0) == GPIO_PIN_SET) // PH0 BUSY
	  {
	    if ((HAL_GetTick() - t0) >= timeout_ms) return false;
	  }
	  return true;
}


//Initialize and send data
void RF_TX_Init_and_Send(void)
{
	HAL_GPIO_WritePin(RFSW_V1_GPIO_Port, RFSW_V1_Pin, 0); //sets Reyax rylr689 in Tx mode
	HAL_GPIO_WritePin(RFSW_V2_GPIO_Port, RFSW_V2_Pin, 1); //sets Reyax rylr689 in Tx mode

	llcc68_chip_status_t llcc68_status;
	  llcc68_get_status(0, &llcc68_status); //check status
	  if(llcc68_status.chip_mode != LLCC68_CHIP_MODE_STBY_RC)	{RF_ERROR();}//if not in sandby send to error, can change this to if in sleep, wakeup function is called and status is set to standby
	  llcc68_check_cmd_status();//checks stastus and makes sure no errors have occured


	   llcc68_pkt_type_t pkt_type;
	   llcc68_get_pkt_type(0, &pkt_type); //gets packet type

	   llcc68_check_cmd_status();
	   //llcc68_get_status(0, &llcc68_status);

	   if(pkt_type != LLCC68_PKT_TYPE_LORA) //check if packet is LoRa
	   {
		   if(pkt_type == LLCC68_PKT_TYPE_GFSK) //if packet is GFSK change to LoRa
		   {
			   llcc68_set_pkt_type(0, LLCC68_PKT_TYPE_LORA); //set packet as lora
		   }
		   else {RF_ERROR();} //throw error is not lora or gfsk
	   }
	   llcc68_check_cmd_status();
	   //llcc68_get_status(0, &llcc68_status);


	   llcc68_set_rf_freq(0, 915000000); //set frequency to 915MHz
	   //llcc68_get_status(0, &llcc68_status);
	   llcc68_check_cmd_status();


	   /*llcc68_pa_cfg_params_t pa_cfg =
	   {
			   .pa_duty_cycle = 0x04,
			   .hp_max 		  = 0x03,  //range from 0x00 to 0x07, 0x07 being highest output power of +22dB
			   .device_sel 	  = 0x00,
			   .pa_lut		  = 0x01
	   };*/
	   llcc68_pa_cfg_params_t pa_cfg = //Power Amplifier configuration pg. 75 in datasheet
	      {
	   		   0x04, //paDutyCycle 0x04 optimal
	   		   0x07, //0x03,  // hpMax  range from 0x00 to 0x07, 0x07 being highest output power of +22dB.  0x07 is supposed to be optimal for +22dB
	   		   0x00, //deviceSel
	   		   0x01  //paLut
	      };
	   llcc68_set_pa_cfg(0, &pa_cfg); //sets the pa config
	   llcc68_check_cmd_status();


	   llcc68_set_tx_params(0, 22, LLCC68_RAMP_200_US); //(0, 14, LLCC68_RAMP_200_US) //power in dBm
	   llcc68_check_cmd_status();


	   llcc68_set_buffer_base_address(0, 0x00, 0x00); //sets starting address inside the TX/RX buffer
	   llcc68_check_cmd_status();


	   /***************************Write bata to Tx/Rx Data Buffer************************/
	   uint8_t TX_buf[] = //bytes to send to ground board
	   {
			   0x00,
			   0x01, //all these are place holders for testing
			   0x10,
			   0x22
	   };
	   uint8_t TX_offset = 0;
	   llcc68_write_buffer(0, TX_offset, TX_buf, sizeof(TX_buf)); //moves the data into the buffer
	   llcc68_check_cmd_status();


	   llcc68_mod_params_lora_t lora_mod_params = //LoRa modulation parameters pg.86
	   {
			   LLCC68_LORA_SF9, //spreading factor of 9
			   LLCC68_LORA_BW_125, ///bandwidth of 125(kHz)
			   LLCC68_LORA_CR_4_5, //Coding rate of 4/5
			   0x00
	   };
	   llcc68_set_lora_mod_params(0, &lora_mod_params);
	   llcc68_check_cmd_status();


	   llcc68_pkt_params_lora_t lora_pkt_params = //pg. 91
	   {
			   8, //pramble length in symbols
			   LLCC68_LORA_PKT_EXPLICIT, // explicit includes payload length, coding rate, and CRC
			   32, 		//Payload length or max length, 32 is relatively arbitrary value, set to max value expected
			   0x01, //CRC 0x01 is enabled
			   0x00  //IQ setup, 0x00 is standard
	   };
	   llcc68_set_lora_pkt_params(0, &lora_pkt_params);
	   llcc68_check_cmd_status();


	   llcc68_irq_mask_t tx_irq_mask= // flags that will make DIO pin set
			   LLCC68_IRQ_TX_DONE |
			   LLCC68_IRQ_TIMEOUT ;
	   llcc68_set_dio_irq_params(0, tx_irq_mask, tx_irq_mask, 0x0000, 0x0000);
	   llcc68_check_cmd_status();


	   llcc68_set_lora_sync_word(0, 0x12);  //0x34 is supposed to public, 0x12 is supposed to be private
	   llcc68_check_cmd_status();


	   llcc68_set_tx(0, LLCC68_MAX_TIMEOUT_IN_MS); //sets chip to transmit, need to set the Rf switch in Tx before this
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

void RF_TX_Init(void)
{
	HAL_GPIO_WritePin(RFSW_V1_GPIO_Port, RFSW_V1_Pin, 0); //sets Reyax rylr689 in Tx mode
	HAL_GPIO_WritePin(RFSW_V2_GPIO_Port, RFSW_V2_Pin, 1); //sets Reyax rylr689 in Tx mode


	llcc68_chip_status_t llcc68_status;
	  llcc68_get_status(0, &llcc68_status); //check status
	  if(llcc68_status.chip_mode != LLCC68_CHIP_MODE_STBY_RC)	{RF_ERROR();}//if not in sandby send to error, can change this to if in sleep, wakeup function is called and status is set to standby
	  llcc68_check_cmd_status();//checks stastus and makes sure no errors have occured


	   llcc68_pkt_type_t pkt_type;
	   llcc68_get_pkt_type(0, &pkt_type); //gets packet type

	   llcc68_check_cmd_status();
	   //llcc68_get_status(0, &llcc68_status);

	   if(pkt_type != LLCC68_PKT_TYPE_LORA) //check if packet is LoRa
	   {
		   if(pkt_type == LLCC68_PKT_TYPE_GFSK) //if packet is GFSK change to LoRa
		   {
			   llcc68_set_pkt_type(0, LLCC68_PKT_TYPE_LORA); //set packet as lora
		   }
		   else {RF_ERROR();} //throw error is not lora or gfsk
	   }
	   llcc68_check_cmd_status();
	   //llcc68_get_status(0, &llcc68_status);


	   llcc68_set_rf_freq(0, 915000000); //set frequency to 915MHz
	   //llcc68_get_status(0, &llcc68_status);
	   llcc68_check_cmd_status();


	   llcc68_pa_cfg_params_t pa_cfg = //Power Amplifier configuration pg. 75 in datasheet
	      {
	   		   0x04, //paDutyCycle 0x04 optimal
	   		   0x07, //0x03,  // hpMax  range from 0x00 to 0x07, 0x07 being highest output power of +22dB.  0x07 is supposed to be optimal for +22dB
	   		   0x00, //deviceSel
	   		   0x01  //paLut
	      };
	   llcc68_set_pa_cfg(0, &pa_cfg); //sets the pa config
	   llcc68_check_cmd_status();


	   llcc68_set_tx_params(0, 22, LLCC68_RAMP_200_US); //(0, 14, LLCC68_RAMP_200_US) //power in dBm
	   llcc68_check_cmd_status();


	   llcc68_set_buffer_base_address(0, 0x00, 0x00); //sets starting address inside the TX/RX buffer
	   llcc68_check_cmd_status();


	   llcc68_mod_params_lora_t lora_mod_params = //LoRa modulation parameters pg.86
	   {
			   LLCC68_LORA_SF9, //spreading factor of 9
			   LLCC68_LORA_BW_125, ///bandwidth of 125(kHz)
			   LLCC68_LORA_CR_4_5, //Coding rate of 4/5
			   0x00
	   };
	   llcc68_set_lora_mod_params(0, &lora_mod_params);
	   llcc68_check_cmd_status();


	   llcc68_pkt_params_lora_t lora_pkt_params = //pg. 91
	   {
			   8, //pramble length in symbols
			   LLCC68_LORA_PKT_EXPLICIT, // explicit includes payload length, coding rate, and CRC
			   32, 		//Payload length or max length, 32 is relatively arbitrary value, set to max value expected
			   0x01, //CRC 0x01 is enabled
			   0x00  //IQ setup, 0x00 is standard
	   };
	   llcc68_set_lora_pkt_params(0, &lora_pkt_params);
	   llcc68_check_cmd_status();


	   llcc68_irq_mask_t tx_irq_mask= // flags that will make DIO pin set
			   LLCC68_IRQ_TX_DONE |
			   LLCC68_IRQ_TIMEOUT ;
	   llcc68_set_dio_irq_params(0, tx_irq_mask, tx_irq_mask, 0x0000, 0x0000);
	   llcc68_check_cmd_status();


	   llcc68_set_lora_sync_word(0, 0x12);  //0x34 is supposed to public, 0x12 is supposed to be private
	   llcc68_check_cmd_status();
}


void RF_TX_send(uint8_t* tx_payload, uint8_t tx_size)
{  //May add logic to make sure module is in transmit mode before running this function
	   /***************************Write bata to Tx/Rx Data Buffer***********************/
	/*uint8_t TX_buf[] = //bytes to send to ground board
		   {
				   0x00,
				   0x01, //all these are place holders for testing
				   0x10,
				   0x22
		   };*/
	   //uint8_t TX_offset = 0;
	   llcc68_write_buffer(0, 0, tx_payload, tx_size); //moves the data into the buffer  (0, TX_offset, TX_buf, sizeof(TX_buf)
	   llcc68_check_cmd_status();


	   llcc68_set_tx(0, LLCC68_MAX_TIMEOUT_IN_MS); //sets chip to transmit, need to set the Rf switch in Tx before this
	   llcc68_check_cmd_status();


	   /***********************************Wait for DIO Flag************************/
	   bool RF_DIO = 0;
	   while(RF_DIO == 0)
	   {
		   RF_DIO = HAL_GPIO_ReadPin(LoRa_INT_1_GPIO_Port, LoRa_INT_1_Pin);
	   }
	   llcc68_irq_mask_t RF_IRQ = 0;
	   llcc68_get_irq_status(0, &RF_IRQ);
	   llcc68_get_and_clear_irq_status(0, &RF_IRQ);  //there are seperate  get and clear functions
	   llcc68_check_cmd_status();
}


void RF_RX_Init_and_Receive(void)
{
	HAL_GPIO_WritePin(RFSW_V1_GPIO_Port, RFSW_V1_Pin, 1); //sets Reyax rylr689 in Rx mode
	HAL_GPIO_WritePin(RFSW_V2_GPIO_Port, RFSW_V2_Pin, 0); //sets Reyax rylr689 in Rx mode


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


	   llcc68_set_buffer_base_address(0, 0x00, 0x00);
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


	   llcc68_irq_mask_t rx_irq_mask= //may need to add CRC, Header SyncWord, Valid and error
			   LLCC68_IRQ_RX_DONE |
			   LLCC68_IRQ_TIMEOUT ;
	   llcc68_set_dio_irq_params(0, rx_irq_mask, rx_irq_mask, 0x0000, 0x0000);
	   llcc68_check_cmd_status();


	   llcc68_set_lora_sync_word(0, 0x12);  //0x34 is supposed to public, 0x12 is supposed to be private
	   llcc68_check_cmd_status();


	   llcc68_set_rx(0, LLCC68_MAX_TIMEOUT_IN_MS);
	   llcc68_check_cmd_status();


	   /***********************************Wait for DIO Flag************************/
	   bool RF_DIO = 0;
	   while(RF_DIO == 0)
	   {
		   RF_DIO = HAL_GPIO_ReadPin(LoRa_INT_1_GPIO_Port, LoRa_INT_1_Pin);
	   }

	   llcc68_handle_rx_done(0); //stops internal timer to prevent possible timeout and reset
	   llcc68_check_cmd_status();

	   llcc68_irq_mask_t RF_IRQ = 0;
	   llcc68_get_and_clear_irq_status(0, &RF_IRQ);  //there are seperate  get and clear functions, probably need to add logic to check for timeout or error
	   llcc68_check_cmd_status();


	   	   //llcc68_get_lora_stats(context, stats);
	   	   //llcc68_get_lora_pkt_status(context, pkt_status);
	   llcc68_rx_buffer_status_t RF_buf_status[] = {0};
	   llcc68_get_rx_buffer_status(0, RF_buf_status);
	   llcc68_check_cmd_status();

	   //uint8_t rx_buf_length = RF_buf_status->pld_len_in_bytes ;
	   //uint8_t rx_buf_start = RF_buf_status[1];
	   uint8_t Rx_buf[RF_buf_status->pld_len_in_bytes] ;
	   llcc68_read_buffer(0, RF_buf_status->buffer_start_pointer, Rx_buf, RF_buf_status->pld_len_in_bytes);
}

void RF_RX_Init(void)
{
	HAL_GPIO_WritePin(RFSW_V1_GPIO_Port, RFSW_V1_Pin, 1); //sets Reyax rylr689 in Rx mode
	HAL_GPIO_WritePin(RFSW_V2_GPIO_Port, RFSW_V2_Pin, 0); //sets Reyax rylr689 in Rx mode


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


	   llcc68_set_buffer_base_address(0, 0x00, 0x00);
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


	   llcc68_irq_mask_t rx_irq_mask= //may need to add CRC, Header SyncWord, Valid and error
			   LLCC68_IRQ_RX_DONE |
			   LLCC68_IRQ_TIMEOUT ;
	   llcc68_set_dio_irq_params(0, rx_irq_mask, rx_irq_mask, 0x0000, 0x0000);
	   llcc68_check_cmd_status();


	   llcc68_set_lora_sync_word(0, 0x12);  //0x34 is supposed to public, 0x12 is supposed to be private
	   llcc68_check_cmd_status();
}

 void RF_RX_receive(uint8_t* rx_buf)
{
		llcc68_set_rx(0, LLCC68_MAX_TIMEOUT_IN_MS);
	   llcc68_check_cmd_status();


	   /***********************************Wait for DIO Flag************************/
	   bool RF_DIO = 0;
	   while(RF_DIO == 0)
	   {
		   RF_DIO = HAL_GPIO_ReadPin(LoRa_INT_1_GPIO_Port, LoRa_INT_1_Pin);
	   }

	   llcc68_handle_rx_done(0); //stops internal timer to prevent possible timeout and reset
	   llcc68_check_cmd_status();

	   llcc68_irq_mask_t RF_IRQ = 0;
	   llcc68_get_and_clear_irq_status(0, &RF_IRQ);  //there are seperate  get and clear functions, probably need to add logic to check for timeout or error
	   llcc68_check_cmd_status();


	   	   //llcc68_get_lora_stats(context, stats);
	   	   //llcc68_get_lora_pkt_status(context, pkt_status);
	   llcc68_rx_buffer_status_t RF_buf_status[] = {0};
	   llcc68_get_rx_buffer_status(0, RF_buf_status);
	   llcc68_check_cmd_status();

	   //uint8_t rx_buf_length = RF_buf_status->pld_len_in_bytes ;
	   //uint8_t rx_buf_start = RF_buf_status[1];
	   //uint8_t Rx_buf[RF_buf_status->pld_len_in_bytes] ;
	   llcc68_read_buffer(0, RF_buf_status->buffer_start_pointer, rx_buf, RF_buf_status->pld_len_in_bytes);
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
