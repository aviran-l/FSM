/******************************************************************
*									      						  *
*			      	  Calculator API	    					  *
*					     	Aviran						 		  *
*			  Implementation of FSM based basic calculator		  *
*																  *
*******************************************************************/

#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__

typedef struct calculator calc_t;

enum status
{
	CALC_SUCCESS = 0,
	MEMORY_ERROR = 1,
	MATH_ERROR,
	SYNTAX_ERROR
};


/* Function name:	CalcInit
 * Description:		This function creates a calculator.
 * Arguments:		none.
 * Return value:	calc_t pointer or NULL pointer if the request fails.
 * NOTE:			It is the user's responsibility to use the CalcDestroy
 					function after the calculator is no longer needed.
 */ 
calc_t *CalcInit(void);


/* Function name:	Calculator
 * Description:		This function calculates the mathematical expression in 
 					'input'.
 * Arguments:		calc - calc_t pointer.
 					input - a string containing a mathematical expression.
 					status - See enum status for details.
 * Return value:	Result of mathematical expression. In case of error, 
 					returned value will not be the final result of mathematical 
 					expression.
 * NOTE:			Max string length - 256 characters.
 */ 
double Calculator(calc_t *calc, const char *input, int *status);


/* Function name:	CalcDestory
 * Description:		This function destroys the calculator.
 * Arguments:		calc - calc_t pointer.
 * Return value:	none.
 */ 
void CalcDestroy(calc_t *calc);


#endif /* __CALCULATOR_H__ */
