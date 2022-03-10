#pragma once

//------------------------------

template <typename value_t>
void ProgramContainer::append (value_t value) 
{
	extend (reinterpret_cast <byte_t*> (&value), sizeof (value_t));
}

//------------------------------

template <typename value_t>
value_t ProgramContainer::get (size_t index_in_bytes) const
{
	#ifdef _DEBUG
		if (index_in_bytes >= bytes ()) throw std::runtime_error ("Wrong index");
	#endif

	value_t value = {};
	std::memcpy (&value, data () + index_in_bytes, sizeof (value_t));

	return value;
}

//------------------------------