#include <cstring>
#include <stdexcept>
#include <string>

#include "Assembler.h"
#include "Utils.hpp"
#include "ByteCode.h"

#pragma warning (disable: 4018)

//------------------------------
//                                                       line digits, addr digits
Assembler::listing_settings DEFAULT_LISTINGS_SETTINGS = {3,           4          };

//------------------------------

Assembler::Assembler ():
	m_program          (),
	m_source_code      ({0}),
	m_next_token_index (0),
	m_listing_stream   (nullptr),
	m_listing_settings (DEFAULT_LISTINGS_SETTINGS)
{}

//------------------------------

Assembler::~Assembler ()
{
	releaseTokens ();
}

//------------------------------

void Assembler::setSourceCode (const char* source)
{
	m_source_code.begin = source;
	m_source_code.len   = strlen (source);

	tokenize ();
}

const char* Assembler::getSourceCode ()
{
	return m_source_code.begin;
}

//------------------------------

void Assembler::setProgram (const ProgramContainer& program)
{
	m_program.clear ();
	m_program = program;
	m_program.shrink ();
}

const ProgramContainer& Assembler::getProgram ()
{
	return m_program;
}

//------------------------------

void Assembler::setListingStream (std::ostream* stream)
{
	m_listing_stream = stream;
}

std::ostream* Assembler::getListingStream ()
{
	return m_listing_stream;
}

//------------------------------

void Assembler::setListingSettings (listing_settings settings)
{
	m_listing_settings = settings;
}

Assembler::listing_settings Assembler::getListingSettings ()
{
	return m_listing_settings;
}

//------------------------------

void Assembler::assemble ()
{
	m_program.clear ();
	m_program.push (static_cast <stack_value_t> (ASSEMBLER_VERSION));

	bool hlt_found = false;
	while (m_next_token_index < m_source_code.tokens_count)
	{
		uintptr_t addr = m_program.bytes ();

		source_code_container::token token = nextToken  (TokenType::keyword);
		ByteCode                     cmd   = toByteCode (token.value);

		source_code_container::line line = m_source_code.lines[token.line_number];
		
		if (cmd >= ByteCode::amount)
			throw assembler_error ("Unknown command '%.*s'", token.len, token.begin);

		m_program.push (cmd);

		#define ACD_(command, args, desc) case ByteCode::command:    \
		{														     \
			for (auto& argtype: args)							     \
			{													     \
				stack_value_t value = nextToken (argtype).value;     \
				if (argtype == TokenType::keyword)				     \
					m_program.push (static_cast <ByteCode> (value)); \
																     \
				else m_program.push (value);					     \
			}													     \
																     \
			break;                                                   \
		}
		switch (cmd)
		{
			COMMANDS_DEFINES_

			default:
				break;
		}
		#undef ACD_

		switch (cmd)
		{
			case ByteCode::hlt:
				hlt_found = true;
				break;

			default: 
				break;
		}

		nextToken (TokenType::newline);
		listing ("%0*zu %0*X %.*s\n", m_listing_settings.line_digits, token.line_number, m_listing_settings.addr_digits, addr, line.len, line.begin);
	}

	if (!hlt_found)
	{
		listing ("%*s %0*X [auto healt]\n", m_listing_settings.line_digits, "", m_listing_settings.addr_digits, m_program.bytes ());
		m_program.push (ByteCode::hlt);
	}

	m_program.shrink ();
}

//------------------------------

void Assembler::tokenize ()
{
	assembler_assert (m_source_code.begin);

	releaseTokens ();

	std::vector <source_code_container::line > tmp_lines_buff;
	std::vector <source_code_container::token> tmp_tokens_buff;

	static source_code_container::token newline = {};
	newline.type  = TokenType::newline;
	newline.begin = "$";
	newline.end   = newline.begin + 1;
	newline.len   = 1;
	newline.value = 0xDEFEC8ED;

	size_t current_line_number = 0, current_token_number = 0;
	for (const char* line_begin = m_source_code.begin, *line_end = Assembler_strpbrk (line_begin, LINE_DELIMITERS); line_end; line_begin = line_end+1, line_end = Assembler_strpbrk (line_begin, LINE_DELIMITERS), current_line_number++)
	{
		source_code_container::line current_line = {};
		current_line.begin  = line_begin;
		current_line.end    = line_end;
		current_line.len    = line_end - line_begin;
		current_line.number = current_line_number;

		for (const char* token_begin = line_begin, *token_end = Assembler_strpbrk_x2 (token_begin, CODE_DELIMITERS, LINE_DELIMITERS); token_end && token_begin < line_end; token_begin = token_end+1, token_end = Assembler_strpbrk_x2 (token_begin, CODE_DELIMITERS, LINE_DELIMITERS))
		{
			if (token_begin >= token_end)                                           continue;
			if (strncmp (token_begin, COMMENT_SEQUENCE, COMMENT_SEQUENCE_LEN) == 0) continue;

			source_code_container::token current_token = interptetToken (token_begin, token_end, current_token_number, current_line_number);
			tmp_tokens_buff.push_back (current_token);

			current_token_number++;
		}

		tmp_lines_buff.push_back (current_line);

		newline.number      = current_token_number;
		newline.line_number = current_line_number;
		tmp_tokens_buff.push_back (newline);

		current_token_number++;
	}

	m_source_code.lines_count = tmp_lines_buff.size ();
	m_source_code.lines = new source_code_container::line[m_source_code.lines_count] {0};
	std::memcpy (m_source_code.lines, tmp_lines_buff.data (), m_source_code.lines_count * sizeof (source_code_container::line));

	m_source_code.tokens_count = tmp_tokens_buff.size ();
	m_source_code.tokens = new source_code_container::token[m_source_code.tokens_count] {0};
	std::memcpy (m_source_code.tokens, tmp_tokens_buff.data (), m_source_code.tokens_count * sizeof (source_code_container::token));
	
	m_next_token_index = 0;
}

