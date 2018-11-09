#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xspi.h"
#include "defines.h"

#include "interrupt.h"
#include "adc.h"
#include "CDS_core.h"
#include "packer.h"
#include "telemetry.h"
#include "ad5293.h"
#include "ad5371.h"
#include "max14802.h"
#include "sequencer.h"
#include "sequencer_samples.h"
#include "uart.h"
#include "excecute.h"
#include "eth.h"
#include "exec.h"
#include "smart_buffer.h"
#include "eth.h"
#include "io_func.h"

system_state_t sys;

int main() 
{
   //Xil_ICacheEnable();
   //Xil_DCacheEnable();

   // Init generic vars, eth driver, io_func (stdout control) and interrupts.
   generic_vars_init(&(sys.generic_vars));
   eth_init(XPAR_GPIO_ETH_DEVICE_ID,&(sys.eth));
   io_init(&sys);
   intc_init(XPAR_INTC_0_DEVICE_ID);

   mprint("--- ################### ---\n\r");
   mprint("--- Initializing System ---\n\r");
   mprint("--- ################### ---\n\r");
   mprint("\r\n");

   mprint("--- Initialize Leds ---\r\n");
   gpio_leds_init(XPAR_LEDS_GPIO_DEVICE_ID, &(sys.leds));

   mprint("--- Initialize Smart Buffer ---\r\n");
   smart_buffer_init(&(sys.smart_buffer));

   mprint("--- Initialize Exec function catalog ---\r\n");
   exec_init(&(sys.exec));

   mprint("--- Initialize Uart ---\r\n");
   uart_init(XPAR_UART_DEVICE_ID);

   mprint("--- Initialize sequencer ---\r\n");
   sequencer_init(&(sys.seq));

   mprint("--- Initialize packer ---\r\n");
   packer_init(&(sys.packer_sw));

   mprint("--- Initialize ADC 15 MHz controller ---\r\n");
   adc_init(XPAR_GPIO_ADC_DEVICE_ID, &(sys.gpio_adc));

   mprint("--- Initialize CDS core ---\r\n");
   cds_core_init(&(sys.cds));

   mprint("--- Initialize Bias Voltages ---\r\n");
   ad5293_init(&(sys.biases), XPAR_SPI_LDO_DEVICE_ID, XPAR_GPIO_LDO_DEVICE_ID);

   mprint("--- Initialize Clocks ---\r\n");
   ad5371_init(&(sys.clk_sw), &(sys.clks), XPAR_SPI_DAC_DEVICE_ID, XPAR_GPIO_DAC_DEVICE_ID);

   mprint("--- Initialize Voltage Switch ---\r\n");
   max14802_init(XPAR_SPI_VOLT_SW_DEVICE_ID, XPAR_GPIO_VOLT_SW_DEVICE_ID, &(sys.bias_sw), &(sys.gpio_sw));

   mprint("--- Initialize Telemetry ---\r\n");
   telemetry_init(&(sys.telemetry), XPAR_SPI_TELEMETRY_DEVICE_ID, XPAR_GPIO_TELEMETRY_DEVICE_ID);

	/*ad5293_creg_wr(1, 0, SPI_LDO_VDRAIN_SLAVE_SELECT);
	ad5293_creg_wr(1, 0, SPI_LDO_VDD_SLAVE_SELECT);
	ad5293_creg_wr(1, 0, SPI_LDO_VR_SLAVE_SELECT);
	ad5293_creg_wr(1, 0, SPI_LDO_VSUB_SLAVE_SELECT);*/

	// Enable LDOs.
	/*ad5293_sw_en(GPIO_LDO_VDRAIN, 1);
   	ad5293_sw_en(GPIO_LDO_VDD, 1);
   	ad5293_sw_en(GPIO_LDO_VR, 1);
   	ad5293_sw_en(GPIO_LDO_VSUB, 1);*/

   // Wait until the sequencer finishes executing.
   int status = 0;
   int nWords = 0;
   uint8_t bufWords[USERCOMMANDLENGTH];
   for	(int u = 0; u<USERCOMMANDLENGTH; u = u +1) {bufWords[u] = 0;}
   uint8_t userWord[USERCOMMANDLENGTH];
   for	(int u = 0; u<USERCOMMANDLENGTH; u = u +1) {userWord[u] = 0;}
   int userWordInd = 0;

   mprint("--- ############################### ---\r\n");
   mprint("--- System Initialization Completed ---\r\n");
   mprint("--- ############################### ---\r\n");
   mprint("\r\n");

   // Blink led to indicate end of initialization.
   for (int i=0; i<10; i++)
   {
	   gpio_leds_change_state(&(sys.leds.leds_group.led0), &(sys.leds.state), GPIO_LEDS_LED_ON);
	   tdelay_ms(50);
	   gpio_leds_change_state(&(sys.leds.leds_group.led0), &(sys.leds.state), GPIO_LEDS_LED_OFF);
	   tdelay_ms(50);
   }
   gpio_leds_change_state(&(sys.leds.leds_group.led0), &(sys.leds.state), GPIO_LEDS_LED_ON);

   	/* ********************************************** */
   	/* **************** MAIN LOOP ******************* */
   	/* ********************************************** */
   	mprint("Accepting comands...\r\n");
   	char errStr[256];
   	while (1)
   	{
   		nWords = uart_rcv(bufWords);
   		if (nWords == 0)
   		{
   			nWords = eth_mdata_get(bufWords);
   		}

   		for (int iChar = 0; iChar<nWords; iChar++)
   		{
   			if (bufWords[iChar] == ASCII_CHAR_CR)
   			{
   				if (sys.generic_vars.echo.value == GENERIC_VARS_ECHO_ON)
   				{
   					print("\r\n");
   				}

   				// Executing command...
   				gpio_leds_change_state(&(sys.leds.leds_group.led1), &(sys.leds.state), GPIO_LEDS_LED_ON);

   				// Execute command.
   				status = excecute_interpret(&sys,(char *)userWord, errStr);
   				mprint("Done\r\n");
   				if (status != 0)
   				{
   					mprint(errStr);
   				}

   				// Clean User Command Buffer.
   				for	(int u=0; u<USERCOMMANDLENGTH; u++)
   				{
   					userWord[u] = 0;
   				}
   				userWordInd = 0;

   				gpio_leds_change_state(&(sys.leds.leds_group.led1), &(sys.leds.state), GPIO_LEDS_LED_OFF);
   			}
   			else if (	bufWords[iChar] == ASCII_CHAR_BS 	||
						bufWords[iChar] == ASCII_CHAR_CAN 	||
						bufWords[iChar] == ASCII_CHAR_EM 	||
						bufWords[iChar] == ASCII_CHAR_SUB 	||
						bufWords[iChar] == ASCII_CHAR_ESC 	||
						bufWords[iChar] == ASCII_CHAR_FS 	||
						bufWords[iChar] == ASCII_CHAR_DEL
					)
   			{
   				// Clean User Command Buffer.
   				for	(int u=0; u<USERCOMMANDLENGTH; u++)
   				{
   					userWord[u] = 0;
   				}
   				userWordInd = 0;

   				if ( (int)sys.generic_vars.echo.value == GENERIC_VARS_ECHO_ON) {
   					print("\33[2K");
   					print("\r\n");
   				}
   			}
   			else
   			{
   				//add character to the user vector
   				userWord[userWordInd] = bufWords[iChar];
   				if ( (int)sys.generic_vars.echo.value == GENERIC_VARS_ECHO_ON) {
   					print((const char *) &(userWord[userWordInd]));
   				}
   				userWordInd = userWordInd + 1;
   			}

   		}

   		// Check if sequencer has finished.
   		if (sequencer_eos(&(sys.seq)))
   		{
   			packer_change_sw_status(&(sys.packer_sw.start),PACKER_START_OFF);
   			mprint("Read done\r\n");
   		}

   		// Check if transfer has finished.
   		if (smart_buffer_eot((&(sys.smart_buffer))))
   		{
   			smart_buffer_change_status(&(sys.smart_buffer.capture_start), SMART_BUFFER_CAPTURE_STOP);
   			smart_buffer_change_status(&(sys.smart_buffer.transfer_start), SMART_BUFFER_TRNASFER_STOP);
   			mprint("Transfer done\r\n");
   		}

   	}

   	//Xil_DCacheDisable();
   	//Xil_ICacheDisable();
   	return 0;
}

