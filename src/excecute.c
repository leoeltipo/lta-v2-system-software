/* excecute.c
 *
 * @file excecute.c
 * @brief excecute comand users
 * @author Guillermo Fernandez Moroni (fmoroni.guillermo@gmail.com)
 */


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <xil_printf.h>
#include "xil_io.h"
#include <stdlib.h>


#include "ad5293.h"
#include "ad5371.h"
#include "max14802.h"
#include "sequencer.h"
#include "packer.h"
#include "uart.h"
#include "adc.h"
#include "telemetry.h"
#include "excecute.h"
#include "leds.h"
#include "smart_buffer.h"
#include "io_func.h"
#include "eth.h"


int excecute_interpret(system_state_t *sys, char *userWord, char *errStr)
{
	//separete with white spaces
	user_words_t commandWord[USERNUMBWORDS];
	char *token;
	char *rest;

	rest = userWord;
	// walk through other tokens
	int wordInd = 0;
	while( (token = strtok_r(rest, " ", &rest)) ) {
		//save word
		strcpy(commandWord[wordInd].word,token);
		wordInd = wordInd + 1;
	}

	free(token);
	free(rest);

   switch(wordInd){
   case NO_WORD:
	   io_sprintf(errStr, "Not a valid command\r\n");
	   return -1;
	   break;

   case ONE_WORD:
	   if (strcmp(commandWord[0].word,"help") == 0)
	   {
		   excecute_help(sys);
		   return 0;
	   }
	   else
	   {
		   io_sprintf(errStr, "Invalid command: %s\r\n",commandWord[0].word);
		   return -1;
	   }
	   break;

   case TWO_WORD:
	   //get command
	   if (strcmp(commandWord[0].word,"get")==0)
	   	{
	   		return excecute_get(sys, commandWord[1].word, errStr);
	   	}

	   // exec command.
	   else if (strcmp(commandWord[0].word,"exec")==0)
	   {
		   int status = execute_exec(sys, commandWord[1].word);
		   if (status != 0)
		   {
			   io_sprintf(errStr, "Invalid command: %s %s\r\n",commandWord[0].word,commandWord[1].word);
		   }
		   return status;
	   }
	   else
	   {
		   io_sprintf(errStr, "Invalid command: %s %s\r\n",commandWord[0].word,commandWord[1].word);
		   return -1;
	   }
	   break;

   case THREE_WORDS:
	   // set command.
	   if (strcmp(commandWord[0].word,"set")==0)
	   {
		   return excecute_set(	sys, commandWord[1].word,commandWord[2].word,errStr);
	   }

	   // get telemetry command.
	   else if ( (strcmp(commandWord[0].word,"get")==0) && (strcmp(commandWord[1].word,"telemetry")==0) )
	   {
		   // Special case for telemetry.
		   int status = excecute_get_telemetry(sys, commandWord[2].word);
		   if (status != 0)
		   {
			   io_sprintf(errStr, "Invalid command: %s %s %s\r\n",commandWord[0].word,commandWord[1].word,commandWord[2].word);
		   }
		   return status;
	   }
	   else
	   {
		   io_sprintf(errStr, "Invalid command: %s %s %s\r\n",commandWord[0].word,commandWord[1].word,commandWord[2].word);
		   return -1;
	   }

	   break;

   case FOUR_WORDS:
	   //extra commands

		if (strcmp(commandWord[0].word,"set")==0)
		{
			if (strcmp(commandWord[1].word,"seq")==0)
			{
				int status = execute_setseq(sys, commandWord[2].word, commandWord[3].word);
				if (status != 0)
				{
					io_sprintf(errStr, "### Could not write %d @%d\r\n", commandWord[3].word, commandWord[2].word);
					return -1;
				}
				return status;
			}
			else
			{
				io_sprintf(errStr, "Invalid command: %s %s %s %s\r\n",commandWord[0].word,commandWord[1].word,commandWord[2].word,commandWord[3].word);
				return -1;
			}
		}
		else
	   	{
			io_sprintf(errStr, "Invalid command: %s %s %s %s\r\n",commandWord[0].word,commandWord[1].word,commandWord[2].word,commandWord[3].word);
			return -1;
	   	}
		break;

   default:
	   io_sprintf(errStr, "Not a valid command\r\n");
	   return -1;
	   break;
   }
}

