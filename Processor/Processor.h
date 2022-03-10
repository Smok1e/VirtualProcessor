#pragma once

//------------------------------

#include <iostream>

#include "ExceptionMessage.h"
#include "ByteCode.h"
#include "Stack.h"
#include "ProgramContainer.h"

//------------------------------

typedef Stack <stack_value_t> ProgramStack;

//------------------------------

class processor_error: public exception_message 
{ 
public:
	explicit processor_error (const char* format, ...);
	explicit processor_error (const std::string& string);

};

//------------------------------

class Processor
{
public:
	Processor ();

	void                    setProgram (const ProgramContainer& program);
	const ProgramContainer& getProgram ();

	void          setInputStream  (std::istream* stream);
	void          setOutputStream (std::ostream* stream);
	void          setErrorStream  (std::ostream* stream);
	std::istream* getInputStream  ();
	std::ostream* getOutputStream ();
	std::ostream* getErrorStream  ();

private:
	ProgramStack     m_stack;
	ProgramContainer m_program;

	std::istream* m_stream_in;
	std::ostream* m_stream_out;
	std::ostream* m_stream_err;

};

//------------------------------