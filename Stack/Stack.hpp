#pragma once

//------------------------------

#include <typeinfo>
#include <algorithm>

#include "Stack.h"

//------------------------------

template <typename data_t> const typename Stack <data_t>::canary_t Stack <data_t>::CanaryValue = static_cast <typename Stack <data_t>::canary_t> (0xDEFEC8ED);
template <typename data_t> const char                              Stack <data_t>::FreeValue   = static_cast <char                             > (0xFD      ); // means free data

//------------------------------

template <typename data_t>
Stack <data_t>::Stack ():
	m_lft_canary (CanaryValue),
	m_hash       (0),
	m_data       (nullptr),
	m_size       (0),
	m_capacity   (0),
	m_destructed (false),
	m_rgt_canary (CanaryValue)
{
	m_data = new char[2 * sizeof (canary_t)];

	*reinterpret_cast <canary_t*> (m_data                    ) = CanaryValue;
	*reinterpret_cast <canary_t*> (m_data + sizeof (canary_t)) = CanaryValue;

	updateHash ();
	
	resize (0);
	CHECK_STACK_;
}

template <typename data_t>
Stack <data_t>::Stack (size_t size):
	m_lft_canary (CanaryValue),
	m_hash       (0),
	m_data       (nullptr),
	m_size       (0),
	m_capacity   (0),
	m_destructed (false),
	m_rgt_canary (CanaryValue)
{
	m_data = new char[2 * sizeof (canary_t)];

	*reinterpret_cast <canary_t*> (m_data                    ) = CanaryValue;
	*reinterpret_cast <canary_t*> (m_data + sizeof (canary_t)) = CanaryValue;

	updateHash ();
	
	resize (size);
	CHECK_STACK_;
}

template <typename data_t>
Stack <data_t>::~Stack ()
{
	delete[] (m_data);
	m_data = nullptr;

	m_destructed = true;
}

//------------------------------

template <typename data_t>
void Stack <data_t>::resize (size_t new_size)
{
	CHECK_STACK_;

	if (m_capacity == new_size)
		return;

	char* tmp_data = new char[new_size * sizeof (data_t) + 2 * sizeof (canary_t)] {0};

	*reinterpret_cast <canary_t*> (tmp_data                                                 ) = CanaryValue;
	*reinterpret_cast <canary_t*> (tmp_data + new_size * sizeof (data_t) + sizeof (canary_t)) = CanaryValue;

	for (size_t i = 0; i < new_size; i++)
		if (i < m_capacity) *getData (tmp_data, i) = *getData (m_data, i);
		else free (i, tmp_data);

	std::swap (m_data,     tmp_data);
	std::swap (m_capacity, new_size);

	delete[] (tmp_data);

	if (m_size > m_capacity) m_size = m_capacity;

	updateHash ();
	CHECK_STACK_;
}

//------------------------------

template <typename data_t>
void Stack <data_t>::push (const data_t& value)
{
	CHECK_STACK_;

	if (m_capacity <= m_size) resize ((m_capacity + 1) * 2);
	
	set (m_size, value);
	m_size++;

	updateHash ();
	CHECK_STACK_;
}

//------------------------------

template <typename data_t>
data_t Stack <data_t>::pop ()
{
	CHECK_STACK_;

	data_t value = top ();
	
	m_size--;
	free (m_size);

	updateHash ();

	CHECK_STACK_;
	return value;
}

//------------------------------

template <typename data_t>
data_t Stack <data_t>::top () const
{
	return get (m_size-1);
}

//------------------------------

template <typename data_t>
void Stack <data_t>::clear ()
{
	CHECK_STACK_;

	for (size_t i = 0; i < m_size; i++)
		free (i);

	m_size = 0;
	updateHash ();
	CHECK_STACK_;
}

//------------------------------

template <typename data_t>
void Stack <data_t>::reverse ()
{
	CHECK_STACK_;

	for (size_t i = 0; i < m_size/2; i++)
		MemorySwap (getData (nullptr, i), getData (nullptr, m_size-i-1), sizeof (data_t));

	CHECK_STACK_;
}

//------------------------------

template <typename data_t>
size_t Stack <data_t>::size () const
{
	return m_size;
}

template <typename data_t>
size_t Stack <data_t>::capacity () const
{
	return m_capacity;
}

template <typename data_t>
size_t Stack <data_t>::bytes () const
{
	return m_capacity * sizeof (data_t) + 2 * sizeof (canary_t);
}

template <typename data_t>
bool Stack <data_t>::empty () const
{
	return m_size < 1;
}

//------------------------------

template <typename data_t>
data_t* Stack <data_t>::begin ()
{
	return getData ();
}

template <typename data_t>
const data_t* Stack <data_t>::begin () const
{
	return const_cast <const data_t*> (getData ());
}

template <typename data_t>
data_t* Stack <data_t>::end ()
{	
	return getData (m_data, m_size);
}

template <typename data_t>
const data_t* Stack <data_t>::end () const
{	
	return const_cast <const data_t*> (getData (m_data, m_size));
}

//------------------------------

template <typename data_t>
void Stack <data_t>::set (size_t index, const data_t& value)
{
	*reinterpret_cast <data_t*> (m_data + index * sizeof (data_t) + sizeof (canary_t)) = value;
}

