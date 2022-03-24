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

	stack_value_t m_registers[REGISTERS_COUNT];
	byte_t        m_memory   [PROCESSOR_MEMORY_SIZE];

	int m_test_marker;

	// IO
	void print (std::ostream* stream, const char* format, va_list args);

	// Program data
	template <typename value_t> value_t nextValue ();
	ByteCode                            nextInstruction ();
	stack_value_t                       nextStackValue  ();
	byte_type                           nextByteType    ();
	byte_t                              nextByte        ();
	
	stack_value_t* nextAddress (byte_type args);

	void jump (uintptr_t addr);


	// Stack
	void push        (stack_value_t value );
	void pushCommand (ByteCode      cmd   );
	void pushNumber  (double        number);

	stack_value_t popValue       ();
	ByteCode      popInstruction ();
	double        popNumber      ();
	void          pop            ();
	stack_value_t top            ();

	// Registers
	void          regSet            (size_t index, stack_value_t value );
	void          regSetCommand     (size_t index, ByteCode      cmd   );
	void          regSetNumber      (size_t index, double        number);

	stack_value_t regGetStackValue  (size_t index);
	ByteCode      regGetInstruction (size_t index);
	double        regGetNumber      (size_t index);

	// Memory
	void          memSet (double address, stack_value_t value);
	stack_value_t memGet (double address);

	// Other
	void processInstruction (ByteCode cmd);
	void setReturnValue (double retval);
};

//------------------------------