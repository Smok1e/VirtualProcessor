#pragma once

//------------------------------

#include <stdexcept>

//------------------------------

enum class StackError
{
	NoError,
	Unknown,
	CapacityError,
	LeftBoundBroken,
	RightBoundBroken,
	WrongHash,
	ValuePositionViolation,
	Destructed,
	LeftDataBoundBroken,
	RightDataBoundBroken,
	WrongLength
};

//------------------------------

class stack_check_error: public std::exception
{
public:
	stack_check_error ();
	stack_check_error (StackError err);
	stack_check_error (StackError err, const char* func, int line);
	stack_check_error (const stack_check_error& copy);

	StackError  code () const;
	const char* what () const;
	const char* func () const;
	int         line () const;

	stack_check_error& operator= (const stack_check_error& that);

protected:
	StackError  m_err_code;
	const char* m_func;
	int         m_line;

};

//------------------------------

const char* StrStackError (StackError err);

//------------------------------