void excecute_help(system_state_t *sys)
{
	mprint("\r\n");
	mprint("### LTA Help ###\r\n");
	mprint("-> help\r\n");
	mprint("-> set <variable> <value>\r\n");
	mprint("-> get <variable>\r\n");
	mprint("-> get all\r\n");
	mprint("-> get telemetry <variable>\r\n");
	mprint("-> get telemetry help\r\n");
	mprint("-> get telemetry all\r\n");
	mprint("-> exec <function>\r\n");
	mprint("-> exec help\r\n");
	mprint("\r\n");
}

int excecute_get(system_state_t *sys, const char *varID, char *errStr) {

	int flag_all = 0;
	char str[50];

	// get all system variables
	if (strcmp(varID,"all")==0)
	{
		flag_all = 1;
	}

	// Clocks' voltages.
	if (flag_all) mprint("### Clocks' Voltages ###\r\n");

	clk_status_t *clk = (clk_status_t *) &(sys->clks);
	int nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		if (flag_all)
		{
			// Print value.
			io_sprintf(str,"%s = %f\r\n", clk->name, clk->value);
			mprint(str);
		}
		else if (strcmp(varID, clk->name)==0)
		{
			// Print value.
			io_sprintf(str,"%s = %f\r\n", clk->name, clk->value);
			mprint(str);

		 	return 0;
		}
		clk++;
	}
	if (flag_all) mprint("\r\n");

	// Clocks' switches.
	if (flag_all) mprint("### Clocks' switches ###\r\n");
	clk_sw_status_t *clk_sw = (clk_sw_status_t *) &(sys->clk_sw.sw_group);
	int nClocks_sw = sizeof(clk_sw_group_status_t)/sizeof(clk_sw_status_t);
	for(int i = 0; i < nClocks_sw; i++)
	{
		if (flag_all)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", clk_sw->name, clk_sw->status);
			mprint(str);
		}
		else if (strcmp(varID,clk_sw->name)==0)
		{
			// Print value.
		 	io_sprintf(str, "%s = %d\r\n", clk_sw->name, clk_sw->status);
		 	mprint(str);

		 	return 0;
		}
		clk_sw++;
	}
	if (flag_all) mprint("\r\n");

	// Bias Voltages.
	if (flag_all) mprint("### Bias Voltages ###\r\n");
	bias_status_t *bias = (bias_status_t *) &(sys->biases);
	int nBiases = sizeof(bias_group_status_t)/sizeof(bias_status_t);
	for(int i = 0; i < nBiases; i++)
	{
		if (flag_all)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %f\r\n", bias->name, bias->value);
		 	mprint(str);
		}
		else if (strcmp(varID,bias->name)==0)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %f\r\n", bias->name, bias->value);
		 	mprint(str);

		 	return 0;
		}
		bias++;
	}
	if (flag_all) mprint("\r\n");

	// Bias's Switches.
	if (flag_all) mprint("### Bias Switches ###\r\n");
	bias_sw_status_t *bias_sw = (bias_sw_status_t *) &(sys->bias_sw.sw_group);
	int nbias_sw = sizeof(bias_sw_group_status_t)/sizeof(bias_sw_status_t);
	for(int i = 0; i < nbias_sw; i++)
	{
		if (flag_all)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", bias_sw->name, bias_sw->status);
		 	mprint(str);
		}
		else if (strcmp(varID, bias_sw->name)==0)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", bias_sw->name, bias_sw->status);
		 	mprint(str);

		 	return 0;
		}
		bias_sw++;
	}
	if (flag_all) mprint("\r\n");

	// Packer.
	if (flag_all) mprint("### Packer ###\r\n");
	packer_sw_status_t *packer_sw = (packer_sw_status_t *) &(sys->packer_sw);
	int nPacker_sw = sizeof(packer_sw_group_status_t)/sizeof(packer_sw_status_t);
	for(int i = 0; i < nPacker_sw; i++)
	{
		if (flag_all)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", packer_sw->name, packer_sw->status);
		 	mprint(str);
		}
		else if (strcmp(varID, packer_sw->name)==0)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", packer_sw->name, packer_sw->status);
		 	mprint(str);

		 	return 0;
		}
		packer_sw++;
	}
	if (flag_all) mprint("\r\n");

	// ADC.
	if (flag_all) mprint("### ADC ###\r\n");
	adc_sw_status_t *adc_sw = (adc_sw_status_t *) &(sys->gpio_adc.sw_group);
	int nAdc_sw = sizeof(adc_sw_group_status_t)/sizeof(adc_sw_status_t);
	for(int i = 0; i < nAdc_sw; i++)
	{
		if (flag_all)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", adc_sw->name, adc_sw->status);
		 	mprint(str);
		}
		else if (strcmp(varID, adc_sw->name)==0)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", adc_sw->name, adc_sw->status);
		 	mprint(str);

		 	return 0;
		}
		adc_sw++;
	}
	if (flag_all) mprint("\r\n");

	// Sequencer.
	if (flag_all) mprint("### Sequencer ###\r\n");
	seq_sw_status_t *seq_sw = (seq_sw_status_t *) &(sys->seq.sw_group);
	int nSeq_sw = sizeof(seq_sw_group_status_t)/sizeof(seq_sw_status_t);
	for(int i = 0; i < nSeq_sw; i++)
	{
		if (flag_all)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", seq_sw->name, seq_sw->status);
		 	mprint(str);
		}
		else if (strcmp(varID, seq_sw->name)==0)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", seq_sw->name, seq_sw->status);
		 	mprint(str);

		 	return 0;
		}
		seq_sw++;
	}
	if (flag_all) mprint("\r\n");

	// Correlated Double Sampling.
	if (flag_all) mprint("### Correlated Double Sampling ###\r\n");
	cds_var_status_t *cds_var = (cds_var_status_t *) &(sys->cds);
	int nCds_var = sizeof(cds_var_group_status_t)/sizeof(cds_var_status_t);
	for(int i = 0; i < nCds_var; i++)
	{
		if (flag_all)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", cds_var->name, cds_var->value);
		 	mprint(str);
		}
		else if (strcmp(varID, cds_var->name)==0)
		{
		 	// Print value.
		 	io_sprintf(str, "%s = %d\r\n", cds_var->name, cds_var->value);
		 	mprint(str);

		 	return 0;
		}
		cds_var++;
	}
	if (flag_all) mprint("\r\n");

	// Generic variables.
	if (flag_all) mprint("### Generic Variables ###\r\n");
	generic_var_t *generic_var = (generic_var_t *) &(sys->generic_vars);
	int nGen_var = sizeof(generic_vars_t)/sizeof(generic_var_t);
	for(int i = 0; i < nGen_var; i++)
	{
		if (flag_all)
		{
			// Print value.
			io_sprintf(str, "%s = %f\r\n", generic_var->name, generic_var->value);
			mprint(str);
		}
		else if (strcmp(varID, generic_var->name)==0)
		{
			// Print value.
			io_sprintf(str, "%s = %f\r\n", generic_var->name, generic_var->value);
			mprint(str);

			return 0;
		}
		generic_var++;
	}
	if (flag_all) mprint("\r\n");

	//	Leds.
	if (flag_all) mprint("### Leds ###\r\n");
	led_status_t *leds_var = (led_status_t *) &(sys->leds);
	int nLeds = sizeof(led_group_status_t)/sizeof(led_status_t);
	for(int i = 0; i < nLeds; i++)
	{
		if (flag_all)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", leds_var->name, leds_var->status);
			mprint(str);
		}
		else if (strcmp(varID, leds_var->name)==0)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", leds_var->name, leds_var->status);
			mprint(str);

			return 0;
		}
		leds_var++;
	}
	if (flag_all) mprint("\r\n");

	// Smart buffer.
	if (flag_all) mprint("### Smart Buffer ###\r\n");
	smart_buffer_status_t *smart_buffer_var = (smart_buffer_status_t *) &(sys->smart_buffer);
	int nSmartBuf = sizeof(smart_buffer_group_status_t)/sizeof(smart_buffer_status_t);
	for(int i = 0; i < nSmartBuf; i++)
	{
		if (flag_all)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", smart_buffer_var->name, smart_buffer_var->value);
			mprint(str);
		}
		else if (strcmp(varID, smart_buffer_var->name)==0)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", smart_buffer_var->name, smart_buffer_var->value);
			mprint(str);

			return 0;
		}
		smart_buffer_var++;
	}
	if (flag_all) mprint("\r\n");

	// Ethernet.
	if (flag_all) mprint("### Ethernet ###\r\n");
	eth_status_t *eth_var = (eth_status_t *) &(sys->eth);
	int nEth = sizeof(eth_t)/sizeof(eth_status_t);
	for(int i = 0; i < nEth; i++)
	{
		if (flag_all)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", eth_var->name, eth_var->status);
			mprint(str);
		}
		else if (strcmp(varID, eth_var->name)==0)
		{
			// Print value.
			io_sprintf(str, "%s = %d\r\n", eth_var->name, eth_var->status);
			mprint(str);

			return 0;
		}
		eth_var++;
	}
	if (flag_all) mprint("\r\n");

	// Get sequencer in RAM.
	if (strcmp(varID,"seq")==0)
	{
		mprint("### Sequencer in RAM ###\r\n");

		// Print sequencer program.
		for(int i = 0; i < sys->seq.sequencer.size; i++)
		{
			// FIXME: print 32 bits values in hex.
			io_sprintf(str, "@%d, %u\r\n", i, sys->seq.sequencer.program[i]);
			mprint(str);
		}

		return 0;
	}

	if (strcmp(varID,"b")==0)
	{
		mprint("\n\r\n\rriBer es de la B!!! ...esa mancha no se borra...\n\r\n\r");

		return 0;
	}

	// If reaches this point for individual variables, it means the variable was
	// not found.
	if (flag_all)
	{
		return 0;
	}
	else
	{
		io_sprintf(errStr, "### Variable %s not found\r\n", varID);
		return -1;
	}
}

