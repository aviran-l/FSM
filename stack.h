/******************************************************************************
*									      									  *
*							        Stack API		     					  *
*								      Aviran					 			  *
*					Implementation of stack data structure		     		  *
*																			  *
******************************************************************************/

#ifndef __STACK_H__
#define __STACK_H__

#include <stddef.h>  /* size_t */

/* The stack can accomodate all valid data types. It is the user's 
   responsibility to access the stack items according to their data type */

typedef struct stack stack_t;

/* Function name:	StackCreate
 * Description:		The function creates a stack of a given capacity and 
 					returns a stack_t pointer. If stack creation has failed, the 
 					function returns NULL.
 * Arguments:		capacity - maximal number of items the stack can store.
 					size_of_type - the size of one stack item (in bytes).
 * Return value:	stack_t pointer, or NULL pointer if the request fails.
 * NOTE:			it is the user's responsibility to use the StackDestroy 
 					function after the stack is no longer needed.
 */       		 		   		
stack_t *StackCreate(size_t capacity, size_t size_of_type);


/* Function name:	StackDestroy
 * Description:		this function releases the stack resources.
 * Arguments:		stack - a stack_t pointer.
 * Return value:	void.
 */       		 		   		
void StackDestroy(stack_t *stack);


/* Function name:	StackPush
 * Description:		this function copies the value into the stack top, 
 					and returns 0 for success. In case of overflow, the item is 
 					not added and 1 is returned.
 * Arguments:		stack - a stack_t pointer.
 					item_ptr - pointer to the item that is to be added to stack 
 							   top.
 * Return value:	returns 0 if the request was completed, otherwise returns 1.
 */       		 		   		
int StackPush(stack_t *stack, const void *value);


/* Function name:	StackPop
 * Description:		this function removes an item from stack top.
 * Arguments:		stack - a stack_t pointer.
 * Return value:	void.
 */       		 		   		
void StackPop(stack_t *stack);


/* Function name:	StackPeek
 * Description:		this function returns a pointer to the item at the top of
 					the stack. 
 * Arguments:		stack - a stack_t pointer.
 * Return value:	A pointer to the item at the top, which is suitable to the 
                    stack data item type. In case of empty stack returns NULL.
 */       		 		   		
void *StackPeek(const stack_t *stack);


/* Function name:	StackSize
 * Description:		this function returns the current number of items in stack.
 * Arguments:		stack - a stack_t pointer.
 * Return value:	current number of items in the stack.
 */       		 		   		
size_t StackSize(const stack_t *stack);

#endif /* __STACK_H__ */
