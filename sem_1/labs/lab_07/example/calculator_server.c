#include "calculator.h"

struct CALCULATOR *
calculator_proc_1_svc(struct CALCULATOR *argp, struct svc_req *rqstp)
{
	static struct CALCULATOR  result;

	/*
	 * insert server code here
	 */

	/* -<<< Add to test */
	switch(argp->op)
	{
		case ADD:
			result.result = argp->arg1 + argp->arg2;
			break;
		case SUB:
			result.result = argp->arg1 - argp->arg2;
			break;
		case MUL:
			result.result = argp->arg1 * argp->arg2;
			break;
		case DIV:
			result.result = argp->arg1 / argp->arg2;
			break;
		default:
			break;
	}
	/* -<<< Add to test */

	return &result;
}
