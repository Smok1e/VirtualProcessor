#pragma once

//------------------------------

#include <vector>
#include <string>
#include <cstdarg>
#include <iostream>
#include <ostream>
#include <sstream>

//------------------------------

#include "ExceptionMessage.h"
#include "ByteCode.h"
#include "ProgramContainer.h"

//------------------------------

class assembler_error: public exception_message 
{ 
public:
	explicit assembler_error (const char* format, ...);
	explicit assembler_error (const std::string& string);

};

//------------------------------

class Assembler
{
public:
	struct listing_settings
	{
		unsigned __int8 line_digits;
		unsigned __int8 addr_digits;
		unsigned __int8 cont_bytes;
		bool            cont_print_binary;
	};

	Assembler ();

	~Assembler ();

	void        setSourceCode (const char* source);
	const char* getSourceCode ();

	void loadSourceCode (const char* filename);

	void                    setProgram (const ProgramContainer& program);
	const ProgramContainer& getProgram ();

	void             setListingSettings (listing_settings settings);
	listing_settings getListingSettings ();

	void          setListingStream (std::ostream* stream);
	std::ostream* getListingStream ();

	void compile ();

	void dumpTokens ();

private:
	ProgramContainer m_program;
	size_t           m_program_index;
	int              m_next_token_index;
	std::ostream*    m_listing_stream;

	struct source_code_container
	{
		struct token
		{
			const char*   begin;
			const char*   end;
			size_t        len;
			size_t        number;
			size_t        line_number;
			stack_value_t value;
			
			TokenType type;
		};

		struct line
		{
			const char* begin;
			const char* end;
			size_t      len;
			size_t      number;
		};

		const char* begin;
		size_t      len;

		line*  lines;
		token* tokens;
		size_t lines_count;
		size_t tokens_count;

		      char* loaded_source;
		const char* filename;
	} m_source_code;

	struct name_pair
	{
		const char* name_begin;
		const char* name_end;
		size_t      name_len;

		size_t address;
	};

	std::vector <name_pair> m_name_table;

	listing_settings m_listing_settings;
	
	bool assemble ();

	TokenType                    determineTokenType     (const char* begin, const char* end);
	stack_value_t                interpretNumberToken   (const char* begin, size_t len);
	stack_value_t                interpretCommandToken  (const char* begin, size_t len);
	stack_value_t                interpretRegisterToken (const char* begin, size_t len);
	stack_value_t                interpretAddressToken  (const char* begin, size_t len);
	stack_value_t                interpretLabelToken    (const char* begin, size_t len);
	source_code_container::token interptetToken         (const char* begin, const char* end, size_t number, size_t line_number);

	void compileInstruction (const std::initializer_list <TokenType>& args);

	static const char* StrTokenType (TokenType type);

	double   toNumber   (stack_value_t value);
	ByteCode toByteCode (stack_value_t value);

	void tokenize      ();
	void releaseTokens ();

	source_code_container::token followingToken (                size_t offset = 0);
	source_code_container::token followingToken (TokenType type, size_t offset = 0);

	source_code_container::token nextToken ();
	source_code_container::token nextToken (TokenType type);

	void listing      (const char* format, ...);
	void listing_line (int line, uintptr_t addr, byte_t* content_begin, size_t content_size, const char* message, int len = -1);
};

//------------------------------

#define assembler_assert(expr) if (!(expr)) throw assembler_error ("Assertion failed in file %s, function %s, line #%d: '" #expr "' == 0", __FILE__, __func__, __LINE__);

//------------------------------

extern Assembler::listing_settings DEFAULT_LISTING_SETTINGS;

//------------------------------

const char* Assembler_strpbrk    (const char* begin, const char* delimiters);
const char* Assembler_strpbrk_x2 (const char* begin, const char* delimiters1, const char* delimiters2);
const char* Assembler_strnpbrk   (const char* begin, const char* delimiters, size_t limit);

//------------------------------