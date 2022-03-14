#include <string>

#include "Processor.h"
#include "Utils.hpp"

//------------------------------

Processor::Processor ():
	m_stack   (),
	m_program (),

	m_stream_in  (nullptr),
	m_stream_out (nullptr),
	m_stream_err (nullptr),

	m_next_data_index (0),

	m_retval (0),

	m_registers {}
{}

//------------------------------

void Processor::setProgram (const ProgramContainer& program)
{
	m_program = program;
}

const ProgramContainer& Processor::getProgram ()
{
	return m_program;
}

//------------------------------

void          Processor::setInputStream  (std::istream* stream) { m_stream_in  = stream; }
void          Processor::setOutputStream (std::ostream* stream) { m_stream_out = stream; }
void          Processor::setErrorStream  (std::ostream* stream) { m_stream_err = stream; }
std::istream* Processor::getInputStream  ()                     { return m_stream_in;    }
std::ostream* Processor::getOutputStream ()                     { return m_stream_out;   }
std::ostream* Processor::getErrorStream  ()                     { return m_stream_err;   }

void Processor::defaultIO ()
{
	setInputStream  (&std::cin );
	setOutputStream (&std::cout);
	setErrorStream  (&std::cerr);
}

//------------------------------

std::string Processor::input ()
{
	if (!m_stream_in)
	{
		error ("Warning: Input stream not set, but program trying to read input\n");
		return "";
	}

	std::string in;
	*m_stream_in >> in;
	return in;
}

void Processor::output (const char* format, ...)
{
	va_list args = {};
	va_start (args, format);
	print (m_stream_out, format, args);
	va_end (args);
}

void Processor::error (const char* format, ...)
{
	va_list args = {};
	va_start (args, format);
	print (m_stream_out, format, args);
	va_end (args);
}

//------------------------------

void Processor::print (std::ostream* stream, const char* format, va_list args)
{
    if (!stream) return;

	size_t len = vsnprintf (nullptr, 0, format, args);
	char* str = new char[len+1];
	
	vsprintf_s (str, len+1, format, args);
	stream -> write (str, len);
	
	delete[] (str);
}

//------------------------------

double Processor::run ()
{			
	m_next_data_index = 0;
	m_retval          = 0;

	unsigned version = static_cast <unsigned> (nextStackValue ());
	if (version != ASSEMBLER_VERSION)
	{
		error ("Fatal error: Wrong version (program version is %d, processor version is %d)\n", version, ASSEMBLER_VERSION);
		throw processor_error ("Wrong program version (%d instead of %d)", version, ASSEMBLER_VERSION);
	}

	bool running = true;
	while (running)
	{
		ByteCode cmd = nextInstruction ();
		if (cmd == ByteCode::hlt)
		{
			running = false;
			break;
		}

		processInstruction (cmd);
	}

	return m_retval;
}

//------------------------------

template <typename value_t>
value_t Processor::nextValue ()
{
	if (m_next_data_index >= m_program.bytes ())
	{
		error ("Fatal error: Program data corrupted");
		throw processor_error ("Failed to get next value");
	}

	value_t value = m_program.get <value_t> (m_next_data_index);
	m_next_data_index += sizeof (value_t);

	return value;
}

//------------------------------

ByteCode Processor::nextInstruction ()
{
	return nextValue <ByteCode> ();
}

stack_value_t Processor::nextStackValue ()
{
	return nextValue <stack_value_t> ();
}

TokenType Processor::nextArgType ()
{
	return nextValue <TokenType> ();
}

byte_t Processor::nextByte ()
{
	return nextValue <byte_t> ();
}

//------------------------------

void Processor::jump (uintptr_t addr)
{
	if (addr >= m_program.bytes ())
	{
		error ("Fatal error: Invalid address (0x%08X), maximum possible address is 0x%08X", addr, m_program.bytes ());
		throw processor_error ("Invalid address (0x%08X), maximum possible address is 0x%08X", addr, m_program.bytes ());
	}

	m_next_data_index = addr;
}

//------------------------------

void Processor::processInstruction (ByteCode cmd)
{
	#define ACD_(instruction, args, descr, ...) case ByteCode::##instruction: { __VA_ARGS__; break; }

	switch (cmd)
	{
		COMMANDS_DEFINES_

		default:
		{
			error ("Fatal error: Unknown instruction 0x%02X\n", cmd);
			throw processor_error ("Unknown instruction 0x%02X\n", cmd);

			break;
		}
	}

	#undef ACD_
}

//------------------------------

void Processor::push (stack_value_t value)
{
	m_stack.push (value);
}

void Processor::push (ByteCode cmd)
{
	push (static_cast <stack_value_t> (cmd));
}

void Processor::push (double number)
{
	push (static_cast <stack_value_t> (number * NUMBERS_MODIFIER));
}

//------------------------------

stack_value_t Processor::popValue ()
{
	if (m_stack.empty ())
	{
		error ("Fatal error: trying to pop value, but stack is empty");
		throw processor_error ("Trying to pop empty stack");
	}

	return m_stack.pop ();
}

ByteCode Processor::popInstruction ()
{
	return static_cast <ByteCode> (popValue ());
}

double Processor::popNumber ()
{
	return static_cast <double> (popValue ()) / NUMBERS_MODIFIER;
}

void Processor::pop ()
{
	popValue ();
}

//------------------------------

stack_value_t Processor::top ()
{
	return m_stack.top ();
}

//------------------------------

void Processor::regSet (size_t index, stack_value_t value)
{
	if (index >= REGISTERS_COUNT)
	{
		error ("Fatal error: Invalid register index 0x%02X\n", index);
		throw processor_error ("Invalid register index 0x%02X", index);
	}

	m_registers[index] = value;
}

void Processor::regSet (size_t index, ByteCode cmd)
{
	regSet (index, static_cast <stack_value_t> (cmd));
}

void Processor::regSet (size_t index, double number)
{
	regSet (index, static_cast <stack_value_t> (floor (number * NUMBERS_MODIFIER)));
}

//------------------------------

stack_value_t Processor::regGetStackValue (size_t index)
{
	if (index >= REGISTERS_COUNT)
	{
		error ("Fatal error: Invalid register index 0x%02X\n", index);
		throw processor_error ("Invalid register index 0x%02X", index);
	}

	return m_registers[index];
}

ByteCode Processor::regGetInstruction (size_t index)
{
	return static_cast <ByteCode> (regGetStackValue (index));
}

double Processor::regGetNumber (size_t index)
{
	return static_cast <double> (regGetStackValue (index)) / NUMBERS_MODIFIER;
}

//------------------------------

void Processor::setReturnValue (double retval)
{
	m_retval = retval;
}

//------------------------------

processor_error::processor_error (const char* format, ...):
	exception_message ()
{
	va_list args = {};
	va_start (args, format);
	m_what = Format (format, args);
	va_end (args);
}

processor_error::processor_error (const std::string& string):
	exception_message (string)
{}

//------------------------------