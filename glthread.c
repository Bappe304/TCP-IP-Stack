/* Description: This file has glued linked list related API calls.*/


#include "glthread.h"
#include <stdlib.h>

void init_glthread(glthread_t *glthread)
{
	glthread->left = NULL;
	glthread->right = NULL;

}

void glthread_add_next(glthread_t *curr_glthread, glthread_t *new_glthread)
{
	if(!curr_glthread->right)
	{
		curr_glthread->right = new_glthread;
		new_glthread->left = curr_glthread;
		return;
	}

	glthread_t *temp = curr_glthread->right;
}
