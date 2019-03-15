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
#include "flash.h"
#include "master_sel.h"
#include "gpio_root.h"

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

   // Initialize uart.
   uart_init(XPAR_UART_DEVICE_ID);

   // Initialize generic vars.
   generic_vars_init(&(sys.generic_vars));

   // Initialize ethernet and io.
   eth_init(XPAR_ETH_HIE_GPIO_ETH_DEVICE_ID,&(sys.eth));
   io_init(&sys);

   // Initialize interrupts.
   intc_init(XPAR_INTC_0_DEVICE_ID);

   // Initialize flash.
   flash_init(XPAR_SPI_FLASH_DEVICE_ID, &(sys.flash));

   // Check if root mode was switched on.
   gpio_root_init(XPAR_GPIO_ROOT_DEVICE_ID);
   if (gpio_root_sw())
   {
	   // Main loop.
	   unsigned int cmdBufferIdx = 0;
	   //uart_printMenu();

	   // Erase buffers.
	   uart_eraseBuffer(bufWords, USERCOMMANDLENGTH);
	   uart_eraseBuffer(userWord, USERCOMMANDLENGTH);

	   print("\033[2J");
	   print("\033[H");
	   print("Entering flash programming mode.\r\n");
	   print("Enter password: ");

	   while(1)
	   {
		   // Read char from serial port.
		   int n = uart_rcv(bufWords);
		   for (int i=0; i<n; i++)
		   {
			   // Enter was detected.
			   if (bufWords[i] == ASCII_CHAR_CR)
			   {
				   print("\r\n");

				   // Check password.
				   if (gpio_root_get_waitPass())
				   {
					   gpio_root_check_pass((char *)userWord);
				   }
				   else
				   {
					   // Parse command.
					   uart_parseCmd((char *)userWord);

					   print("\r\n");
				   }

				   // Clean command buffer.
				   uart_eraseBuffer(userWord, USERCOMMANDLENGTH);
				   cmdBufferIdx = 0;
			   }
			   // Other chars were detected that delete the command buffer.
			   else if (
					   bufWords[i] == ASCII_CHAR_BS 	||
					   bufWords[i] == ASCII_CHAR_CAN 	||
					   bufWords[i] == ASCII_CHAR_EM 	||
					   bufWords[i] == ASCII_CHAR_SUB 	||
					   bufWords[i] == ASCII_CHAR_ESC 	||
					   bufWords[i] == ASCII_CHAR_FS 	||
					   bufWords[i] == ASCII_CHAR_DEL
			   )
			   {
				   // Clean command buffer.
				   uart_eraseBuffer(userWord, USERCOMMANDLENGTH);
				   cmdBufferIdx = 0;

				   print("\33[2K");
				   print("\r\n");
			   }
			   else
			   {
				   //add character to the user vector
				   userWord[cmdBufferIdx] = bufWords[i];
				   cmdBufferIdx++;

				   if (gpio_root_get_waitPass())
				   {
					   print("*");
				   }
				   else
				   {
					   print((const char*)&bufWords[i]);
				   }
			   }
		   }
	   }
   }

   // Configure board IP.
   eth_change_ip(&(sys.eth.ipEth), sys.flash.ip.str);
   tdelay_s(2);

   mprint("--- ################### ---\n\r");
   mprint("--- Initializing System ---\n\r");
   mprint("--- ################### ---\n\r");
   mprint("\r\n");

   mprint("--- Initialize Leds ---\r\n");
   gpio_leds_init(XPAR_LEDS_GPIO_DEVICE_ID, &(sys.leds));

   mprint("--- Initialize Master Selection Logic ---\r\n");
   master_sel_init(&(sys.master_sel));

   mprint("--- Initialize Sync Generation Logic ---\r\n");
   sync_gen_init(&(sys.sync_gen));

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

   mprint("\r\n");
   mprint("--- ################# ---\r\n");
   mprint("--- Board Information ---\r\n");
   mprint("--- ################# ---\r\n");
   mprint("\r\n");

   char info[50];
   io_sprintf(info, "-> Fimrware Version:\t%s\r\n", sys.flash.firm_version.str);
   mprint(info);
   io_sprintf(info, "-> Fimrware Date:\t%s\r\n", sys.flash.firm_date.str);
   mprint(info);
   io_sprintf(info, "-> Fimrware Hash:\t%s\r\n", sys.flash.firm_hash.str);
   mprint(info);
   io_sprintf(info, "-> Software Version:\t%s\r\n", sys.flash.soft_version.str);
   mprint(info);
   io_sprintf(info, "-> Software Date:\t%s\r\n", sys.flash.soft_date.str);
   mprint(info);
   io_sprintf(info, "-> Software Hash:\t%s\r\n", sys.flash.soft_hash.str);
   mprint(info);
   io_sprintf(info, "-> Board ID:\t\t%x\r\n", sys.flash.id);
   mprint(info);
   io_sprintf(info, "-> Board IP:\t\t%s\r\n", sys.flash.ip.str);
   mprint(info);
   mprint("\r\n");

   mprint("--- ############################### ---\r\n");
   mprint("--- System Initialization Completed ---\r\n");
   mprint("--- ############################### ---\r\n");
   mprint("\r\n");
   tdelay_s(2);

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
   // Erase buffers.
   uart_eraseBuffer(bufWords, USERCOMMANDLENGTH);
   uart_eraseBuffer(userWord, USERCOMMANDLENGTH);

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
		   // Stop Sync Gen.
		   sync_gen_change_status(&(sys.sync_gen.stop), SYNC_GEN_STOP);

		   // Stop packer.
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

   //Xil_DCacheDisable();
   //Xil_ICacheDisable();
   return 0;
}