template <typename data_t>
data_t Stack <data_t>::get (size_t index) const
{
	return *reinterpret_cast <data_t*> (m_data + index * sizeof (data_t) + sizeof (canary_t));
}

//------------------------------

template <typename data_t>
data_t* Stack <data_t>::getData (char* data /*= nullptr*/, size_t index /*= 0*/)
{
	if (!data) data = m_data;
	return reinterpret_cast <data_t*> (data + sizeof (canary_t) + index * sizeof (data_t));
}

//------------------------------

template <typename data_t>
void Stack <data_t>::free (size_t index, char* data /*= nullptr*/)
{
	memset (getData (data, index), FreeValue, sizeof (data_t));
}

//------------------------------

template <typename data_t>
bool Stack <data_t>::isFree (size_t index, char* data /*= nullptr*/) const
{
	char* begin = reinterpret_cast <char*> (data, index);
	for (size_t i = 0; i < sizeof (data_t); i++)
		if (begin[i] != FreeValue) return false;

	return true;
}

//------------------------------

template <typename data_t>
void Stack <data_t>::updateHash ()
{
	const char* data = reinterpret_cast <const char*> (this);
	
	m_hash = 0;
	hash_t hash = 0;
	for (size_t i = 0; i < sizeof (*this); i++)
		hash += data[i];

	m_hash = hash;
}

//------------------------------

template <typename data_t>
StackError Stack <data_t>::ok ()
{
	if (m_size > m_capacity)         return StackError::CapacityError;
	if (m_lft_canary != CanaryValue) return StackError::LeftBoundBroken;
	if (m_rgt_canary != CanaryValue) return StackError::RightBoundBroken;

	hash_t old_hash = m_hash;
	updateHash ();

	if (m_hash != old_hash) return StackError::WrongHash;
	if (m_destructed)       return StackError::Destructed;

	if (*reinterpret_cast <canary_t*> (m_data                                                   ) != CanaryValue) return StackError::LeftDataBoundBroken;
	if (*reinterpret_cast <canary_t*> (m_data + m_capacity * sizeof (data_t) + sizeof (canary_t)) != CanaryValue) return StackError::RightDataBoundBroken;

	return StackError::NoError;
}

//------------------------------

template <typename data_t>
void Stack <data_t>::printBytes (std::ostream& stream /*= std::cout*/) const
{
	stream << Format ("%0*X ", sizeof (canary_t)*2, *reinterpret_cast <canary_t*> (m_data));

	for (size_t i = 0; i < m_capacity; i++)
	{
		for (size_t b = 0; b < sizeof (data_t); b++)
			stream << Format ("%02X", *reinterpret_cast <unsigned char*> (sizeof (canary_t) + m_data + i * sizeof (data_t) + b));
		stream << ' ';
	}

	stream << Format ("%0*X", sizeof (canary_t)*2, *reinterpret_cast <canary_t*> (m_data + sizeof (canary_t) + sizeof (data_t) * m_capacity));
	stream << '\n';
}

//------------------------------

template <typename data_t>
void Stack <data_t>::printDebugDump (std::ostream& stream /*= std::cout*/) const
{
	stream << Format ("Stack debug dump:\n"                                                            );
	stream << Format ("this     = %p\n",                this                                           );
	stream << Format ("data_t   = %-15s (sizeof = %zu)\n", typeid (data_t  ).name (), sizeof (data_t  ));
	stream << Format ("canary_t = %-15s (sizeof = %zu)\n", typeid (canary_t).name (), sizeof (canary_t));
	stream << Format ("hash_t   = %-15s (sizeof = %zu)\n", typeid (hash_t  ).name (), sizeof (hash_t  ));
	stream << Format ("capacity = %zu, size = %zu\n",   m_capacity, m_size                             );
	stream << Format ("\n"                                                                             );
	stream << Format ("state - unknown (stack is constant)\n"                                          );
	
	stream << Format ("\n"                                                                             );
	stream << Format ("data bytes ([%zu bytes left canary] [%zu bytes data, ...] [%zu bytes right canary]):\n", sizeof (canary_t), sizeof (data_t), sizeof (canary_t));
	printBytes (stream);
}

//------------------------------

template <typename data_t>
void Stack <data_t>::printDebugDump (std::ostream& stream /*= std::cout*/)
{
	stream << Format ("Stack debug dump:\n"                                                            );
	stream << Format ("this     = %p\n",                this                                           );
	stream << Format ("data_t   = %-15s (sizeof = %zu)\n", typeid (data_t  ).name (), sizeof (data_t  ));
	stream << Format ("canary_t = %-15s (sizeof = %zu)\n", typeid (canary_t).name (), sizeof (canary_t));
	stream << Format ("hash_t   = %-15s (sizeof = %zu)\n", typeid (hash_t  ).name (), sizeof (hash_t  ));
	stream << Format ("capacity = %zu, size = %zu\n",   m_capacity, m_size                             );
	stream << Format ("\n"                                                                             );
	stream << Format ("state - '%s'\n",                 StrStackError (ok ())                          );
	
	stream << Format ("\n"                                                                             );
	stream << Format ("data bytes ([%zu bytes left canary] [%zu bytes data, ...] [%zu bytes right canary]):\n", sizeof (canary_t), sizeof (data_t), sizeof (canary_t));
	printBytes (stream);
}

//------------------------------