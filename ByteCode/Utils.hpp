#pragma once

//------------------------------

#include <cstdarg>
#include <cstring>
#include <string>

//------------------------------

#define TXT232UINT(text) *reinterpret_cast <const unsigned __int32*> (text);
#define sizearr(...) (_DoNotUseWithPointers_SizeArr (__VA_ARGS__)	 )
#define sizestr(...) (_DoNotUseWithPointers_SizeArr (__VA_ARGS__) - 1)

//------------------------------

bool        IsDoubleDigit (char character);
bool        IsNumeric     (const char* str                      );
bool        IsNumeric     (const char* str, size_t max          );
bool        IsNumeric     (const char* str, const char* end     );
bool        IsNumeric     (const std::string& string            );
bool        IsNumeric     (const std::string& string, size_t max);
std::string Format        (const char* format, ...         );
std::string Format        (const char* format, va_list args);

//------------------------------

template <typename T, size_t N>
constexpr size_t _DoNotUseWithPointers_SizeArr (T (& /*arr*/) [N])
{
	return N;
}

//------------------------------