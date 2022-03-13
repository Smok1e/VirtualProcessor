#pragma once

//------------------------------

#include <cstdarg>
#include <cstring>
#include <string>

//------------------------------

#define TXT232UINT(text) *reinterpret_cast <const unsigned __int32*> (text);
#define sizearr(...) (_DoNotUseWithPointers_SizeArr (__VA_ARGS__)	 )
#define sizestr(...) (_DoNotUseWithPointers_SizeArr (__VA_ARGS__) - 1)

#define PRINTF_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY(byte) \
  (byte & 0b10000000 ? '1' : '0'),        \
  (byte & 0b01000000 ? '1' : '0'),        \
  (byte & 0b00100000 ? '1' : '0'),        \
  (byte & 0b00010000 ? '1' : '0'),        \
  (byte & 0b00001000 ? '1' : '0'),        \
  (byte & 0b00000100 ? '1' : '0'),        \
  (byte & 0b00000010 ? '1' : '0'),        \
  (byte & 0b00000001 ? '1' : '0') 

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
inline constexpr size_t _DoNotUseWithPointers_SizeArr (T (& /*arr*/) [N])
{
	return N;
}

//------------------------------

template <typename value_t>
inline constexpr bool IsOnly1BitSet (value_t value)
{
	return (value & (value - 1)) == 0 && value != 0;
}

//------------------------------