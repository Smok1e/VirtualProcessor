#include "Stack.h"

//------------------------------

void StackDebugCheck (StackError check_result, const char* file, const char* func, int line)
{
	if (check_result != StackError::NoError)
		throw stack_check_error (check_result, func, line);
}

//------------------------------

void MemorySwap (void* a, void* b, size_t size)
{
	__int64 buffer = 0;
	size_t buffsize = sizeof (buffer);

	#define LVALUE_(type, data, offset) *( reinterpret_cast <type*> ( reinterpret_cast <char*> (data) + (offset) ) )

	#define SWAP_(type)                                                    \
	{																	   \
		LVALUE_ (type, &buffer, 0     ) = LVALUE_ (type, a,       offset); \
		LVALUE_ (type, a,       offset) = LVALUE_ (type, b,       offset); \
		LVALUE_ (type, b,       offset) = LVALUE_ (type, &buffer, 0     ); \
		offset += sizeof (type);									 	   \
	}

	size_t offset = 0;
	int    bytes  = size;

	for (bytes, offset; bytes >= buffsize; bytes -= buffsize)
		SWAP_ (__int64);

	if (bytes >= sizeof (__int32)) { SWAP_ (__int32); bytes -= sizeof (__int32); }
	if (bytes >= sizeof (__int16)) { SWAP_ (__int16); bytes -= sizeof (__int16); }
	if (bytes >= sizeof (__int8 )) { SWAP_ (__int8 ); bytes -= sizeof (__int8 ); }

	#undef LVALUE_
	#undef SWAP_
}

//------------------------------