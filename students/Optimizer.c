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
    Instruction *curr, *prev, *next;

    head = ReadInstructionList(stdin);
    if (!head) {
        ERROR("No instructions\n");
        exit(EXIT_FAILURE);
    }

    curr = head;
    prev = head->prev;
    next = head->next;

    while(curr && next){
        if(prev && prev->opcode == LOADI &&
                curr && curr->opcode == LOADI){
            switch(next->opcode){
                /*add*/
                case ADD:
                    curr->field1 = next->field1;
                    curr->field2 = prev->field2 + curr->field2;

                    /*sub*/
                case SUB:
                    curr->field1 = next->field1;
                    if(next->field2 > next->field3){
                        curr->field2 = curr->field2 - prev->field2;
                    }
                    else{
                        curr->field2 = curr->field2 - prev->field2; 
                    }

                    /*mul*/
                case MUL:
                    curr->field1 = next->field1;
                    curr->field2 = curr->field2 * prev->field2;

                    curr->next = next->next;
                    curr->next->prev = curr;
                    curr->prev = prev->prev;
                    curr->prev->next = curr;
                    free(prev);
                    free(next);

                default:
                    break;
            }
        }
        curr = curr->next;
        prev = curr->prev;
        next = curr->next;


    }
    PrintInstructionList(stdout, head);
    DestroyInstructionList(head);
    return EXIT_SUCCESS;
}
