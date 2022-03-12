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

	void defaultIO ();

	std::string input  ();
	void        output (const char* format, ...);
	void        error  (const char* format, ...);

	double run ();

private:
	ProgramStack     m_stack;
	ProgramContainer m_program;

	std::istream* m_stream_in;
	std::ostream* m_stream_out;
	std::ostream* m_stream_err;

	size_t m_next_data_index;
	
	double m_retval;

	void print (std::ostream* stream, const char* format, va_list args);

	template <typename value_t> 
	value_t nextValue ();

	ByteCode      nextInstruction ();
	stack_value_t nextStackValue  ();

	void push (stack_value_t value );
	void push (ByteCode      cmd   );
	void push (double        number);

	stack_value_t popValue       ();
	ByteCode      popInstruction ();
	double        popNumber      ();
	void          pop            ();

	void processInstruction (ByteCode cmd);

	void setReturnValue (double retval);
};

//------------------------------