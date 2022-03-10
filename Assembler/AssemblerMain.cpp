#include <cstdio>
#include <stdexcept>

#include "Assembler.h"
#include "Utils.hpp"
#include "Stack.h"

//------------------------------

int main ()
{
	std::stringstream listing;

	try
	{
		const char* source =
			"push 123   \n"
			"push 10    \n"
			"add        \n"
			"push 5     \n"
			"mul        \n"
			"out        \n";

		Assembler assembler;
		assembler.setListingStream (&std::cout);
		assembler.setSourceCode    (source);
		assembler.assemble         ();

		//printf ("Listing:\n");
		//printf (listing.str ().c_str ());
	}

	catch (assembler_error err)
	{
		printf ("Exception: %s\n", err.what ());
	}
 }

//------------------------------