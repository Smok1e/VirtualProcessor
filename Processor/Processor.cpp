#include <string>
#include <thread>

#include "Processor.h"
#include "Utils.hpp"

#include <shellscalingapi.h>
#include <TXU.h>

//------------------------------

int InitProcessor ()
{
	HMODULE module = LoadLibraryA ("Shcore.dll");
	assert (module);

	HRESULT (WINAPI* SetProcessDpiAwareness) (PROCESS_DPI_AWARENESS value) = reinterpret_cast <HRESULT (WINAPI*) (PROCESS_DPI_AWARENESS value)> (GetProcAddress (module, "SetProcessDpiAwareness"));
	assert (SetProcessDpiAwareness);

	SetProcessDpiAwareness (PROCESS_PER_MONITOR_DPI_AWARE);

	FreeLibrary (module);

	_txConsole = -1;
	return 1;
}

int _ProcessorInitialized = InitProcessor ();

//------------------------------

Processor::Processor ():
	m_stack      (),
	m_call_stack (),
	m_program    (),

	m_stream_in  (nullptr),
	m_stream_out (nullptr),
	m_stream_err (nullptr),

	m_next_data_index (0),

	m_retval (0),

	m_registers {},
	m_memory    {},
	
	m_background (nullptr),

	m_test_marker (0)
{
	setNumbersModifier (NUMBERS_MODIFIER);
}

Processor::~Processor ()
{
	if (m_background)
		txDeleteDC (m_background);
}

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
	print (m_stream_err, format, args);
	va_end (args);
}

void Processor::fatal (const char* format, ...)
{
	va_list args = {};
	va_start (args, format);
	print (m_stream_err, format, args);
	va_end (args);

	error ("Call stack:\n");
	if (m_call_stack.empty ())
		error ("Empty\n");

	else for (stack_value_t addr: m_call_stack)
		error ("  0x%0*X\n", 2 * sizeof (addr), addr);
	error ("\n");
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
	static const char* background_filename = "background.bmp";
	m_background = txLoadImage (background_filename);
	if (!m_background)
		error ("Warning: Failed to load '%s'\n", background_filename);

	m_stack.clear      ();
	m_call_stack.clear ();

	m_next_data_index = 0;
	m_retval          = 0;

	unsigned version = static_cast <unsigned> (nextStackValue ());
	if (version != ASSEMBLER_VERSION)
	{
		fatal ("Fatal error: Wrong version (program version is %d, processor version is %d)\n", version, ASSEMBLER_VERSION);
		throw processor_error ("Wrong program version (%d instead of %d)", version, ASSEMBLER_VERSION);
	}

	stack_value_t numbers_modifier = nextStackValue ();
	setNumbersModifier (numbers_modifier);

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
		fatal ("Fatal error: Program data corrupted");
		throw processor_error ("Failed to get next value");
	}

	value_t value = m_program.get <value_t> (m_next_data_index);
	m_next_data_index += sizeof (value_t);

	return value;
}

//------------------------------

double Processor::nextNumber ()
{
	return static_cast <double> (nextValue <stack_value_t> ());
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
		if      (args.arg1_type == TokenType::Numeric ) lft = nextNumber ();
		else if (args.arg1_type == TokenType::Register) lft = regGetStackValue (nextByte ());

		stack_value_t rgt = 0;
		if      (args.arg2_type == TokenType::Numeric ) rgt = nextNumber ();
		else if (args.arg2_type == TokenType::Register) rgt = regGetStackValue (nextByte ());

		uintptr_t addr = static_cast <uintptr_t> (static_cast <double> (lft + rgt) / getNumbersModifier ());
		if (addr + sizeof (stack_value_t) > PROCESSOR_MEMORY_SIZE)
		{
			fatal ("Fatal error: Invalid address - 0x%08X\n", addr);
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
			tmp_buff = nextNumber ();
			return &tmp_buff;
		}
	}

	return nullptr;
}

//------------------------------

void Processor::jump (uintptr_t addr)
{
	if (addr >= m_program.bytes ())
	{
		fatal ("Fatal error: Invalid address (0x%08X), maximum possible address is 0x%08X\n", addr, m_program.bytes ());
		throw processor_error ("Invalid address (0x%08X), maximum possible address is 0x%08X", addr, m_program.bytes ());
	}

	if (addr == ASSEMBLER_INVALID_ADDRESS)
	{
		fatal ("Fatal error: Invalid address (0x%08X)\n", addr);
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
			fatal ("Fatal error: Unknown instruction 0x%02X\n", cmd);
			throw processor_error ("Unknown instruction 0x%02X\n", cmd);

			break;
		}
	}

	#undef ACD_
}

//------------------------------

void Processor::push (stack_value_t value)
{
	if (PROCESSOR_STACK_LIMIT >= 0 && m_stack.size () >= PROCESSOR_STACK_LIMIT)
	{
		fatal ("Fatal error: Stack limit reached (%d)\n", PROCESSOR_STACK_LIMIT);
		throw processor_error ("Stack limit reached (%d)\n", PROCESSOR_STACK_LIMIT);
	}

	m_stack.push (value);
}

