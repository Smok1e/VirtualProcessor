#include <sstream>

#include "Processor.h"

//------------------------------

int main ()
{
	try
	{
		ProgramContainer program;
		program.load ("../Assembler/program.bin");

		Processor processor;
		processor.defaultIO ();
		processor.setProgram (program);

		double retval = processor.run ();
		//printf ("\nReturn value: %lf\n", retval);	
	}

	catch (processor_error err)
	{
		// printf ("Processor error: %s\n", err.what ());
		// вывод ошибок направлен в stderr
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