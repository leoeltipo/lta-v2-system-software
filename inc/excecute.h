/*
 * packer.h
 * @file packer.h
 * @brief Sequencer driver
 * @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */

#ifndef EXCECUTE_H_
#define EXCECUTE_H_

#include "ad5293.h"
#include "ad5371.h"
//#include "ad7328.h"
#include "max14802.h"
#include "sequencer.h"
#include "packer.h"
#include "adc.h"
#include "defines.h"

#define USERCOMMANDLENGTH 100
#define USERWORDLENTHG 50
#define USERNUMBWORDS 5

#define NO_WORD 0
#define ONE_WORD 1
#define TWO_WORD 2
#define THREE_WORDS 3
#define FOUR_WORDS 4

typedef struct {
	char word[USERWORDLENTHG];
} user_words_t;

int excecute_interpret(system_state_t *sys, char *userWord, char *errStr);
void excecute_help(system_state_t *sys);
int excecute_get(system_state_t *sys, const char *varID, char *errStr);
int excecute_get_telemetry(system_state_t *sys, const char *varID);
int execute_exec(system_state_t *sys, const char *varID);
int excecute_set(system_state_t *sys, char *varID, char *varVal, char *errStr);
int execute_setseq(system_state_t *sys, const char *varVal1, const char *varVa21);

#endif // EXCECUTE_H_
