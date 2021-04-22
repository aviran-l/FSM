/******************************************************************
*									      						  *
*			      	        Calculator	    					  *
*					          Aviran				 			  *
*			  Implementation of FSM based basic calculator 		  *
*																  *
*******************************************************************/
#include <stdlib.h> /* malloc, free */

#include "calculator.h"
#include "stack.h"

#define MAX_NUMBERS_IN_STRING (128)
#define MAX_OPERATORS_IN_STRING (127)
#define SIZE_OF_OPERATOR (sizeof(char))
#define SIZE_OF_NUMBER (sizeof(double))
#define VALID_OPERATORS (4) /* { + , - , * , / }         */
#define FSM_LUT_WIDTH (3)   /* { digits , + - * / , \0 } */
#define NUM_OF_STATES (3)   /* { WFO , WFN , ERROR }     */

typedef double (*math_action_t)(double first_val, double second_val, 
																int *status);


typedef struct stacks
{
	stack_t *numbers;
	stack_t *operators;
}calc_stacks_t;


typedef int (*calculator_fsm_t)(const char *input, calc_t *calculator);


typedef struct lut_fsm_element
{
	calculator_fsm_t handler;
	int next_state;
}lut_fsm_element_t;


typedef struct operators_lut
{
	math_action_t action;
	int priority;
}operators_lut_t;


struct calculator
{
	calc_stacks_t stacks;
	lut_fsm_element_t **lut_fsm_table;
	operators_lut_t *operators_lut;
};


enum machine_states
{
	WAIT_FOR_OPERATOR = 0,
	WAIT_FOR_NUMBER,
	ERROR
};


enum events
{
	DIGITS = 0,
	OPERATORS,
	NUL_CHAR,
	MAX_EVENTS
};

enum priorities
{
	ADD_SUB_PRIORITY = 0,
	MUL_DIV_PRIORITY
};


static double Add(double first_val, double second_val, int *status)
{
	(void)status;
	
	return first_val + second_val;
}


static double Substract(double first_val, double second_val, int *status)
{
	(void)status;
	
	return first_val - second_val;
}


static double Multiply(double first_val, double second_val, int *status)
{
	(void)status;
	
	return first_val * second_val;
}


static double Divide(double first_val, double second_val, int *status)
{
	if (0.0 == second_val)
	{
		*status = MATH_ERROR;

		return 0.0;
	}
	
	return first_val / second_val;
}


static void InitOperatorsLUT(operators_lut_t *lut)
{
	lut->action = &Add;
	lut->priority = ADD_SUB_PRIORITY;
	  
	++lut;
	
	lut->action = &Substract;
	lut->priority = ADD_SUB_PRIORITY;

	++lut;
	
	lut->action = &Multiply;
	lut->priority = MUL_DIV_PRIORITY;

	++lut;
	
	lut->action = &Divide;
	lut->priority = MUL_DIV_PRIORITY;
}


static int GetOperatorLUTIndex(const char *input, calc_t *calculator)
{
	(void)calculator;

	switch (*input)
	{
		case '+':
			return 0;
			break;
		case '-':
			return 1;
			break;
		case '*':
			return 2;
			break;
		case '/':
			return 3;
			break;
	}

	return -1;
}


static int ResolveExpressions(calc_t *calculator)
{
	double first_val = 0;
	double second_val = 0;
	double action_result_val = 0;
	double *action_result = &action_result_val;
	int status_val = CALC_SUCCESS;
	int *status = &status_val;
	char operator = 0; 
		
	/* resolving is done as long as there is more than one number */ 
	while (1 < StackSize(calculator->stacks.numbers))
	{	
		/* LIFO means the first to be peeked is the second operand */
		second_val = *(double *)StackPeek(calculator->stacks.numbers);
		StackPop(calculator->stacks.numbers);

		first_val = *(double *)StackPeek(calculator->stacks.numbers);
		StackPop(calculator->stacks.numbers);

		operator = *(char *)StackPeek(calculator->stacks.operators);
		StackPop(calculator->stacks.operators);
		
		*action_result = calculator->operators_lut[GetOperatorLUTIndex
		(&operator, calculator)].action(first_val, second_val, status);

		if (CALC_SUCCESS != *status)
		{
			return *status;
		}
		
		/* pushing the result of the last expression for further resolving */
		if (StackPush(calculator->stacks.numbers, (void *)action_result))
		{
			return MEMORY_ERROR;		
		}
	}

	return CALC_SUCCESS;
}