//------------------------------

void Assembler::releaseTokens ()
{
	if (!m_source_code.lines) return;

	delete[] (m_source_code.lines );
	delete[] (m_source_code.tokens);

	m_source_code.lines  = nullptr;
	m_source_code.tokens = nullptr;

	m_source_code.lines_count = 0;
	m_source_code.tokens      = 0;

	m_source_code.begin = nullptr;
	m_source_code.len   = 0;
}

//------------------------------

TokenType Assembler::determineTokenType (const char* begin, const char* end)
{
	return std::isdigit (*begin)? TokenType::numeric: TokenType::keyword;
}

//------------------------------

stack_value_t Assembler::interpretNumberToken (const char* str, size_t len)
{
	if (!IsNumeric (str, len))
		throw assembler_error ("Syntax error: '%.*s' is not numberic", len, str);

	return static_cast <stack_value_t> (floor (strtod (str, nullptr) * NUMBERS_MODIFIER));
}

//------------------------------

stack_value_t Assembler::interpretCommandToken (const char* str, size_t len)
{
	ByteCode cmd = TranslateToByteCode (str);
	if (cmd == ByteCode::unknown)
		throw assembler_error ("Syntax error: Unknown command '%.*s'", len, str);

	return static_cast <stack_value_t> (cmd);
}

//------------------------------

Assembler::source_code_container::token Assembler::interptetToken (const char* begin, const char* end, size_t number, size_t line_number)
{
	source_code_container::token token = {};
	token.begin       = begin;
	token.end         = end;
	token.len         = end - begin;
	token.number      = number;
	token.line_number = line_number;
	token.type        = determineTokenType (begin, end);

	switch (token.type)
	{
		case TokenType::keyword:
			token.value = interpretCommandToken (begin, token.len);
			break;

		case TokenType::numeric:
			token.value = interpretNumberToken (begin, token.len);
			break;

		default:
			assembler_assert ("Unknown token type" && false);
			break;
	}

	return token;
}

//------------------------------

const char* Assembler::StrTokenType (TokenType type)
{
	#define TRANSLATE_(type) case TokenType::##type: { return #type; break; }

	switch (type)
	{
		TRANSLATE_ (keyword);
		TRANSLATE_ (numeric);
		TRANSLATE_ (newline);

		default: break;
	}

	#undef TRANSLATE_

	return "unknown";
}

//------------------------------

double Assembler::toNumber (stack_value_t value)
{
	return static_cast <double> (value) / NUMBERS_MODIFIER;
}

//------------------------------

ByteCode Assembler::toByteCode (stack_value_t value)
{
	return static_cast <ByteCode> (value);
}

//------------------------------

Assembler::source_code_container::token& Assembler::nextToken ()
{
	if (m_next_token_index >= m_source_code.tokens_count)
		throw assembler_error ("Failed to get next token");

	source_code_container::token& token = m_source_code.tokens[m_next_token_index];
	m_next_token_index++;

	return token;
}

//------------------------------

Assembler::source_code_container::token& Assembler::nextToken (TokenType type)
{
	source_code_container::token token = nextToken ();

	if (token.type != type)
		throw assembler_error ("Expected %s, got %s", StrTokenType (type), StrTokenType (token.type));

	return token;
}

//------------------------------

void Assembler::test ()
{
	printf ("Tokens count: %zu\n", m_source_code.tokens_count);
	for (size_t i = 0; i < m_source_code.tokens_count; i++)
	{
		const source_code_container::token& token = m_source_code.tokens[i];

		printf ("Token #%zu:\n",     token.number             );
		printf ("  text: '%.*s'\n",  token.len, token.begin   );
		printf ("  len: %zu\n",      token.len                );
		printf ("  line: %zu\n",     token.line_number        );
		printf ("  type: %s\n",      StrTokenType (token.type));
		printf ("  value: 0x%08X\n", token.value              );
		printf ("\n");
	}
}

//------------------------------

void Assembler::listing (const char* format, ...)
{
	if (!m_listing_stream)
		return;

	va_list args = {};
	va_start (args, format);
	size_t len = vsnprintf (nullptr, 0, format, args);
	va_end (args);

	char* str = new char[len+1];

	va_start (args, format);
	vsprintf_s (str, len+1, format, args);
	va_end (args);

	m_listing_stream -> write (str, len);
	delete[] (str);
}

//------------------------------

assembler_error::assembler_error (const char* format, ...):
	exception_message ()
{
	va_list args = {};
	va_start (args, format);
	m_what = Format (format, args);
	va_end (args);
}

assembler_error::assembler_error (const std::string& string):
	exception_message (string)
{}

//------------------------------

const char* Assembler_strpbrk (const char* begin, const char* delimiters)
{
	for (const char* cur = begin; *cur; cur++)
		if (strchr (delimiters, *cur)) return cur;

	return nullptr;
}

//------------------------------

const char* Assembler_strpbrk_x2 (const char* begin, const char* delimiters1, const char* delimiters2)
{
	for (const char* cur = begin; *cur; cur++)
		if (strchr (delimiters1, *cur) || 
			strchr (delimiters2, *cur)) return cur;

	return nullptr;
}

//------------------------------

const char* Assembler_strnpbrk (const char* begin, const char* delimiters, size_t limit)
{
	for (size_t i = 0; begin[i] && i < limit; i++)
		if (strchr (delimiters, begin[i])) return begin + i;

	return nullptr;
}

//------------------------------