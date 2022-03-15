#pragma once

//------------------------------

#include <vector>
#include <iostream>
#include <fstream>

#include "Utils.hpp"
#include "Commands.h"

//------------------------------

typedef          __int32            stack_value_t;
typedef unsigned __int8             byte_t;
typedef std::vector <stack_value_t> program_t;

//------------------------------
/*

��������� ���������������� ����������:
������������� ���������� (ByteCode, ��� 1 ���� � ��������� ������)
�����, ��� ������� ��������� ����������, ������� ����� ��������� ��������� ����� (�������� ACD_ (push, TOKENS_ ({TokenType::Numeric | TokenType::Register}), ...))
� ��������� ������� �� 1 �����, ������� ���������� ��� ���� ������, ������� ������� � ��������� �� �����������. � ��� ��� ������� ���������. �����, ��� ���� 
�������� ����� ������������� ���, �� ����� �� ����������� ������� �������� ����������.

��������� ��������, ��� �������� ����� ���� ������ �����, ���� � ���� ��������� [������ ������ ���� ����� �������� 1], ��������� [������ ��� ��������� �������� ���� ��������� ���].

����� ���������� � ���������� � ����� � ����������, ������� �������� ���������� (stack_value_t, ��� 4 ����� � ��������� ������), ���� ������� �������.

� ����� ��������� ���������� ����� ����� ���: 
[�������������][���� ����������][�������� ����������]

��������� ��������� �������� ������������������ �� ���������������� ����������, ������ ������� ������ ����. 
� ������ ��������� ����������� ������������� ������ ���������� (stack_value_t ��� 4 �������� ����������� �����).
� ����� ��������� ������ ��������� ���������� hlt (��� ����������).

*/
//------------------------------

#define ACD_(cmd, argc, desc, ...) cmd,

enum class ByteCode: byte_t
{
	reserved,

	COMMANDS_DEFINES_

	amount,
	unknown
};

#undef ACD_

//------------------------------

typedef byte_t token_base_t;
enum class TokenType: token_base_t
{
	Keyword  = 0b00000010,
	Numeric  = 0b00000100,
	Register = 0b00001000,
	Newline  = 0b00010000,

	None     = 0b11111111
};

//------------------------------

extern const unsigned ByteCodesBegin;
extern const unsigned ByteCodesAmount;

//------------------------------

extern const char*            CODE_DELIMITERS;
extern const char*            LINE_DELIMITERS;
extern const char*            COMMENT_SEQUENCE;
extern const unsigned         COMMENT_SEQUENCE_LEN;
extern const char*            REGISTER_SEQUENCE;
extern const unsigned         REGISTER_SEQUENCE_LEN;

//     const unsigned         NUMBERS_ACURACY    = 3;
       const unsigned         NUMBERS_MODIFIER	 = 256;
       const unsigned         ASSEMBLER_VERSION  = 17;
       const unsigned         ASSEMBLER_BUFFSIZE = 1024;
       const unsigned __int32 PROGRAM_SIGNATURE  = TXT232UINT ("Meow"); // MEOW - Mcasm Executable Outrage Waffle
	   const unsigned __int8  REGISTERS_COUNT    = 4;

//------------------------------

struct program_header
{
	unsigned __int32 signature;
	unsigned __int16 version;
	unsigned __int32 bytes_count;
	unsigned __int8  numbers_modifier;
};

//------------------------------

ByteCode    TranslateToByteCode (const char* str, size_t len);
const char* ByteCodeToStr       (ByteCode code);
const char* CommandManual       (ByteCode code);

int         RegisterIndex    (const char* begin, size_t len);
const char* StrRegisterIndex (size_t index);

bool IsSingleTokenType (TokenType type);

//------------------------------

TokenType operator | (TokenType lft, TokenType rgt);
TokenType operator & (TokenType lft, TokenType rgt);

//------------------------------