#pragma once

//------------------------------

#include <iostream>

#include "ExceptionMessage.h"
#include "ByteCode.h"
#include "Stack.h"

//------------------------------

typedef Stack <stack_value_t> ProgramStack;
typedef exception_message processor_error;

//------------------------------

void RunProgram (const program_t& program, std::ostream& outstream = std::cout, std::istream& instream = std::cin, std::ostream& errstream = std::cerr);

//------------------------------