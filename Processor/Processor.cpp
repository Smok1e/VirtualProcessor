#include <string>

#include "Processor.h"
#include "Utils.hpp"

//------------------------------

void RunProgram (const program_t& program, std::ostream& outstream /*= std::cout*/, std::istream& instream /*= std::cin*/, std::ostream& errstream /*= std::cerr*/)
{
	#pragma region
	#define ERROR_(format, ...)                                      \
	{														         \
		std::string msg = Format (format, __VA_ARGS__);	             \
		outstream << Format ("Processor error: %s\n", msg.c_str ()); \
													                 \
		stack.clear ();									             \
		throw exception_message (msg);						         \
	}

	#define EXECUTION_ERROR_(format, ...)								                                  \
	{														                                              \
		std::string msg = Format (format, __VA_ARGS__);		                                              \
		errstream << Format ("\nRuntime error at instruction 0x%08X: %s\n", current_index, msg.c_str ()); \
		errstream << "Aborting program\n\n";						                                      \
		stack.printDebugDump (outstream);					                                              \
		return;												                                              \
	}

	#define NEXT_VALUE_(var)                    \
	if (current_index >= program_size)	        \
		ERROR_ ("Invalid argument");            \
	stack_value_t var = program[current_index]; \
	current_index++;

	#define POP_SAFE_(var)                                       \
	if (stack.empty ())								             \
		EXECUTION_ERROR_ ("Value required, but stack is empty"); \
	stack_value_t var = stack.pop ();

	#define POP_NUMBER_(var)								   \
	double var = 0;											   \
	{														   \
		POP_SAFE_ (value);									   \
		var = static_cast <double> (value) / NUMBERS_MODIFIER; \
	}

	#define PUSH_NUMBER_(number) stack.push (static_cast <stack_value_t> (floor (number * NUMBERS_MODIFIER)))

	#define BINARY_OPERATOR_(operator_, ...) \
	{							             \
		POP_NUMBER_ (lft);                   \
		POP_NUMBER_ (rgt);				     \
		__VA_ARGS__                          \
		double res = lft operator_ rgt;      \
		PUSH_NUMBER_ (res);                  \
	}

	#define OPERATOR_CASE_(code, oper, ...) case code: { BINARY_OPERATOR_ (oper, __VA_ARGS__); break; };
	#pragma endregion

	size_t program_size  = program.size ();
	size_t current_index = 0;

	ProgramStack stack;

	NEXT_VALUE_ (version);
	if (version != ASSEMBLER_VERSION)
		ERROR_ ("Wrong assembler version (%d), %d is required", version, ASSEMBLER_VERSION);

	bool running = true;
									  
	size_t size = program.size ();
	while (running && current_index < program_size)
	{
		NEXT_VALUE_ (current_value);
		ByteCode cmd = static_cast <ByteCode> (current_value);

		switch (cmd)
		{
			case ByteCode::push:
			{
				NEXT_VALUE_ (value);
				stack.push (value);

				break;
			}

			case ByteCode::pop:
			{
				if (stack.empty ())
					EXECUTION_ERROR_ ("Pop called with empty stack");

				break;
			}

			OPERATOR_CASE_ (ByteCode::add, +);
			OPERATOR_CASE_ (ByteCode::sub, -);
			OPERATOR_CASE_ (ByteCode::mul, *);
			OPERATOR_CASE_ (ByteCode::div, /, { if (rgt == 0) EXECUTION_ERROR_ ("Zero division") });

			case ByteCode::in:
			{
				outstream << "Enter value: ";
				
				std::string input;
				instream >> input;
				
				if (!IsNumeric (input))
					EXECUTION_ERROR_ ("Invalid number input: '%s' is not numeric", input.c_str ());

				double number = 0;
				try 
				{ 
					number = std::stod (input); 
				}

				catch (std::out_of_range)
				{
					EXECUTION_ERROR_ ("Input number out of range");
				}

				PUSH_NUMBER_ (number);

				break;
			}

			case ByteCode::out:
			{
				if (stack.empty ())
					EXECUTION_ERROR_ ("Out command called with empty stack");

				outstream << std::to_string (static_cast <double> (stack.top ()) / NUMBERS_MODIFIER) << '\n';

				break;
			}

			case ByteCode::hlt:
			{
				running = false;

				break;
			}

			case ByteCode::ver:
			{
				PUSH_NUMBER_ (ASSEMBLER_VERSION);

				break;
			}

			case ByteCode::hlp:
			{
				outstream << "Assembler commands reference:\n";
				for (size_t i = 0; i < ByteCodesAmount; i++)
					outstream << ByteCodeToStr (static_cast <ByteCode> (i)) << "\n";

				break;
			}

			case ByteCode::man:
			{
				NEXT_VALUE_ (man_cmd);
				outstream << CommandManual (static_cast <ByteCode> (man_cmd)) << '\n';

				break;
			}

			default: break;
		}
	}

	#undef ERROR_
	#undef NEXT_VALUE_
	#undef POP_SAFE_
	#undef POP_NUMBER_
	#undef PUSH_NUMBER_
	#undef OPERATOR_CASE_
}

//------------------------------