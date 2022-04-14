#include "ProgramContainer.h"

//------------------------------

ProgramContainer::ProgramContainer ():
	m_data ()
{}

ProgramContainer::ProgramContainer (const ProgramContainer& copy):
	m_data (copy.m_data)
{}

//------------------------------

void ProgramContainer::save (std::ostream& stream) const
{
	program_header header   = {};
	header.signature        = PROGRAM_SIGNATURE;
	header.version          = ASSEMBLER_VERSION;
	header.bytes_count      = bytes ();
	header.numbers_modifier = NUMBERS_MODIFIER;

	stream.write (reinterpret_cast <const char*> (&header), sizeof (program_header));
	stream.write (reinterpret_cast <const char*> (data ()), bytes ());
}

void ProgramContainer::save (const char* filename) const
{
	std::ofstream stream;
	stream.exceptions (std::ios::failbit | std::ios::badbit);
	stream.open (filename, std::ios::binary);

	save (stream);
	stream.close ();
}

//------------------------------

void ProgramContainer::load (std::istream& stream)
{
	program_header header = {};
	stream.read (reinterpret_cast <char*> (&header), sizeof (header));

	if (header.signature != PROGRAM_SIGNATURE) throw std::runtime_error ("Wrong signature"  );
	if (header.version   != ASSEMBLER_VERSION) throw std::runtime_error ("Different version");

	clear ();
	m_data.resize (header.bytes_count);
	stream.read (reinterpret_cast <char*> (data ()), header.bytes_count);
}

void ProgramContainer::load (const char* filename)
{
	std::ifstream stream;
	stream.exceptions (std::ios::failbit | std::ios::badbit);
	stream.open (filename, std::ios::binary);

	load (stream);
	stream.close ();
}

//------------------------------

void ProgramContainer::insert (size_t index_in_bytes, const byte_t* data, size_t count_in_bytes)
{
	m_data.insert (m_data.begin (), data, data+count_in_bytes);
}

//------------------------------

void ProgramContainer::extend (const byte_t* src, size_t count)
{
	m_data.insert (m_data.end (), src, src+count);
}

void ProgramContainer::remove (byte_t* dst, size_t count)
{
	if (dst) std::memcpy (dst, m_data.data () + m_data.size () - count, count);
	m_data.erase (m_data.end () - count, m_data.end ());
}

//------------------------------

bool ProgramContainer::empty () const
{
	return !bytes ();
}

size_t ProgramContainer::bytes () const
{
	return m_data.size ();
}

//------------------------------

void ProgramContainer::clear ()
{
	m_data.clear ();
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