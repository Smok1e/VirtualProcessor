#include "StackCheckError.h"

//------------------------------

stack_check_error::stack_check_error ():
	m_err_code (StackError::Unknown),
	m_func     ("Unknown function"),
	m_line     (0)
{}

stack_check_error::stack_check_error (StackError err):
	m_err_code (err),
	m_func     ("Unknown function"),
	m_line     (0)
{}

stack_check_error::stack_check_error (StackError err, const char* func, int line):
	m_err_code (err),
	m_func     (func),
	m_line     (line)
{}

stack_check_error::stack_check_error (const stack_check_error& copy):
	m_err_code (copy.m_err_code),
	m_func     (copy.m_func),
	m_line     (copy.m_line)
{}

//------------------------------

StackError stack_check_error::code () const
{
	return m_err_code;
}

const char* stack_check_error::what () const
{
	return StrStackError (m_err_code);
}

const char* stack_check_error::func () const
{
	return m_func;
}

int stack_check_error::line () const
{
	return m_line;
}

//------------------------------

stack_check_error& stack_check_error::operator= (const stack_check_error& that)
{
	if (this == &that)
		return *this;

	m_err_code = that.m_err_code;
	m_func     = that.m_func;
	m_line     = that.m_line;

	return *this;
}

//------------------------------

const char* StrStackError (StackError err)
{
	#define STR_ERROR_(err, desc) case err: { return desc; break; }

	switch (err)
	{
		STR_ERROR_ (StackError::NoError,                "No error"                    );
		STR_ERROR_ (StackError::Unknown,                "Unknown error"               );
		STR_ERROR_ (StackError::CapacityError,          "Capacity error"              );
		STR_ERROR_ (StackError::LeftBoundBroken,        "Left struct bound broken"    );
		STR_ERROR_ (StackError::RightBoundBroken,       "Right struct bound broken"   );
		STR_ERROR_ (StackError::WrongHash,              "Wrong struct hash"           );
		STR_ERROR_ (StackError::ValuePositionViolation, "Wrong value"                 );
		STR_ERROR_ (StackError::Destructed,             "Stack used after destruction");
		STR_ERROR_ (StackError::LeftDataBoundBroken,    "Left data bound broken"      );
		STR_ERROR_ (StackError::RightDataBoundBroken,   "Right data bound broken"     );
		STR_ERROR_ (StackError::WrongLength,            "Length error"                );

		default: return "Unknown error";
	}

	#undef STR_ERROR_
}

//------------------------------