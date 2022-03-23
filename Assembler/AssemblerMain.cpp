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
		Assembler assembler;
		assembler.setListingStream (&listing);
		assembler.loadSourceCode   ("source.mcasm");

		//assembler.dumpTokens ();

		assembler.assemble         ();

		assembler.getProgram ().save ("program.bin");
		printf ("Program saved as test.bin\n\n");

		printf ("Listing:\n");
		printf (listing.str ().c_str ());
	}

	catch (assembler_error err)
	{
		printf ("Assembler error: %s\n", err.what ());
	}

	catch (std::exception exc)
	{
		printf ("Unexpected exception thrown: %s\n", exc.what ());
	}
}

//------------------------------