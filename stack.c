/******************************************************************************
*									      									  *
*							           Stack		     					  *
*								      Aviran					 			  *
*					Implementation of stack data structure		     		  *
*																			  *
******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "stack.h"

struct stack
{
	size_t size_of_item;
	void *end_position;
	void *top_position;
	void *base_position;
};


stack_t *StackCreate(size_t capacity, size_t size_of_item)
{
	/* allocating the needed array of bytes according to user input */
	stack_t *allocation_point = (stack_t*) malloc(size_of_item * (capacity + 1)
                             					  + sizeof(stack_t));
	
	if (NULL == allocation_point)
	{
		return NULL;
	}
	
	/* initializing all fields of the managing unit to their proper values */
	allocation_point->size_of_item = size_of_item;
	allocation_point->base_position = allocation_point + 1;
	allocation_point->top_position = allocation_point + 1;
	allocation_point->end_position = allocation_point + 1;
	allocation_point->end_position = (char*) allocation_point->end_position + 
									  size_of_item * capacity;
	
	return allocation_point;	
}


void StackDestroy(stack_t *stack)
{
	free(stack);
	stack = NULL;
}


int StackPush(stack_t *stack, const void *value)
{
	/* in case no room is left in the stack */
	if (stack->top_position == stack->end_position)
	{
		return 1;
	}

	/* assigning the data and advancing pointer to the next empty place */
	memcpy(stack->top_position, value, stack->size_of_item);
	stack->top_position = (char*) stack->top_position + stack->size_of_item;

	return 0; 
}


void StackPop(stack_t *stack)
{	
	/* in case no items are available on the stack */
	if (stack->top_position == stack->base_position)
	{
		return;
	}

	/* assigning the pointer to a lower position means removing an item */
	stack->top_position = (char*) stack->top_position - stack->size_of_item;
}


void *StackPeek(const stack_t *stack)
{
	/* in case no items are available on the stack */
	if (stack->top_position == stack->base_position)
	{
		return NULL;
	}

	/* returning a pointer to the last added item */
	return ((char*) stack->top_position - stack->size_of_item);
}


size_t StackSize(const stack_t *stack)
{
	return (((char*) stack->top_position - (char*) stack->base_position) / 
			stack->size_of_item);
}
