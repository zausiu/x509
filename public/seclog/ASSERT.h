#ifndef MY_ASSERT_H_
#define MY_ASSERT_H_

#include "log.h"
#include "Compiler.h"

#define ASSERT(expr) do{if (unlikely(!(expr))){LOG_ASSERT("assert \"%s\" failed", #expr);}}while(0)
#define ASSERT_ERR(expr, errinfo) do{if (unlikely(!(expr))){LOG_ASSERT("assert \"%s\" failed:%s", #expr, errinfo);}}while(0)
#define ASSERT_RET(expr, args...) do{if (unlikely(!(expr))){LOG_ASSERT("assert \"%s\" failed", #expr); return	args; }}while(0)
#define ASSERT_ERR_RET(expr, errinfo, args...) do{if (unlikely(!(expr))){LOG_ASSERT("assert \"%s\" failed:%s", #expr, errinfo); return	args; }}while(0)

#define CAT_TOKEN_1(t1,t2) t1##t2
#define CAT_TOKEN(t1,t2) CAT_TOKEN_1(t1,t2)

#define COMPILE_ASSERT(x)  \
        enum {CAT_TOKEN (comp_assert_at_line_, __LINE__) = 1 / !!(x) };

#endif

