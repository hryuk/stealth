#include "preprocessor/seq/for_each_i.hpp"
#include "preprocessor/seq/fold_left.hpp"
#include "preprocessor/seq/size.hpp"
#include "preprocessor/cat.hpp"

#define HASH_AND_EMIT(SEQ) EMIT_WORD(BOOST_PP_SEQ_FOLD_LEFT(CRYPT_BYTE, 0, SEQ))
#define CRYPT_BYTE(s, st, x) (((st)>>1)^((x)<<7))

#define EMIT_BYTE_ARRAY(SEQ) BOOST_PP_SEQ_FOR_EACH_I(EMIT_BYTE, 0, SEQ)
#define EMIT_BYTE(r, d, i, e) __asm _emit ((e)&0xFF)
#define EMIT_WORD(d)\
	EMIT_BYTE(0,0,0,((d) >> 0))\
	EMIT_BYTE(0,0,0,((d) >> 8))
#define EMIT_DWORD(d)\
	EMIT_WORD(((d) >> 0))\
	EMIT_WORD(((d) >> 16))

#define CREATE_IDS(SEQ)\
	enum IDS{BOOST_PP_SEQ_FOR_EACH_I(ENUM_ITEM, 0, SEQ) STACKSIZE = (BOOST_PP_SEQ_SIZE(SEQ)*4)};
#define ENUM_ITEM(r, d, i, e) BOOST_PP_CAT(_, e) = ((i+1)*4),

//PUSH de punteros
#ifndef SHELLCODE
	#define pushr(addr)\
		push (addr)
#else
	#define pushr(addr)\
		__asm{push (addr)}\
		__asm{add [esp], edi}
#endif