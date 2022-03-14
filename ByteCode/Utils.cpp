#include "Utils.hpp"

//------------------------------

bool IsDoubleDigit (char character)
{
	return (character >= '0' && character <= '9') || character == '.' || character == '-';
}

bool IsNumeric (const char* str)
{
	for (const char* chr = str; *chr; chr++)
		if (!IsDoubleDigit (*chr)) return false;

	return true;
}

bool IsNumeric (const char* str, size_t max)
{
	for (size_t i = 0; i < max && str[i]; i++)
		if (!IsDoubleDigit (str[i])) return false;

	return true;
}

bool IsNumeric (const char* str, const char* end)
{
	for (const char* begin = str; *begin && begin < end; begin++)
		if (!IsDoubleDigit (*begin)) return false;

	return true;
}

bool IsNumeric (const std::string& string)
{
	for (char chr: string)
		if (!IsDoubleDigit (chr)) return false;

	return true;
}

bool IsNumeric (const std::string& string, size_t max)
{
	size_t len = string.length ();
	for (size_t i = 0; i < len && i < max; i++)
		if (!IsDoubleDigit (string[i])) return false;

	return true;
}

//------------------------------

std::string Format (const char* format, ...)
{
	va_list args = {};
	va_start (args, format);
	size_t buffsize = vsnprintf (nullptr, 0, format, args) + 1;
	va_end (args);

	char* str = new char[buffsize];

	va_start (args, format);
	vsprintf_s (str, buffsize, format, args);
	va_end (args);

	std::string string = str;
	delete[] (str);

	return string;
}

//------------------------

std::string Format (const char* format, va_list args)
{
	size_t buffsize = vsnprintf (nullptr, 0, format, args) + 1;
	
	char* str = new char[buffsize];
	vsprintf_s (str, buffsize, format, args);

	std::string string = str;
	delete[] (str);

	return string;
}

//------------------------------