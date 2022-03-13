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
			"test push \n";

		Assembler assembler;
		assembler.setListingStream (&listing);
		assembler.setSourceCode    (source);
		assembler.assemble         ();

		assembler.getProgram ().save ("test.bin");
		printf ("Program saved as test.bin\n\n");

		printf ("Listing:\n");
		printf (listing.str ().c_str ());
	}

	catch (assembler_error err)
	{
		printf ("Exception: %s\n", err.what ());
	}
}

//------------------------------