static int HandleOperators(const char *input, calc_t *calculator)
{
	/* if stack is empty or priority is lower than the current, then push */
	if (!StackSize(calculator->stacks.operators) || calculator->operators_lut
		[GetOperatorLUTIndex((char *)StackPeek(calculator->stacks.operators), 
							 calculator)].priority < 
		
		calculator->operators_lut[GetOperatorLUTIndex(input, 
													  calculator)].priority)
		
	{
		if (StackPush(calculator->stacks.operators, (void *)input))
		{
			return MEMORY_ERROR;
		}
	}
	/* otherwise, resolve whatever there is currently on the stacks */
	else
	{
		if(ResolveExpressions(calculator))
		{
			return MEMORY_ERROR;
		}
		
		if (StackPush(calculator->stacks.operators, (void *)input))
		{
			return MEMORY_ERROR;
		}
	}
	
	return CALC_SUCCESS;
}


static int HandleNumbers(const char *input, calc_t *calculator)
{
	double extracted_digit = strtod(input, NULL);
	double *extracted_digit_ptr = &extracted_digit;

	if (StackPush(calculator->stacks.numbers, (void *)extracted_digit_ptr))
	{
		return MEMORY_ERROR;
	}
	
	return CALC_SUCCESS;
}


static void InitHandlersLUT(lut_fsm_element_t **lut_fsm_table)
{ 
	/* WAIT_FOR_NUMBER since first char must belong to a number */
	lut_fsm_table[WAIT_FOR_NUMBER][DIGITS].handler = &HandleNumbers;
	lut_fsm_table[WAIT_FOR_NUMBER][DIGITS].next_state = WAIT_FOR_NUMBER;
	 
	lut_fsm_table[WAIT_FOR_OPERATOR][OPERATORS].handler = &HandleOperators;
	lut_fsm_table[WAIT_FOR_OPERATOR][OPERATORS].next_state = WAIT_FOR_NUMBER;
}


static void CreateCalcStacks(calc_stacks_t *stacks, 
										size_t numbers_stack_capacity,
										size_t size_of_type_number, 
					   				    size_t operators_stack_capacity,
					  		            size_t size_of_type_operator)
{
	stacks->numbers = StackCreate(numbers_stack_capacity, size_of_type_number);
		
	if (NULL == stacks->numbers)
	{
		return;
	} 
	
	stacks->operators = StackCreate(operators_stack_capacity, 
								    size_of_type_operator);
	
	if (NULL == stacks->operators)
	{
		free(stacks->numbers);
		stacks->numbers = NULL;

		return;
	}
}


static operators_lut_t *CreateOperatorsLUT(void)
{
	operators_lut_t *operators_lut = 
	(operators_lut_t *)malloc(sizeof(operators_lut_t) * VALID_OPERATORS);
		
	if (NULL == operators_lut)
	{
		return NULL;
	}
	
	return operators_lut;
}


static lut_fsm_element_t **CreateHandlersLUT(void)
{
	lut_fsm_element_t **lut_fsm_element = 
	(lut_fsm_element_t **)malloc(sizeof(lut_fsm_element_t*) * FSM_LUT_WIDTH);
	size_t index = 0;

	if (NULL == lut_fsm_element)
	{
		return NULL;
	}

	/* having each pointer in array of pointers point to an allocated array */
	for (; index < FSM_LUT_WIDTH; ++index)
	{
		lut_fsm_element[index] = 
		(lut_fsm_element_t *)malloc(sizeof(lut_fsm_element_t) * NUM_OF_STATES);

		if (NULL == lut_fsm_element[index])
		{
			break;
		}
	}

	/* freeing all the arrays that were allocated successfuly thus far */
	if (FSM_LUT_WIDTH - 1 > index)
	{
		for (; (int) index >= 0 ; --index)
		{
			free(lut_fsm_element[index]);
			lut_fsm_element[index] = NULL;
		}

		free(lut_fsm_element);
		lut_fsm_element = NULL;

		return NULL; 
	}

	return lut_fsm_element;
}