int excecute_get_telemetry(system_state_t *sys, const char *varID) {

	int ret = 0;

	// help
	if (strcmp(varID,"help") == 0)
	{
		mprint("### Telemetry variables ###\r\n");
		char str[50];
		int i;
		telemetry_source_t *src = (telemetry_source_t*) &(sys->telemetry);
		int n = sizeof(telemetry_group_t)/sizeof(telemetry_source_t);
		for (i=0; i<n; i++)
		{
			io_sprintf(str, "%s : %s\r\n", src->name, src->description);
			mprint(str);
			src++;
		}
		ret = 0;
	}

	// all
	else if (strcmp(varID,"all") == 0)
	{
		mprint("### Telemetry values ###\r\n");
		char str[50];
		int i;
		float value;

		telemetry_source_t *src = (telemetry_source_t*) &(sys->telemetry);
		int n = sizeof(telemetry_group_t)/sizeof(telemetry_source_t);
		for (i=0; i<n; i++)
		{
			ret = telemetry_read(src, &value);
			if (ret == 0)
			{
				io_sprintf(str, "%s = %f\r\n", src->name, value);
				mprint(str);
				src++;
			}
		}
		ret = 0;
	}

	// single value
	else
	{
		int i;
		float value;
		char str[50];

		telemetry_source_t *src = (telemetry_source_t*) &(sys->telemetry);
		int n = sizeof(telemetry_group_t)/sizeof(telemetry_source_t);
		for (i=0; i<n; i++)
		{
			if (strcmp(src->name,varID) == 0)
			{
				ret = telemetry_read(src, &value);
				if (ret == 0)
				{
					io_sprintf(str, "%s = %f\r\n", varID, value);
					mprint(str);
				}
			}
			src++;
		}
	}

	return ret;
}

