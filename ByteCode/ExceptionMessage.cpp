#include "ExceptionMessage.h"
#include "Utils.hpp"

//------------------------------

exception_message::exception_message ():
	m_what ("Unknown error")
{}

exception_message::exception_message (const char* format, va_list args):
	m_what (Format (format, args))
{}

exception_message::exception_message (const char* format, ...):
	m_what ("")
{
	va_list args = {};
	va_start (args, format);
	m_what = Format (format, args);
	va_end (args);
}

exception_message::exception_message (const std::string& msg):
	m_what (msg)
{}
 
exception_message::exception_message (const exception_message& copy):
	m_what (copy.m_what)
{}

//------------------------------

const char* exception_message::what () const
{
	return m_what.c_str ();
}

//------------------------------

exception_message& exception_message::operator= (const char* msg)
{
	m_what = msg;
	return *this;
}

exception_message& exception_message::operator= (const std::string& msg)
{
	m_what = msg;
	return *this;
}

exception_message& exception_message::operator= (const exception_message& that)
{
	if (this == &that)
		return *this;

	m_what = that.m_what;
	return *this;
}

//------------------------------