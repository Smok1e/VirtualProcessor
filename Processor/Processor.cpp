#include <string>

#include "Processor.h"
#include "Utils.hpp"

//------------------------------

Processor::Processor ():
	m_stack   (),
	m_program (),

	m_stream_in  (nullptr),
	m_stream_out (nullptr),
	m_stream_err (nullptr)
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