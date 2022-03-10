#pragma once

//------------------------------

#include <vector>

#include "ByteCode.h"

//------------------------------

class ProgramContainer
{
public:
	ProgramContainer ();
	ProgramContainer (const ProgramContainer& copy);

	void save (std::ostream& stream);
	void load (std::istream& stream);

	template <typename value_t> void    push (value_t value);
	template <typename value_t> value_t pop  ();

	ByteCode      popByteCode   ();
	stack_value_t popStackValue ();

	bool   empty () const;
	size_t count () const;
	size_t bytes () const;

	void clear  ();
	void shrink ();

	      byte_t* begin ();
	const byte_t* begin () const;

	      byte_t* end   ();
	const byte_t* end   () const;

	      byte_t* data  ();
	const byte_t* data  () const;

private:
	std::vector <byte_t> m_data;
	size_t               m_count;

	void extend (const byte_t* src, size_t count);
	void remove (      byte_t* dst, size_t count);

};

//------------------------------

#include "ProgramContainer.hpp"

//------------------------------