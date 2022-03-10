#pragma once

//------------------------------

template <typename value_t>
void ProgramContainer::push (value_t value)
{
	extend (reinterpret_cast <byte_t*> (&value), sizeof (value_t));
}

template <typename value_t>
value_t ProgramContainer::pop ()
{
	#ifdef _DEBUG
		if (empty ()                   ) throw std::runtime_error ("Container is empty");
		if (bytes () < sizeof (value_t)) throw std::runtime_error ("Wrong data size"   );
	#endif

	value_t value = {};

	remove (reinterpret_cast <byte_t*> (&value), sizeof (value));
	return value;
}

//------------------------------