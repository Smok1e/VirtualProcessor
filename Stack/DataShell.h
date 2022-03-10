#pragma once

//------------------------------

template <typename data_t>
class DataShell
{
public:
	DataShell ();

	void   set (const data_t& value);
	data_t get ();

private:
	char m_data[sizeof (data_t)];

};

//------------------------------

#include "DataShell.hpp"

//------------------------------