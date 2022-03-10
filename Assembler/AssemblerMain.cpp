#include <cstdio>
#include <stdexcept>

#include "Assembler.h"
#include "Utils.hpp"
#include "Stack.h"

//------------------------------

void strpbrk_test (const char* source, const char* delimiters);

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

		assembler.test ();
	}

	catch (assembler_error err)
	{
		printf ("Exception: %s\n", err.what ());
	}
}

//------------------------------

void strpbrk_test (const char* source, const char* delimiters)
{
	printf ("Strpbrk ('%s', '%s') test: ", source, delimiters);
	const char* got = Assembler_strpbrk (source, delimiters);
	const char* exp =           strpbrk (source, delimiters);

	if (exp == got || strcmp (exp, got) == 0) printf ("OK!");
	else printf ("FAILED: expected '%s' (%p), got '%s' (%p)", exp, exp, got, got);

	putchar ('\n');
}

//------------------------------