/*
 * exec.h
 *
 *  Created on: Aug 1, 2018
 *      Author: lstefana
 */

#ifndef SRC_EXEC_H_
#define SRC_EXEC_H_

typedef int (*fptr_t)(void *arg);

typedef struct {
	char name[30];
	char description[100];
	fptr_t func;
} exec_func_t;

typedef struct {
	exec_func_t ccd_erase;
	exec_func_t cdd_epurge;
	exec_func_t vsub_down;
} exec_t;

// Initialization.
int exec_init(exec_t *exec);

#endif /* SRC_EXEC_H_ */
