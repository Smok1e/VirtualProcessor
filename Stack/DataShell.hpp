#pragma once

#include "DataShell.h"

//------------------------------

template <typename data_t>
DataShell <data_t>::DataShell ():
	m_data ({0})
{}

//------------------------------

template <typename data_t>
void DataShell <data_t>::set (const data_t& value)
{
	*reinterpret_cast <data_t*> (m_data) = value;
}

template <typename data_t>
data_t DataShell <data_t>::get ()
{
	return *reinterpret_cast <data_t*> (m_data);
}

//------------------------------