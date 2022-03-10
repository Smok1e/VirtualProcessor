#include <sstream>

#include "Processor.h"

//------------------------------

int main ()
{
	try
	{
		ProgramContainer program;
		program.load ("test.bin");

	}

	catch (processor_error err)
	{
		printf ("Processor error: %s\n", err.what ());
	}

	catch (std::exception exc)
	{
		printf ("Unexpected exception: %s\n", exc.what ());
	}

	catch (...)
	{
		printf ("Unknown error\n");
	}
}

//------------------------------