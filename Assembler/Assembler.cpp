#include <cstring>
#include <stdexcept>
#include <string>
#include <chrono>

#include "Assembler.h"
#include "Utils.hpp"
#include "ByteCode.h"

#pragma warning (disable: 4018)

//------------------------------

Assembler::listing_settings DEFAULT_LISTINGS_SETTINGS = 
{
	3,    // line digits
	4,    // address digits
	6,    // max content bytes per line
	false // print content binary if true, else hexademically
};

//------------------------------

Assembler::Assembler ():
	m_program          (),
	m_program_index    (0),
	m_source_code      ({0}),
	m_next_token_index (0),
	m_listing_stream   (nullptr),
	m_listing_settings (DEFAULT_LISTINGS_SETTINGS),
	m_name_table       {},
	m_empty_jumps      (false),
	m_pass_number      (0),
	m_numbers_modifier (NUMBERS_MODIFIER)
{}

//------------------------------

Assembler::~Assembler ()
{
	releaseTokens ();
}

//------------------------------

void Assembler::setSourceCode (const char* source)
{
	releaseTokens ();

	m_source_code.begin    = source;
	m_source_code.len      = strlen (source);
	m_source_code.filename = "Loaded from string";

	tokenize ();
}

const char* Assembler::getSourceCode ()
{
	return m_source_code.begin;
}

//------------------------------

