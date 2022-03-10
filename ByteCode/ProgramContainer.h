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

	void save (std::ostream& stream) const;
	void save (const char* filename) const;

	void load (std::istream& stream);
	void load (const char* filename);

	template <typename value_t> 
	void append (value_t value);

	bool   empty () const;
	size_t bytes () const;

	void clear  ();
	void shrink ();

	template <typename value_t>
	value_t get (size_t index_in_bytes) const;

	      byte_t* begin ();
	const byte_t* begin () const;

	      byte_t* end   ();
	const byte_t* end   () const;

	      byte_t* data  ();
	const byte_t* data  () const;

private:
	std::vector <byte_t> m_data;

	void extend (const byte_t* src, size_t count);
	void remove (      byte_t* dst, size_t count);

};

//------------------------------

#include "ProgramContainer.hpp"

//------------------------------