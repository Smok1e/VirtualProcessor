#include "Assembler.h"
#include "Processor.h"

//------------------------------

int main ()
{
	Assembler assembler;

	try
	{
		std::stringstream listing;
		assembler.setListingStream (&listing);
		assembler.loadSourceCode ("../Assembler/source.mcasm");
		assembler.compile ();

		printf ("Listing:\n");
		printf (listing.str ().c_str ());
		printf ("\n\n\n");
	}

	catch (assembler_error err)
	{
		printf ("Assembler error: %s\n", err.what ());
		return 0;
	}

	Processor processor;

	try
	{
		processor.defaultIO ();
		processor.setProgram (assembler.getProgram ());
		processor.run ();
	}

	catch (processor_error err)
	{
		// Вывод ошибок направлен в stderr
		return 0;
	}
}

//------------------------------