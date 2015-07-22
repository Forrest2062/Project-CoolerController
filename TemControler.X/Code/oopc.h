/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#ifndef _OOPC_H_
#define _OOPC_H_

/*define oopc level here*/
#define OOPC_PURE_STATIC 1

#define CLASS(type) \
 typedef struct type type; \
 struct type

#if defined(OOPC_PURE_STATIC)

#define CTOR(type) \
 void *type##Setting(type *t) \
 {

#elif defined(OOPC_STATIC)

#define CTOR(type) \
 void *type##New() \
 { \
    struct type *t; \
    t = (struct type *)malloc(sizeof(struct type));

#elif defined(OOPC_DYNAMIC)

#define CTOR(type) \
 void *type##Setting(type *); \
 void *type##New() \
 { \
    struct type *t; \
    t = (struct type *)malloc(sizeof(struct type)); \
    return type##Setting(t); \
 } \
 void *type##Setting(type *t) \
 {

#else

#error not define oopc level type

#endif

#define END_CTOR return (void *)t;}
#define FUNCTION_SETTING(f1,f2) t->f1 = f2;
#define IMPLEMENTS(type) type type
#define INTERFACE(type) \
 typedef struct type type; \
 struct type

#endif