int execute_exec(system_state_t *sys, const char *varID)
{
	// Help.
	if (strcmp(varID,"help") == 0)
	{
		int i;
		char str[100];

		exec_func_t *src = (exec_func_t*) &(sys->exec);
		int n = sizeof(exec_t)/sizeof(exec_func_t);

		if (n == 0)
		{
			mprint("### Function catalog empty.\r\n");
		}
		else
		{
			mprint("### Function catalog ###\r\n");
			for (i=0; i<n; i++)
			{
				io_sprintf(str, "%s : %s\r\n", src->name, src->description);
				mprint(str);
				src++;
			}
			return 0;
		}
	}

	// Function execution.
	else
	{
		int i;
		exec_func_t *src = (exec_func_t*) &(sys->exec);
		int n = sizeof(exec_t)/sizeof(exec_func_t);
		for (i=0; i<n; i++)
		{
			int comp = strcmp(src->name,varID);
			if (comp == 0)
			{
				return src->func(sys);
			}
			src++;
		}
	}
	return 0;
}

int excecute_set(system_state_t *sys, char *varID, char *varVal, char *errStr) {
	// Convert string to float.
	float value = atof(varVal);

	// Return value.
	int status = -1;

	// Clock voltages.
	clk_status_t *clk = (clk_status_t *) &(sys->clks);
	int nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		if (strcmp(varID, clk->name)==0)
		{
		 	// Set value.
		 	status = ad5371_set_voltage(clk, value);

		 	if (status != 0)
		 	{
		 		io_sprintf(errStr, "%s out of range\r\n", clk->name);
		 		return -1;
		 	}

		 	return status;
		}
		clk++;
	}
	
	// Clock switches.
	clk_sw_status_t *clk_sw = (clk_sw_status_t *) &(sys->clk_sw.sw_group);
	int nClocks_sw = sizeof(clk_sw_group_status_t)/sizeof(clk_sw_status_t);
	for(int i = 0; i < nClocks_sw; i++)
	{
		if (strcmp(varID, clk_sw->name)==0)
		{
		 	// Set value.
		 	return ad5371_change_switch_status(clk_sw, &(sys->clk_sw.state), (const uint8_t) value);
		}
		clk_sw++;
	}

	// Bias voltage.
	bias_status_t *bias = (bias_status_t *) &(sys->biases);
	int nBiases = sizeof(bias_group_status_t)/sizeof(bias_status_t);
	for(int i = 0; i < nBiases; i++)
	{
		if (strcmp(varID, bias->name)==0)
		{
		 	// Set value.
		 	status = ad5293_set_voltage(bias, value);

		 	if ( status != 0)
		 	{
		 		io_sprintf(errStr, "%s out of range\r\n", bias->name);
		 		return -1;
		 	}
		 	return status;
		}
		bias++;
	}

	// Bias switches.
	bias_sw_status_t *bias_sw = (bias_sw_status_t *) &(sys->bias_sw.sw_group);
	int nbias_sw = sizeof(bias_sw_group_status_t)/sizeof(bias_sw_status_t);
	for(int i = 0; i < nbias_sw; i++)
	{
		if (strcmp(varID,(bias_sw+i)->name)==0)
		{
		 	return max14802_volt_sw_state_set(bias_sw+i, &(sys->bias_sw.state), (const uint8_t) value);
		}
	}

	// Packer switches.
	packer_sw_status_t *packer_sw = (packer_sw_status_t *) &(sys->packer_sw);
	int nPacker_sw = sizeof(packer_sw_group_status_t)/sizeof(packer_sw_status_t);
	for(int i = 0; i < nPacker_sw; i++)
	{
		if (strcmp(varID,packer_sw->name)==0)
		{
		 	status = packer_change_sw_status(packer_sw, (const uint8_t) value);

		 	if ( status != 0)
		 	{
		 		io_sprintf(errStr, "%s out of range\r\n", packer_sw->name);
		 		return -1;
		 	}
		 	return status;
		}
		packer_sw++;
	}

	// ADC switches.
	adc_sw_status_t *adc_sw = (adc_sw_status_t *) &(sys->gpio_adc.sw_group);
	int nAdc_sw = sizeof(adc_sw_group_status_t)/sizeof(adc_sw_status_t);
	for(int i = 0; i < nAdc_sw; i++)
	{
		if (strcmp(varID,adc_sw->name)==0)
		{
		 	status = adc_change_sw_status(adc_sw, &(sys->gpio_adc.state), (const uint8_t) value);

		 	if (status != 0)
		 	{
		 		io_sprintf(errStr, "%s out of range\r\n", adc_sw->name);
		 		return -1;
		 	}
		 	return status;
		}
		adc_sw++;
	}

	// Sequencer switches.
	seq_sw_status_t *seq_sw = (seq_sw_status_t *) &(sys->seq.sw_group);
	int nSeq_sw = sizeof(seq_sw_group_status_t)/sizeof(seq_sw_status_t);
	for(int i = 0; i < nSeq_sw; i++)
	{
		if (strcmp(varID, seq_sw->name)==0)
		{
		 	status = seq_change_sw_status(seq_sw, (const uint8_t) value);

		 	if (status != 0)
		 	{
		 		io_sprintf(errStr, "%s out of range\r\n", seq_sw->name);
		 		return -1;
		 	}
		 	return status;
		}
		seq_sw++;
	}

	// CDS variables.
	cds_var_status_t *cds_var = (cds_var_status_t *) &(sys->cds);
	int nCds_var = sizeof(cds_var_group_status_t)/sizeof(cds_var_status_t);
	for(int i = 0; i < nCds_var; i++)
	{
		if (strcmp(varID, cds_var->name)==0)
		{
		 	status = cds_core_change_var_value(cds_var, (const uint16_t) value);

		 	if (status != 0)
		 	{
		 		io_sprintf(errStr, "%s out of range.\r\n", cds_var->name);
		 		return -1;
		 	}
		 	return status;
		}
		cds_var++;
	}

	//	Generic variables.
	generic_var_t *generic_var = (generic_var_t *) &(sys->generic_vars);
	int nGen_var = sizeof(generic_vars_t)/sizeof(generic_var_t);
	for(int i = 0; i < nGen_var; i++)
	{
		if (strcmp(varID, generic_var->name)==0)
		{
			status = generic_vars_change_value(generic_var, (uint8_t) value);

			if (status != 0)
			{
				io_sprintf(errStr, "%s out of range.\r\n", generic_var->name);
				return -1;
			}

			return status;
		}
		generic_var++;
	}

	//	Leds.
	led_status_t *leds_var = (led_status_t *) &(sys->leds);
	int nLeds = sizeof(led_group_status_t)/sizeof(led_status_t);
	for(int i = 0; i < nLeds; i++)
	{
		if (strcmp(varID,leds_var->name)==0)
		{
			return gpio_leds_change_state(leds_var, &(sys->leds.state), (uint8_t) value);
		}
		leds_var++;
	}

	// Smart buffer.
	smart_buffer_status_t *smart_buffer_var = (smart_buffer_status_t *) &(sys->smart_buffer);
	int nSmartBuf = sizeof(smart_buffer_group_status_t)/sizeof(smart_buffer_status_t);
	for(int i = 0; i < nSmartBuf; i++)
	{
		if (strcmp(varID, smart_buffer_var->name)==0)
		{
			status = smart_buffer_change_status(smart_buffer_var, (uint16_t) value);

			if (status != 0)
			{
				io_sprintf(errStr, "%s out of range.\r\n", smart_buffer_var->name);
				return -1;
			}
			return status;
		}
		smart_buffer_var++;
	}



	// Ethernet.
	eth_status_t *eth_var = (eth_status_t *) &(sys->eth);
	int nEth = sizeof(eth_t)/sizeof(eth_status_t);
	for(int i = 0; i < nEth; i++)
	{
		if (strcmp(varID, eth_var->name)==0)
		{
			status = gpio_eth_change_state(eth_var, (uint8_t) value);

			if (status != 0)
			{
				io_sprintf(errStr, "%s out of range.\r\n", eth_var->name);
				return -1;
			}
			return status;
		}
		eth_var++;
	}


	if (strcmp(varID,"seq")==0)
	{
		if(strcmp(varVal,"clear")==0)
		{
			// Clear sequencer program.
			return sequencer_clear_program(&(sys->seq));
		}
		if(strcmp(varVal,"load")==0)
		{
			// Load sequencer program.
			status = sequencer_load_program(&(sys->seq.sequencer));

			if (status != 0)
			{
				io_sprintf(errStr, "%s could not be loaded\r\n", sys->seq.sequencer.name);
				return -1;
			}

			return status;
		}
	}

	// If reaches this point, the variable was not found.
	io_sprintf(errStr, "### Variable %s not found.\r\n", varID);

	return -1;
}

int execute_setseq(system_state_t *sys, const char *posChar, const char *valChar) {

	// Instruction.
	long long valLong = atoll(valChar);
	uint32_t valUint32 = (uint32_t) valLong;

	// Address.
	int  posInt = atoi(posChar);

	// Write instruction @address.
	return sequencer_change_program(&(sys->seq), (unsigned int) posInt, valUint32);
}
