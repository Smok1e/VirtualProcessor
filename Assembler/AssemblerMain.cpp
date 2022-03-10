#include <cstdio>
#include <stdexcept>

#include "Assembler.h"
#include "Utils.hpp"
#include "Stack.h"

//------------------------------

int main ()
{
	try
	{
		const char* source =
			"push 123        \n"
			"push 10         \n"
			"add     >Comment\n"
			"push 5          \n"
			"mul             \n"
			"out             \n";

		Assembler assembler;
		assembler.setSourceCode (source);
		assembler.assemble      ();
		assembler.test          ();
	}

	catch (assembler_error err)
	{
		printf ("Exception: %s\n", err.what ());
	}
}

//------------------------------