void Processor::pushCommand (ByteCode cmd)
{
	push (static_cast <stack_value_t> (cmd));
}

void Processor::pushNumber (double number)
{
	push (static_cast <stack_value_t> (floor (number * getNumbersModifier ())));
}

//------------------------------

stack_value_t Processor::popValue ()
{
	if (m_stack.empty ())
	{
		fatal ("Fatal error: trying to pop value, but stack is empty\n");
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
	return static_cast <double> (popValue ()) / getNumbersModifier ();
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

void Processor::pushCall (stack_value_t addr)
{
	if (PROCESSOR_CALL_STACK_LIMIT >= 0 && m_call_stack.size () >= PROCESSOR_CALL_STACK_LIMIT)
	{
		fatal ("Fatal error: Call stack limit reached (%d)\n", PROCESSOR_CALL_STACK_LIMIT);
		throw processor_error ("Call stack limit reached (%d)\n", PROCESSOR_CALL_STACK_LIMIT);
	}

	m_call_stack.push (addr);
}

stack_value_t Processor::popCall ()
{
	if (m_call_stack.empty ())
	{
		fatal ("Fatal error: Call stack is empty (using jmp instead of call?)");
		throw processor_error ("Call stack is empty");
	}

	return m_call_stack.pop ();
}

stack_value_t Processor::topCall ()
{
	if (m_call_stack.empty ())
	{
		fatal ("Fatal error: Call stack is empty (using jmp instead of call?)");
		throw processor_error ("Call stack is empty");
	}

	return m_call_stack.top ();
}

//------------------------------

void Processor::regSet (size_t index, stack_value_t value)
{
	if (index >= REGISTERS_COUNT)
	{
		fatal ("Fatal error: Invalid register index 0x%02X\n", index);
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
	regSet (index, static_cast <stack_value_t> (floor (number * getNumbersModifier ())));
}

//------------------------------

stack_value_t Processor::regGetStackValue (size_t index)
{
	if (index >= REGISTERS_COUNT)
	{
		fatal ("Fatal error: Invalid register index 0x%02X\n", index);
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
	return static_cast <double> (regGetStackValue (index)) / getNumbersModifier ();
}

//------------------------------

void Processor::setNumbersModifier (stack_value_t modifier)
{
	regSet (NUMBERS_MODIFIER_REGISTER, modifier);
}

stack_value_t Processor::getNumbersModifier ()
{
	return regGetStackValue (NUMBERS_MODIFIER_REGISTER);
}

//------------------------------

void Processor::memSet (double address, stack_value_t value)
{
	uintptr_t addr = static_cast <uintptr_t> (floor (address));

	if (addr + sizeof (stack_value_t) > PROCESSOR_MEMORY_SIZE)
	{
		fatal ("Fatal error: Invalid address - 0x%08X\n", addr);
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

void Processor::openWindow (double size_x, double size_y)
{
	if (txWindow ())
		return;

	txCreateWindow (static_cast <int> (size_x * (PROCESSOR_PIXEL_SCALE + 1)), static_cast <int> (size_y * (PROCESSOR_PIXEL_SCALE + 1)));
	SetWindowTextA (txWindow (), "MCASM Virtual processor display window");

	static const char* icon_filename = "icon.ico";
	HICON icon = (HICON) LoadImageA (NULL, icon_filename, IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_SHARED);
	if (icon)
	{
		SendMessageA (txWindow (), WM_SETICON, ICON_SMALL, (LPARAM) icon);
		SendMessageA (txWindow (), WM_SETICON, ICON_BIG,   (LPARAM) icon);
	}

	else
	{
		error ("Warning: Failed to load '%s'\n", icon_filename);
	}
}

void Processor::display (double begin_addr)
{
	static COLORREF background_color = TX_BLACK;
	static COLORREF foreground_color = TX_WHITE;
	static COLORREF frame_color      = RGB (64, 64, 64);

	if (!txWindow ())
		return;

	if (m_background)
		StretchBlt (txDC (), 0, 0, txGetExtentX (), txGetExtentY (), m_background, 0, 0, txGetExtentX (m_background), txGetExtentY (m_background), SRCCOPY);

	else
	{
		txSetFillColor (frame_color);
		txClear        ();
	}

	stack_value_t* buffer = reinterpret_cast <stack_value_t*> (m_memory + static_cast <size_t> (floor (begin_addr)));
	int size_x = txGetExtentX () / (PROCESSOR_PIXEL_SCALE + 1);
	int size_y = txGetExtentY () / (PROCESSOR_PIXEL_SCALE + 1);

	txBegin ();

	for (size_t x = 0; x < size_x; x++)
	for (size_t y = 0; y < size_y; y++)
	{
		COLORREF color = static_cast <COLORREF> (buffer[y*size_x+x]);
		txSetColor     (color, 0);
		txSetFillColor (color);

		int x0 = x  * (PROCESSOR_PIXEL_SCALE + 1);
		int y0 = y  * (PROCESSOR_PIXEL_SCALE + 1);
		int x1 = x0 + PROCESSOR_PIXEL_SCALE;
		int y1 = y0 + PROCESSOR_PIXEL_SCALE;

		txRectangle (x0, y0, x1, y1);
	}

	txEnd ();
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