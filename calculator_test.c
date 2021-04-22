#include <stdio.h>

#include "calculator.h"
#include "stack.h"


int main(int argc, char* argv[])
{
	calc_t *my_calc = NULL;
	double result = 0;
	const char *math_expression = "4+5*2-6/3+7*1-8+9-0";
	int status_val = 0;
	int *status_result = &status_val;
	
	(void)argc;
	(void)argv;

	my_calc = CalcInit();

	if (NULL == my_calc)
	{
		return 1;
	}

	result = Calculator(my_calc, math_expression, status_result);
	printf("%f\n", result);
	CalcDestroy(my_calc);

	return 0;
}
