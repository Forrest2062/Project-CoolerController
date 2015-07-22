/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#define TRUE 1
#define FALSE 0
#define NULL (void *)0

/*Bit Operation macro*/ 
#define SETBIT(var,bitnum) ((var) |= (0x1<<(bitnum)))
#define CLRBIT(var,bitnum) ((var) &= ~(0x1<<(bitnum)))
#define REDBIT(var,bitnum) (((var)>>(bitnum))&0x01)

#define OFFSETOF(type,member) ((unsigned int)&((type *)0)->member)  

/*base data type of hitech-c for pic*/
typedef bit bool;
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short int u16;
typedef signed short int s16;
typedef unsigned short long int u24;
typedef signed short long int s24;
typedef unsigned long int u32;
typedef signed long int s32;

#endif
