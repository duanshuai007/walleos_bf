#ifndef _INCLUDE_STDDEF_H_
#define _INCLUDE_STDDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (__GNUC__ >= 4)
#define offsetof1(type, member)  __builtin_offsetof(type, member)
#else
#define offsetof1(type, field)   ((size_t)(&((type *)0)->field))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STDDEF_H__ */

