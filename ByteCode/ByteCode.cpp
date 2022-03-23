#include <stdexcept>

#include "ByteCode.h"
#include "Utils.hpp"
#include "ProgramContainer.h"

//------------------------------

const unsigned ByteCodesBegin  = static_cast <unsigned> (ByteCode::reserved) + 1;
const unsigned ByteCodesAmount = static_cast <unsigned> (ByteCode::amount  ) - 1;

const char*    CODE_DELIMITERS       = " \r\t+-";
const char*    LINE_DELIMITERS       = "\n";

const char*    COMMENT_SEQUENCE      = ">";
const unsigned COMMENT_SEQUENCE_LEN  = strlen (COMMENT_SEQUENCE);

const char*    REGISTER_SEQUENCE     = "@";
const unsigned REGISTER_SEQUENCE_LEN = strlen (REGISTER_SEQUENCE);

//------------------------------

ByteCode TranslateToByteCode (const char* str, size_t len)
{
	#define ACD_(cmd, argc, desc, ...) if (strncmp (str, #cmd, len) == 0) { return ByteCode::##cmd; };
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

int RegisterIndex (const char* begin, size_t len)
{
	const char* register_begin        = begin;
	const char* register_end_sequence = register_begin + 1;

	if (*register_end_sequence != 'x')
		return -1;

	int register_index = *register_begin - 'a';

	if (register_index < 0 || register_index >= REGISTERS_COUNT)
		return -1;

	return register_index;
}

//------------------------------

const char* StrRegisterIndex (size_t index)
{
	static char buffer[ASSEMBLER_BUFFSIZE] = "";
	sprintf_s (buffer, "%s%cx", REGISTER_SEQUENCE, 'a' + index);

	return buffer;
}

//------------------------------

bool IsSingleTokenType (TokenType type)
{
	return IsOnly1BitSet (static_cast <token_base_t> (type));
}

//------------------------------

TokenType operator | (TokenType lft, TokenType rgt)
{
	return static_cast <TokenType> (static_cast <token_base_t> (lft) | static_cast <token_base_t> (rgt));
}

TokenType operator & (TokenType lft, TokenType rgt)
{
	return static_cast <TokenType> (static_cast <token_base_t> (lft) & static_cast <token_base_t> (rgt));
}

//------------------------------