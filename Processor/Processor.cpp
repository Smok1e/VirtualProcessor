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

	m_registers {},
	m_memory    {},

	m_test_marker (0)
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

byte_type Processor::nextByteType ()
{
	return nextValue <byte_type> ();
}

byte_t Processor::nextByte ()
{
	return nextValue <byte_t> ();
}

stack_value_t* Processor::nextAddress (byte_type args)
{
	if (args.is_address)
	{
		stack_value_t lft = 0;
		if      (args.arg1_type == TokenType::Numeric ) lft = nextStackValue ();
		else if (args.arg1_type == TokenType::Register) lft = regGetStackValue (nextByte ());

		stack_value_t rgt = 0;
		if      (args.arg2_type == TokenType::Numeric ) rgt = nextStackValue ();
		else if (args.arg2_type == TokenType::Register) rgt = regGetStackValue (nextByte ());

		uintptr_t addr = static_cast <uintptr_t> (static_cast <double> (lft + rgt) / NUMBERS_MODIFIER);
		if (addr + sizeof (stack_value_t) > PROCESSOR_MEMORY_SIZE)
		{
			error ("Fatal error: Invalid address - 0x%08X\n", addr);
			throw processor_error ("Invalid address - 0x%08X", addr);
		}

		return reinterpret_cast <stack_value_t*> (m_memory + addr);
	}

	else
	{
		if (args.arg1_type == TokenType::Register)
			return m_registers + nextByte ();

		else if (args.arg1_type == TokenType::Numeric)
		{
			static stack_value_t tmp_buff = 0;
			tmp_buff = nextStackValue ();
			return &tmp_buff;
		}
	}
}

//------------------------------

void Processor::jump (uintptr_t addr)
{
	if (addr >= m_program.bytes ())
	{
		error ("Fatal error: Invalid address (0x%08X), maximum possible address is 0x%08X\n", addr, m_program.bytes ());
		throw processor_error ("Invalid address (0x%08X), maximum possible address is 0x%08X", addr, m_program.bytes ());
	}

	if (addr == ASSEMBLER_INVALID_ADDRESS)
	{
		error ("Fatal error: Invalid address (0x%08X)\n", addr);
		throw processor_error ("Invalid address (0x%08X)", addr);
	}

	m_next_data_index = addr;
}

//------------------------------

void Processor::processInstruction (ByteCode cmd)
{
	byte_type args = nextByteType ();
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

void Processor::pushCommand (ByteCode cmd)
{
	push (static_cast <stack_value_t> (cmd));
}

void Processor::pushNumber (double number)
{
	push (static_cast <stack_value_t> (floor (number * NUMBERS_MODIFIER)));
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

void Processor::regSetCommand (size_t index, ByteCode cmd)
{
	regSet (index, static_cast <stack_value_t> (cmd));
}

void Processor::regSetNumber (size_t index, double number)
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

void Processor::memSet (double address, stack_value_t value)
{
	uintptr_t addr = static_cast <uintptr_t> (floor (address));

	if (addr + sizeof (stack_value_t) > PROCESSOR_MEMORY_SIZE)
	{
		error ("Fatal error: Invalid address - 0x%08X\n", addr);
		throw processor_error ("Invalid address - 0x%08X", addr);
	}

	*reinterpret_cast <stack_value_t*> (m_memory + addr) = value;
}

stack_value_t Processor::memGet (double address)
{
	uintptr_t addr = static_cast <uintptr_t> (floor (address));

	if (addr + sizeof (stack_value_t) > PROCESSOR_MEMORY_SIZE)
	{
		error ("Fatal error: Invalid address - 0x%08X\n", addr);
		throw processor_error ("Invalid address - 0x%08X", addr);
	}

	return *reinterpret_cast <stack_value_t*> (m_memory + addr);	
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