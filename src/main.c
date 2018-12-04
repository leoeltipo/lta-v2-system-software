/*
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A 
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR 
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION 
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE 
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO 
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO 
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE 
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * 
 *
 * This file is a generated sample test application.
 *
 * This application is intended to test and/or illustrate some 
 * functionality of your system.  The contents of this file may
 * vary depending on the IP in your system and may use existing
 * IP driver functions.  These drivers will be generated in your
 * SDK application project when you run the "Generate Libraries" menu item.
 *
 */

#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"

#include "defines.h"
#include "io_func.h"
#include "interrupt.h"
#include "uart.h"
#include "excecute.h"

system_state_t sys;

int main () 
{
   Xil_ICacheEnable();
   Xil_DCacheEnable();

   // Variables for command parsing.
   int status = 0;
   int nWords = 0;
   uint8_t bufWords[USERCOMMANDLENGTH];
   uint8_t userWord[USERCOMMANDLENGTH];
   int userWordInd = 0;
   char errStr[256];

   // Init generic vars, eth driver, io_func (stdout control) and interrupts.
   generic_vars_init(&(sys.generic_vars));
   eth_init(XPAR_ETH_HIE_GPIO_ETH_DEVICE_ID,&(sys.eth));
   io_init(&sys);
   intc_init(XPAR_INTC_0_DEVICE_ID);

   mprint("--- ################### ---\n\r");
   mprint("--- Initializing System ---\n\r");
   mprint("--- ################### ---\n\r");
   mprint("\r\n");

   mprint("--- Initialize Uart ---\r\n");
   uart_init(XPAR_UART_DEVICE_ID);

   mprint("--- Initialize Leds ---\r\n");
   gpio_leds_init(XPAR_LEDS_GPIO_DEVICE_ID, &(sys.leds));

   mprint("--- Initialize Exec function catalog ---\r\n");
   exec_init(&(sys.exec));

   mprint("--- Initialize ADC 15 MHz controller ---\r\n");
   adc_init(XPAR_GPIO_ADC_DEVICE_ID, &(sys.gpio_adc));

   mprint("--- Initialize Smart Buffer ---\r\n");
   smart_buffer_init(&(sys.smart_buffer));

   mprint("--- Initialize Packer ---\r\n");
   packer_init(&(sys.packer_sw));

   mprint("--- Initialize CDS core ---\r\n");
   cds_core_init(&(sys.cds));

   mprint("--- Initialize Sequencer ---\r\n");
   sequencer_init(&(sys.seq));

   mprint("--- Initialize Telemetry ---\r\n");
   telemetry_init(&(sys.telemetry), XPAR_SPI_TELEMETRY_DEVICE_ID, XPAR_GPIO_TELEMETRY_DEVICE_ID);

   mprint("--- Initialize Bias Voltages ---\r\n");
   ldos_init(&(sys.biases), XPAR_SPI_LDO_DEVICE_ID, XPAR_GPIO_LDO_DEVICE_ID);

   mprint("--- Initialize Clocks ---\r\n");
   dac_init(&(sys.clk_sw), &(sys.clks), XPAR_SPI_DAC_DEVICE_ID, XPAR_GPIO_DAC_DEVICE_ID);

   mprint("--- Initialize Voltage Switch ---\r\n");
   volt_sw_init(XPAR_SPI_VOLT_SW_DEVICE_ID, XPAR_GPIO_VOLT_SW_DEVICE_ID, &(sys.bias_sw), &(sys.gpio_sw));

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
   // Initialize buffers.
   for	(int u = 0; u<USERCOMMANDLENGTH; u = u +1)
   {
	   bufWords[u] = 0;
   }
   for	(int u = 0; u<USERCOMMANDLENGTH; u = u +1)
   {
	   userWord[u] = 0;
   }
   mprint("Accepting comands...\r\n");


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




   print("---Exiting main---\n\r");

   Xil_DCacheDisable();
   Xil_ICacheDisable();
   return 0;
}
