#include "ProgramContainer.h"

//------------------------------

ProgramContainer::ProgramContainer ():
	m_data  (),
	m_count (0)
{}

ProgramContainer::ProgramContainer (const ProgramContainer& copy):
	m_data  (copy.m_data),
	m_count (copy.m_count)
{}

//------------------------------

void ProgramContainer::save (std::ostream& stream)
{
	program_header header  = {};
	header.signature       = PROGRAM_SIGNATURE;
	header.version         = ASSEMBLER_VERSION;
	header.elem_count      = count ();
	header.bytes_count     = bytes ();
	header.numbers_acuracy = NUMBERS_ACURACY;

	stream.write (reinterpret_cast <char*> (&header), sizeof (program_header));
	stream.write (reinterpret_cast <char*> (data ()), bytes ());
}

void ProgramContainer::load (std::istream& stream)
{
	program_header header = {};
	stream.read (reinterpret_cast <char*> (&header), sizeof (header));

	if (header.signature != PROGRAM_SIGNATURE) throw std::runtime_error ("Wrong signature"  );
	if (header.version   != ASSEMBLER_VERSION) throw std::runtime_error ("Different version");

	clear ();
	m_data.resize (header.bytes_count);
	stream.read (data (), header.bytes_count);
}

//------------------------------

void ProgramContainer::extend (const byte_t* src, size_t count)
{
	m_data.insert (m_data.end (), src, src+count);
	m_count++;
}

void ProgramContainer::remove (byte_t* dst, size_t count)
{
	if (dst) std::memcpy (dst, m_data.data () + m_data.size () - count, count);

	m_data.erase (m_data.end () - count, m_data.end ());
	m_count--;
}

//------------------------------

ByteCode ProgramContainer::popByteCode ()
{
	return pop <ByteCode> ();
}

stack_value_t ProgramContainer::popStackValue ()
{
	return pop <stack_value_t> ();
}

//------------------------------

bool ProgramContainer::empty () const
{
	return !m_count;
}

size_t ProgramContainer::count () const
{
	return m_count;
}

size_t ProgramContainer::bytes () const
{
	return m_data.size ();
}

//------------------------------

void ProgramContainer::clear ()
{
	m_data.clear ();
	m_count = 0;
}

void ProgramContainer::shrink ()
{
	m_data.shrink_to_fit ();
}

//------------------------------

byte_t* ProgramContainer::begin ()
{
	return m_data.data ();
}

const byte_t* ProgramContainer::begin () const
{
	return const_cast <const byte_t*> (m_data.data ());
}

byte_t* ProgramContainer::end ()
{
	return begin () + m_data.size ();
}

const byte_t* ProgramContainer::end () const
{
	return begin () + m_data.size ();
}

byte_t* ProgramContainer::data ()
{
	return begin ();
}

const byte_t* ProgramContainer::data () const
{
	return begin ();
}

//------------------------------