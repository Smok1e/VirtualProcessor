#define _STACK_FORCE_CHECK

#include "Stack.h"

//------------------------------

int main ()
{
	try
	{
		Stack <unsigned char> stack;

		stack.push (0xAA);
		stack.push (0xBB);
		stack.push (0xCC);
		stack.push (0xDD);
		stack.push (0xEE);

		stack.printDebugDump ();
		printf ("\n");
		stack.reverse ();
		stack.printDebugDump ();
	}

	catch (stack_check_error exc)
	{
		printf ("Stack error: %s\n", exc.what ());
	}
}

//------------------------------