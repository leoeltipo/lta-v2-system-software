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

#include "dac.h"
#include "defines.h"
#include "io_func.h"
#include "interrupt.h"
#include "ldos.h"
#include "uart.h"
#include "leds.h"
#include "telemetry.h"
#include "volt_sw.h"
//#include "smart_buffer.h"
//#include "cds_core.h"
//#include "packer.h"

system_state_t sys;

int main () 
{
   Xil_ICacheEnable();
   Xil_DCacheEnable();

   float value;
   telemetry_source_t *tel_src;
   bias_status_t *bias;

   // Init generic vars, eth driver, io_func (stdout control) and interrupts.
   generic_vars_init(&(sys.generic_vars));
   eth_init(XPAR_GPIO_ETH_DEVICE_ID,&(sys.eth));
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

   //mprint("--- Initialize Smart Buffer ---\r\n");
   //smart_buffer_init(&(sys.smart_buffer));

   //mprint("--- Initialize packer ---\r\n");
   //packer_init(&(sys.packer_sw));

   //mprint("--- Initialize CDS core ---\r\n");
   //cds_core_init(&(sys.cds));

   mprint("--- Initialize sequencer ---\r\n");
   sequencer_init(&(sys.seq));

   mprint("--- Initialize Telemetry ---\r\n");
   telemetry_init(&(sys.telemetry), XPAR_SPI_TELEMETRY_DEVICE_ID, XPAR_GPIO_TELEMETRY_DEVICE_ID);

   mprint("--- Initialize Bias Voltages ---\r\n");
   ldos_init(&(sys.biases), XPAR_SPI_LDO_DEVICE_ID, XPAR_GPIO_LDO_DEVICE_ID);

   mprint("--- Initialize Clocks ---\r\n");
   dac_init(&(sys.clk_sw), &(sys.clks), XPAR_SPI_DAC_DEVICE_ID, XPAR_GPIO_DAC_DEVICE_ID);

   mprint("--- Initialize Voltage Switch ---\r\n");
   volt_sw_init(XPAR_SPI_VOLT_SW_DEVICE_ID, XPAR_GPIO_VOLT_SW_DEVICE_ID, &(sys.bias_sw), &(sys.gpio_sw));

   tel_src = &sys.telemetry.v_p1v8;
   telemetry_read(tel_src, &value);

   bias = &sys.biases.vsub;
   ldos_set_voltage(bias, 50);

   dac_set_voltage(&(sys.clks.v1ah), 7.5);
   dac_set_voltage(&(sys.clks.v1al), 7.5);

   dac_set_voltage(&(sys.clks.h1ah), 5);
   dac_set_voltage(&(sys.clks.h1al), 5);

   volt_sw_state_set(&(sys.bias_sw.sw_group.p15v_sw), &(sys.bias_sw.state), 1);

   //packer_change_sw_status(&(sys.packer_sw.source), PACKER_TRSRC_RAW_CHA);
   //packer_change_sw_status(&(sys.packer_sw.start), PACKER_START_ON);
   //packer_change_sw_status(&(sys.packer_sw.start), PACKER_START_OFF);

   while(1)
   {
	   tdelay_s(1);
	   mprint("Hola\r\n");
   }




   print("---Exiting main---\n\r");

   Xil_DCacheDisable();
   Xil_ICacheDisable();
   return 0;
}
