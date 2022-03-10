#pragma once

//------------------------------

#include <cstdio>
#include <ostream>
#include <iostream>

#include "Utils.hpp"
#include "StackCheckError.h"

//------------------------------

#ifdef CHECK_STACK_
	#undef CHECK_STACK_
#endif

#if (defined (_DEBUG) || defined (_STACK_FORCE_CHECK))
	#define CHECK_STACK_ StackDebugCheck (ok (), __FILE__, __func__, __LINE__);
#else
	#define CHECK_STACK_ ;
#endif

//------------------------------

template <typename data_t>
class Stack
{
public:
	typedef unsigned long canary_t;
	typedef long     long hash_t;

	const static canary_t CanaryValue;
	const static char     FreeValue;

	Stack ();
	Stack (size_t size);

	~Stack ();

	void resize (size_t new_size);

	void          push    (const data_t& value);
	data_t        pop     ();
	inline data_t top     () const;
	void          clear   ();
	void          reverse ();

	inline size_t size     () const;
	inline size_t capacity () const;
	inline size_t bytes    () const;
	inline bool   empty    () const;

	inline       data_t* begin ();
	inline const data_t* begin () const;
	inline       data_t* end   ();
	inline const data_t* end   () const;

	void printBytes     (std::ostream& stream = std::cout) const;
	void printDebugDump (std::ostream& stream = std::cout) const;
	void printDebugDump (std::ostream& stream = std::cout);

private:
	canary_t m_lft_canary;
	
	hash_t m_hash;

	char*  m_data;
	size_t m_size;
	size_t m_capacity;

	bool m_destructed;

	canary_t m_rgt_canary;

	inline void   set (size_t index, const data_t& value);
	inline data_t get (size_t index) const;

	inline data_t* getData (char* data = nullptr, size_t index = 0);

	inline void free   (size_t index, char* data = nullptr);
	inline bool isFree (size_t index, char* data = nullptr) const;

	void updateHash ();

	StackError ok ();

};

//------------------------------

void StackDebugCheck (StackError check_result, const char* file, const char* func, int line);
void MemorySwap      (void* a, void* b, size_t size);

//------------------------------

#include "Stack.hpp"

//------------------------------