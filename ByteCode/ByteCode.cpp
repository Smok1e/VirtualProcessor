#include <stdexcept>

#include "ByteCode.h"
#include "Utils.hpp"
#include "ProgramContainer.h"

//------------------------------

const unsigned ByteCodesAmount = static_cast <unsigned> (ByteCode::amount);

const char*    CODE_DELIMITERS      = " \r\t";
const char*    LINE_DELIMITERS      = "\n";
const char*    COMMENT_SEQUENCE     = ">";
const unsigned COMMENT_SEQUENCE_LEN = strlen (COMMENT_SEQUENCE);

//------------------------------

ByteCode TranslateToByteCode (const char* str)
{
	#define ACD_(cmd, argc, desc, ...) if (strncmp (str, #cmd, sizestr (#cmd)) == 0) { return ByteCode::##cmd; };
	COMMANDS_DEFINES_
	#undef ACD_

	return ByteCode::unknown;
}

//------------------------------

const char* ByteCodeToStr (ByteCode code)
{
	#define ACD_(cmd, argc, desc, ...) case ByteCode::##cmd: { return #cmd; break; }
	switch (code) { COMMANDS_DEFINES_ };
	#undef ACD_

	return nullptr;
}

//------------------------------

const char* CommandManual (ByteCode code)
{
	#define ACD_(cmd, argc, desc, ...) case ByteCode::##cmd: { return desc; break; }
	switch (code) { COMMANDS_DEFINES_ };
	#undef ACD_
	
	return "Unknown command";
}

//------------------------------