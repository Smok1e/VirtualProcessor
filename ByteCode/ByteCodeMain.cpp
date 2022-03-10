#include "ByteCode.h"

//------------------------------

int main ()
{
	for (size_t i = 0; i < ByteCodesAmount; i++)
		printf ("%s\n", CommandManual (static_cast <ByteCode> (i)));
}

//------------------------------