#include "preprocessor/seq/for_each_i.hpp"
#include "preprocessor/seq/fold_left.hpp"
#include "preprocessor/seq/size.hpp"
#include "preprocessor/cat.hpp"

#define HASH_AND_EMIT(SEQ) EMIT_DWORD(BOOST_PP_SEQ_FOLD_LEFT(CRYPT_BYTE, BOOST_PP_SEQ_HEAD(SEQ), BOOST_PP_SEQ_TAIL(SEQ)))
#define CRYPT_BYTE(s, st, x) (((st) << 0x2)+(x))

#define EMIT_BYTE_ARRAY(SEQ) BOOST_PP_SEQ_FOR_EACH_I(EMIT_BYTE, 0, SEQ)
#define EMIT_BYTE(r, d, i, e) __asm _emit (e)
#define EMIT_DWORD(d)\
	EMIT_BYTE(0,0,0,((d) >> 0)&0xFF)\
	EMIT_BYTE(0,0,0,((d) >> 8)&0xFF)\
	EMIT_BYTE(0,0,0,((d) >> 16)&0xFF)\
	EMIT_BYTE(0,0,0,((d) >> 24)&0xFF)
#define CREATE_IDS(SEQ)\
	enum IDS{BOOST_PP_SEQ_FOR_EACH_I(ENUM_ITEM, 0, SEQ) STACKSIZE = (BOOST_PP_SEQ_SIZE(SEQ)*4)};
#define ENUM_ITEM(r, d, i, e) BOOST_PP_CAT(_, e) = ((i+1)*4),