calc_t *CalcInit(void)
{
	calc_t *calculator = (calc_t *)malloc(sizeof(calc_t));

	if (NULL == calculator)
	{
		return NULL;
	}
	
	CreateCalcStacks(&calculator->stacks, MAX_NUMBERS_IN_STRING, 
										  SIZE_OF_NUMBER, 
										  MAX_OPERATORS_IN_STRING, 
										  SIZE_OF_OPERATOR);
					  				         
	if (NULL == calculator->stacks.numbers)
	{
		free(calculator);
		calculator = NULL;
			
		return NULL;
	}

	calculator->operators_lut = CreateOperatorsLUT();
		
	if (NULL == calculator->operators_lut)
	{
		StackDestroy(calculator->stacks.operators);
		StackDestroy(calculator->stacks.numbers);
		free(calculator);
		calculator = NULL;
			
		return NULL;
	}
		
	InitOperatorsLUT(calculator->operators_lut);
		
	calculator->lut_fsm_table = CreateHandlersLUT();
							 
	if (NULL == calculator->lut_fsm_table)
	{
		free(calculator->operators_lut);
		calculator->operators_lut = NULL;
		StackDestroy(calculator->stacks.operators);
		StackDestroy(calculator->stacks.numbers);
		free(calculator);
		calculator = NULL;
			
		return NULL;
	}
		
	InitHandlersLUT(calculator->lut_fsm_table);	  

	return calculator;
}


double Calculator(calc_t *calc, const char *input, int *status)
{
	double ret_val = 0;

	if (NULL == calc)
	{
		*status = MEMORY_ERROR;

		return 0.0;
	}

	while ('\0' != *input)
	{
		if ('0' <= *input && '9' >= *input)
		{
			/* if an operator is expected when a digit was handed over */
			if (calc->lut_fsm_table[WAIT_FOR_NUMBER][DIGITS].next_state == WAIT_FOR_OPERATOR)
			{
				*status = SYNTAX_ERROR;
					
				return 0.0;
			}

			*status = calc->lut_fsm_table[WAIT_FOR_NUMBER][DIGITS].handler(input, calc);

			calc->lut_fsm_table[WAIT_FOR_NUMBER][DIGITS].next_state = WAIT_FOR_OPERATOR;
			calc->lut_fsm_table[WAIT_FOR_OPERATOR][OPERATORS].next_state = WAIT_FOR_OPERATOR;
		}

		if ('+' == *input || '-' == *input || '*' == *input || '/' == *input)
		{
			/* if a number is expected when an operator was handed over */
			if (calc->lut_fsm_table[WAIT_FOR_OPERATOR][OPERATORS].next_state == WAIT_FOR_NUMBER)
			{
				*status = SYNTAX_ERROR;

				return 0.0;
			}

			*status = calc->lut_fsm_table[WAIT_FOR_OPERATOR][OPERATORS].handler(input, calc);
			
			calc->lut_fsm_table[WAIT_FOR_NUMBER][DIGITS].next_state = WAIT_FOR_NUMBER;
			calc->lut_fsm_table[WAIT_FOR_OPERATOR][OPERATORS].next_state = WAIT_FOR_NUMBER;
		}

		++input;
	}

	/* resolving all expressions still awaiting to be resolved  */
	*status = ResolveExpressions(calc);
	InitHandlersLUT(calc->lut_fsm_table);

	if (CALC_SUCCESS == *status)
	{
		ret_val = *(double *)StackPeek(calc->stacks.numbers);
		StackPop(calc->stacks.numbers);
		
		return ret_val;
	}

	return 0.0;
}


void CalcDestroy(calc_t *calc)
{
	size_t index = 0;

	free(calc->operators_lut);
	calc->operators_lut = NULL;

	for (; index < FSM_LUT_WIDTH; ++index)
	{
		free(calc->lut_fsm_table[index]);
		calc->lut_fsm_table[index] = NULL;
	}

	free(calc->lut_fsm_table);
	calc->lut_fsm_table = NULL;
	
	StackDestroy(calc->stacks.operators);
	StackDestroy(calc->stacks.numbers);
	free(calc);
	calc = NULL;
}
