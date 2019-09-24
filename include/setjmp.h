/* copyright(C) 2003 H.Kawai (under KL-01). */
#ifndef _INCLUDE_SETJMP_H_ // [
#define _INCLUDE_SETJMP_H_

#if (defined(__cplusplus))
	extern "C" {
#endif

typedef s32 jmp_buf[3]; /* EBP, EIP, ESP */

#define setjmp(env)			__builtin_setjmp(env)
#define longjmp(env, val)	__builtin_longjmp(env, val)

#if (defined(__cplusplus))
	}
#endif

#endif
