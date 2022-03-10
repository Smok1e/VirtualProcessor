#pragma once

//------------------------------

#include <vector>
#include <iostream>
#include <fstream>

#include "Utils.hpp"
#include "Commands.h"

//------------------------------

typedef unsigned __int32            stack_value_t;
typedef std::vector <stack_value_t> program_t;
typedef char                        byte_t;

//------------------------------

#define ACD_(cmd, argc, desc, ...) cmd,

enum class ByteCode
{
	COMMANDS_DEFINES_

	amount,
	unknown
};

#undef ACD_

//------------------------------

enum class TokenType
{
	keyword,
	numeric,
	newline,

	amount,
	none
};

//------------------------------

extern const unsigned ByteCodesAmount;

extern const char*            CODE_DELIMITERS;
extern const char*            LINE_DELIMITERS;
extern const char*            COMMENT_SEQUENCE;
extern const unsigned         COMMENT_SEQUENCE_LEN;
       const unsigned         NUMBERS_ACURACY               = 3;
       const unsigned         NUMBERS_MODIFIER	            = static_cast <unsigned> (pow (10, NUMBERS_ACURACY));
       const unsigned         ASSEMBLER_VERSION             = 12;
       const unsigned         ASSEMBLER_BUFFSIZE            = 1024;
       const unsigned __int32 PROGRAM_SIGNATURE             = TXT232UINT ("Meow");
       const unsigned         LISTING_LINE_NUMBER_LENGTH    = 3;
       const unsigned         LISTING_ADDRESS_NUMBER_LENGTH = 4;

//------------------------------

struct program_header
{
	unsigned __int32 signature;
	unsigned __int16 version;
	unsigned __int32 elem_count;
	unsigned __int32 bytes_count;
	unsigned __int8  numbers_acuracy;
};

//------------------------------

ByteCode    TranslateToByteCode (const char* str);
const char* ByteCodeToStr       (ByteCode code);
const char* CommandManual       (ByteCode code);

//------------------------------