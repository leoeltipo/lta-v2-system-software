/*
 * packer.h
 * @file packer.h
 * @brief Sequencer driver
 * @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */

#ifndef EXCECUTE_H_
#define EXCECUTE_H_

#include "defines.h"

int excecute_interpret(system_state_t *sys, char *userWord, char *errStr);
void excecute_help(system_state_t *sys);
int excecute_get(system_state_t *sys, const char *varID, char *errStr);
int excecute_get_telemetry(system_state_t *sys, const char *varID);
int execute_exec(system_state_t *sys, const char *varID);
int excecute_set(system_state_t *sys, char *varID, char *varVal, char *errStr);
int execute_setseq(system_state_t *sys, const char *varVal1, const char *varVa21);

#endif // EXCECUTE_H_
