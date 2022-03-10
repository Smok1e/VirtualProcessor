#pragma once

//------------------------------

#include <stdexcept>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <string>

//------------------------------

class exception_message: public std::exception
{
public:
	         exception_message ();
	explicit exception_message (const char* format, va_list args);
	explicit exception_message (const char* format, ...);
	explicit exception_message (const std::string& msg);
	         exception_message (const exception_message& copy);

	virtual const char* what () const;

	virtual exception_message& operator= (const char* msg);
	virtual exception_message& operator= (const std::string& msg);
	virtual exception_message& operator= (const exception_message& that);

protected:
	std::string m_what;

};

//------------------------------