void Assembler::loadSourceCode (const char* filename)
{
	releaseTokens ();

	FILE* file = nullptr;
	errno_t err = fopen_s (&file, filename, "rb");

	if (!file || err)
	{
		static char errbuff[ASSEMBLER_BUFFSIZE] = "";
		strerror_s (errbuff, err);

		throw assembler_error (errbuff);
	}

	fseek (file, 0, SEEK_END);
	size_t len = ftell (file);
	
	m_source_code.loaded_source = new char[len+2];

	fseek (file, 0, SEEK_SET);
	fread (m_source_code.loaded_source, 1, len, file);
	m_source_code.loaded_source[len  ] = '\n';
	m_source_code.loaded_source[len+1] = '\0';

	fclose (file);

	m_source_code.filename = filename;
	m_source_code.begin    = m_source_code.loaded_source;
	m_source_code.len      = len+1;

	tokenize ();
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

void Assembler::compile ()
{
	auto begin = std::chrono::steady_clock::now ();

	m_pass_number = 0;
	m_name_table.clear ();
	m_program.clear ();
	while (assemble ()) 
		m_pass_number++;

	auto end  = std::chrono::steady_clock::now ();
	auto msec = std::chrono::duration_cast <std::chrono::milliseconds> (end - begin);

	listing ("\n==========================\n\n");
	listing ("Compilation done in %zu passes, %d msec.\n", m_pass_number+1, msec.count ());
	listing ("Generated %zu bytes of code\n", m_program.bytes ());
}

//------------------------------

bool Assembler::assemble ()
{
	if (!m_source_code.tokens)
		throw assembler_error ("No source to complie");

	m_empty_jumps      = false;
	m_program_index    = 0;
	m_next_token_index = 0;

	listing ("\nAssembling pass #%zu:\n", m_pass_number+1);

	m_program.set (m_program_index, static_cast <stack_value_t> (ASSEMBLER_VERSION));
	m_program_index += sizeof (stack_value_t);

	listing_line (-1, 0, m_program.begin (), sizeof (stack_value_t), "[version]");

	size_t beg = m_program_index;
	m_program.set (m_program_index, static_cast <stack_value_t> (m_numbers_modifier));
	m_program_index += sizeof (stack_value_t);
	listing_line (-1, beg, m_program.begin () + beg, sizeof (stack_value_t), "[precision]");

	while (m_next_token_index < m_source_code.tokens_count)
	{
		size_t prev_addr = m_program_index;

		source_code_container::token token = nextToken (TokenType::Keyword | TokenType::Newline | TokenType::Label);
		source_code_container::line  line  = m_source_code.lines[token.line_number];

		if (token.type == TokenType::Keyword)
		{
			ByteCode cmd = toByteCode (token.value);

			if (cmd >= ByteCode::amount)
				throw assembler_error ("Unknown command '%.*s'", token.len, token.begin);

			m_program.set (m_program_index, cmd);
			m_program_index += sizeof (cmd);

			#define ACD_(command, args, desc, ...) case ByteCode::##command: { compileInstruction (args); break; };
			switch (cmd)
			{
				COMMANDS_DEFINES_

				default:
					break;
			}
			#undef ACD_

			nextToken (TokenType::Newline);
		}

		else if (token.type == TokenType::Label)
		{
			addLabel (token.begin, token.end, m_program_index);
			nextToken (TokenType::Newline);
		}

		listing_line (line.number, prev_addr, m_program.begin () + prev_addr, m_program_index - prev_addr, line.begin, line.len);
	}

	size_t prev_addr = m_program.bytes ();
	m_program.set (m_program_index, ByteCode::hlt);
	m_program_index += sizeof (ByteCode::hlt);

	listing_line (-1, prev_addr, m_program.begin () + prev_addr, m_program.bytes () - prev_addr, "[haltion - end of code]");

	m_program.shrink ();

	return m_empty_jumps;
}

//------------------------------

void Assembler::tokenize ()
{
	assembler_assert (m_source_code.begin);

	releaseTokens ();

	std::vector <source_code_container::line > tmp_lines_buff;
	std::vector <source_code_container::token> tmp_tokens_buff;

	static source_code_container::token Newline = {};
	Newline.type  = TokenType::Newline;
	Newline.begin = "$";
	Newline.end   = Newline.begin + 1;
	Newline.len   = 1;
	Newline.value = 0xDEADFA11;

	size_t current_line_number = 0, current_token_number = 0;
	for (const char* line_begin = m_source_code.begin, *line_end = Assembler_strpbrk (line_begin, LINE_DELIMITERS); line_end; line_begin = line_end+1, line_end = Assembler_strpbrk (line_begin, LINE_DELIMITERS), current_line_number++)
	{
		source_code_container::line current_line = {};
		current_line.begin  = line_begin;
		current_line.end    = line_end;
		current_line.len    = line_end - line_begin;
		current_line.number = current_line_number;

		bool skip = false;
		for (const char* token_begin = line_begin, *token_end = Assembler_strpbrk_x2 (token_begin, CODE_DELIMITERS, LINE_DELIMITERS); token_end && token_begin < line_end; token_begin = token_end+1, token_end = Assembler_strpbrk_x2 (token_begin, CODE_DELIMITERS, LINE_DELIMITERS))
		{
			if (token_begin >= token_end)                                           continue;
			if (strncmp (token_begin, COMMENT_SEQUENCE, COMMENT_SEQUENCE_LEN) == 0) break;

			source_code_container::token current_token = interptetToken (token_begin, token_end, current_token_number, current_line_number);
			tmp_tokens_buff.push_back (current_token);

			current_token_number++;
		}

		tmp_lines_buff.push_back (current_line);

		Newline.number      = current_token_number;
		Newline.line_number = current_line_number;
		tmp_tokens_buff.push_back (Newline);

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

	if (m_source_code.loaded_source)
		delete[] (m_source_code.loaded_source);

	delete[] (m_source_code.lines );
	delete[] (m_source_code.tokens);

	m_source_code.lines  = nullptr;
	m_source_code.tokens = nullptr;

	m_source_code.lines_count = 0;
	m_source_code.tokens      = 0;

	m_source_code.begin = nullptr;
	m_source_code.len   = 0;

	m_source_code.loaded_source = nullptr;
}

//------------------------------

TokenType Assembler::determineTokenType (const char* begin, const char* end)
{
	if (IsDoubleDigit (*begin) || *begin == '\'')
		return TokenType::Numeric;

	if (strchr (LINE_DELIMITERS, *begin))
		return TokenType::Newline;

	if (strncmp (begin, REGISTER_SEQUENCE, REGISTER_SEQUENCE_LEN) == 0)
		return TokenType::Register;

	if (strncmp (begin, ADDRESS_SEQUENCE, ADDRESS_SEQUENCE_LEN) == 0)
		return TokenType::Address;

	if (*(end-1) == ':')
		return TokenType::Label;

	if (*begin == ':')
		return TokenType::LabelRef;

	return TokenType::Keyword;
}

//------------------------------

stack_value_t Assembler::interpretNumberToken (const char* str, size_t len)
{
	if (len > 2 && strncmp (str, "0x", 2) == 0)
		return static_cast <stack_value_t> (strtol (str, nullptr, 16) * m_numbers_modifier);

	if (len == 3 && *str == '\'' && str[2] == '\'')
		return static_cast <stack_value_t> (str[1] * m_numbers_modifier);

	if (!IsNumeric (str, len))
		throw assembler_error ("Syntax error: '%.*s' is not numeric", len, str);

	return static_cast <stack_value_t> (floor (strtod (str, nullptr) * m_numbers_modifier));
}

//------------------------------

stack_value_t Assembler::interpretCommandToken (const char* str, size_t len)
{
	ByteCode cmd = TranslateToByteCode (str, len);
	if (cmd == ByteCode::unknown)
		throw assembler_error ("Syntax error: Unknown command '%.*s'", len, str);

	return static_cast <stack_value_t> (cmd);
}

//------------------------------

stack_value_t Assembler::interpretRegisterToken (const char* str, size_t len)
{
	if (len != (2 + REGISTER_SEQUENCE_LEN))
		throw assembler_error ("Syntax error: Invalid register '%.*s' len - %zu", len, str, len);

	const char* register_begin_sequence = str;
	const char* register_identifier     = str + REGISTER_SEQUENCE_LEN;
	const char* register_end_character  = str + REGISTER_SEQUENCE_LEN + 1;

	if (strncmp (register_begin_sequence, REGISTER_SEQUENCE, REGISTER_SEQUENCE_LEN) != 0) 
		throw assembler_error ("Syntax error: Invalid register '%.*s' begin sequence - '%.*s'", len, str, register_begin_sequence, REGISTER_SEQUENCE_LEN);

	if (*register_end_character != 'x')
		throw assembler_error ("Syntax error: Invalid register '%.*s' end character - %c", len, str, *register_end_character);

	int index = RegisterIndex (register_identifier, len - REGISTER_SEQUENCE_LEN);
	if (index == -1)
		throw assembler_error ("Syntax error: Invalid register '%.*s'", len, str);

	return static_cast <stack_value_t> (index);
}

//------------------------------

stack_value_t Assembler::interpretAddressToken (const char* str, size_t len)
{
	return static_cast <stack_value_t> (0xDEADADD3);
}

//------------------------------

stack_value_t Assembler::interpretLabelToken (const char* str, size_t len)
{
	return static_cast <stack_value_t> (0xB16B00B5);
}

//------------------------------

stack_value_t Assembler::interpretLabelRefToken (const char* str, size_t len)
{
	return static_cast <stack_value_t> (0xFEEDFACE);
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
		case TokenType::Keyword:  token.value = interpretCommandToken  (begin, token.len); break;
		case TokenType::Numeric:  token.value = interpretNumberToken   (begin, token.len); break;
		case TokenType::Register: token.value = interpretRegisterToken (begin, token.len); break;	
		case TokenType::Address:  token.value = interpretAddressToken  (begin, token.len); break;
		case TokenType::Label:    token.value = interpretLabelToken    (begin, token.len); break;
		case TokenType::LabelRef: token.value = interpretLabelRefToken (begin, token.len); break;

		default: assembler_assert ("Unknown token type" && false); break;
	}

	return token;
}

//------------------------------

void Assembler::addLabel (const char* name_begin, const char* name_end, size_t addr)
{
	name_end--; // "label:" -> "label"
	assembler_assert (name_begin < name_end);

	size_t len = name_end - name_begin;
	auto iter = std::find_if 
	(
		m_name_table.begin (), 
		m_name_table.end   (), 

		[&name_begin, &len](const name_pair& elem)
		{ 
			return strncmp (elem.name_begin, name_begin, len) == 0; 
		}
	);

	if (iter != m_name_table.end ())
		return;

	name_pair pair = {};
	pair.name_begin = name_begin;
	pair.name_end   = name_end;
	pair.name_len   = len;

	pair.address = addr;

	m_name_table.push_back (pair);
}

//------------------------------

size_t Assembler::findLabel (const char* name_begin, const char* name_end)
{
	name_begin++; // ":label" -> "label"
	assembler_assert (name_begin < name_end);

	size_t len = name_end - name_begin;
	auto iter = std::find_if 
	(
		m_name_table.begin (), 
		m_name_table.end   (), 

		[&name_begin, &len](const name_pair& elem)
		{ 
			return strncmp (elem.name_begin, name_begin, len) == 0; 
		}
	);

	if (iter == m_name_table.end ())
		return ASSEMBLER_INVALID_ADDRESS;

	return iter -> address;
}

//------------------------------

void Assembler::compileInstruction (const std::initializer_list <TokenType>& args)
{
	byte_type type = {};
	if (followingToken ().type == TokenType::Address)
	{
		nextToken ();
		type.is_address = true;
	}

	if (args.size () > 0) type.arg1_type = followingToken (*(args.begin () + 0), 0).type;
	if (args.size () > 1) type.arg2_type = followingToken (*(args.begin () + 1), 1).type;

	m_program.set (m_program_index, type);
	m_program_index += sizeof (type);

	for (const auto& argtype: args)
	{
		if (followingToken ().type == TokenType::Newline) 
			break;

		source_code_container::token token = nextToken (argtype);
		switch (token.type)
		{
			case TokenType::Keyword:
				m_program.set (m_program_index, static_cast <ByteCode> (token.value));
				m_program_index += sizeof (ByteCode);

				break;

			case TokenType::Numeric:
				m_program.set (m_program_index, token.value);
				m_program_index += sizeof (token.value);

				break;

			case TokenType::Register:
				m_program.set (m_program_index, static_cast <byte_t> (token.value));
				m_program_index += sizeof (byte_t);

				break;

			case TokenType::LabelRef:
				{
					size_t addr = findLabel (token.begin, token.end);
					if (addr == ASSEMBLER_INVALID_ADDRESS)
					{
						if (m_pass_number > 0)
							throw assembler_error ("Can't resolve label address after a first pass");

						m_empty_jumps = true;
					}

					m_program.set (m_program_index, static_cast <stack_value_t> (addr));
					m_program_index += sizeof (stack_value_t);
				}

				break;

			default:
				break;
		}
	}
}

//------------------------------

const char* Assembler::StrTokenType (TokenType type)
{
	#define TRANSLATE_(type) case TokenType::##type: { return #type; break; }

	switch (type)
	{
		TRANSLATE_ (Keyword );
		TRANSLATE_ (Numeric );
		TRANSLATE_ (Newline );
		TRANSLATE_ (Register);
		TRANSLATE_ (Address );
		TRANSLATE_ (Label   );
		TRANSLATE_ (LabelRef);

		default: break;
	}

	#undef TRANSLATE_

	return "unknown";
}

//------------------------------

double Assembler::toNumber (stack_value_t value)
{
	return static_cast <double> (value) / m_numbers_modifier;
}

//------------------------------

ByteCode Assembler::toByteCode (stack_value_t value)
{
	return static_cast <ByteCode> (value);
}

//------------------------------

Assembler::source_code_container::token Assembler::followingToken (size_t offset /*= 0*/)
{
	static source_code_container::token none_token = {};
	none_token.begin       = "None";
	none_token.end         = none_token.begin + strlen (none_token.begin);
	none_token.len         = none_token.end - none_token.begin;
	none_token.line_number = 0;
	none_token.number      = 0;
	none_token.type        = TokenType::None;
	none_token.value       = 0x8BADC0DE;

	if (m_next_token_index + offset >= m_source_code.tokens_count)
		return none_token;

	return m_source_code.tokens[m_next_token_index + offset];
}

Assembler::source_code_container::token Assembler::followingToken (TokenType type, size_t offset /*= 0*/)
{
	source_code_container::token token = followingToken (offset);
	if (!static_cast <bool> (token.type & type))
		throw assembler_error ("Expected %s, got %s", StrTokenType (type), StrTokenType (token.type));

	return token;
}

//------------------------------

Assembler::source_code_container::token Assembler::nextToken ()
{
	source_code_container::token token = followingToken ();
	m_next_token_index++;

	return token;
}

//------------------------------

Assembler::source_code_container::token Assembler::nextToken (TokenType type)
{
	source_code_container::token token = followingToken (type);
	m_next_token_index++;

	return token;
}

//------------------------------

void Assembler::dumpTokens ()
{
	printf ("Tokens count: %zu\n", m_source_code.tokens_count);
	for (size_t i = 0; i < m_source_code.tokens_count; i++)
	{
		const source_code_container::token& token = m_source_code.tokens[i];

		printf 
		(
			"#%03zu: text: %-10.*s | len: %-2zu | line: %-2zu | type: %-10s | value: 0x%08X\n", 
			token.number, 
			token.len, 
			token.begin, 
			token.len, 
			token.line_number, 
			StrTokenType (token.type), 
			token.value
		);
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

void Assembler::listing_line (int line, uintptr_t addr, byte_t* content_begin, size_t content_size, const char* message, int len /*= -1*/)
{
	// Listing format:
	// [line] [absolute address] [bytes count] [data bytes] [source line]

	if (len < 0) len = strlen (message);
	
	if (line >= 0) 
		listing ("%0*zu ", m_listing_settings.line_digits, line+1);

	else
	{
		for (size_t i = 0; i < m_listing_settings.line_digits; i++)
			m_listing_stream -> put ('-');

		m_listing_stream -> put (' ');
	}

	listing ("0x%0*X ", m_listing_settings.addr_digits, addr);
	
	#define PRINT_CONT_BYTE_(index)																					 \
	{																												 \
		unsigned char byte = *static_cast <unsigned char*> (content_begin + index);									 \
		if (m_listing_settings.cont_print_binary) listing (PRINTF_BINARY_PATTERN " ", PRINTF_BYTE_TO_BINARY (byte)); \
		else                                      listing ("%02X ",                                          byte ); \
	}

	for (size_t i = 0; i < content_size && i < m_listing_settings.cont_bytes; i++)
		PRINT_CONT_BYTE_ (i);

	if (content_size < m_listing_settings.cont_bytes)
		for (size_t i = 0; i < m_listing_settings.cont_bytes - content_size; i++)
			listing ("%*s", (m_listing_settings.cont_print_binary? 8: 2) + 1, "");

	size_t bytes_remaining = (content_size > m_listing_settings.cont_bytes)? content_size - m_listing_settings.cont_bytes: 0;

	listing ("%.*s", len, message);

	for (size_t i = m_listing_settings.cont_bytes, l = m_listing_settings.cont_bytes; i < m_listing_settings.cont_bytes + bytes_remaining; i++, l++)
	{
		if (l >= m_listing_settings.cont_bytes)
			l = 0, listing ("\n%*s %*s ", m_listing_settings.line_digits + 1, "", m_listing_settings.addr_digits + 1, "");

		PRINT_CONT_BYTE_ (i);
	}

	listing ("\n");

	#undef PRINT_CONT_BYTE_
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