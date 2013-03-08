/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2013                              *
 *  Authors: Ulrich Kremer                   *
 *           Hans Christian Woithe           *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

int main()
{
	Instruction *head;
	Instruction *instr1, *instr2, *instr3;
	int opt_flag, opt_calc;

	head = ReadInstructionList(stdin);
	if (!head) {
		ERROR("No instructions\n");
		exit(EXIT_FAILURE);
	}

	/* YOUR CODE GOES HERE */

	PrintInstructionList(stdout, head);
	DestroyInstructionList(head);
	return EXIT_SUCCESS;
}
