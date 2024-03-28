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
	curr_glthread->right = new_glthread;
	new_glthread->left = curr_glthread;
	new_glthread->right = temp;
	temp->left = new_glthread;
}

void glthread_add_before(glthread_t *curr_glthread, glthread_t *new_glthread)
{
  if(!curr_glthread->left)
  {
    new_glthread->left = NULL;
    new_glthread->right = curr_glthread;
    curr_glthread->left = new_glthread;
    return;
  }

  glthread_t *temp = curr_glthread->left;
  new_glthread->right = curr_glthread;
  curr_glthread->left = new_glthread;
  temp->right = new_glthread;
  new_glthread->left = temp;
}

void remove_glthread(glthread_t *curr_glthread)
{
  if(!curr_glthread->left)
  {
    if(curr_glthread->right)
    {
      curr_glthread->right->left = NULL;
      curr_glthread->right = NULL;
      return;
    }
    return;
  }

  if(!curr_glthread->right)
  {
    curr_glthread->left->right = NULL;
    curr_glthread->left = NULL;
    return;
  }

  curr_glthread->left->right = curr_glthread->right;
  curr_glthread->right->left = curr_glthread->left;
  curr_glthread->right = NULL;
  curr_glthread->left = NULL;
}

void delete_glthread_list(glthread_t *base_glthread)
{
  glthread_t *glthreadptr = NULL;

  ITERATE_GLTHREAD_BEGIN(base_glthread, glthreadptr)
  {
    remove_glthread(glthreadptr);
  } 

  ITERATE_GLTHREAD_END(base_glthread, glthreadptr);
}

void glthread_add_last(glthread_t *base_glthread, glthread_t *new_glhtread)
{
  glthread_t *glthreadptr = NULL,
             *prevglthreadptr = NULL;
}

