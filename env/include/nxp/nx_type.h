//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: base
//	File		: nx_type.h
//	Description	: support base type
//	Author		: Goofy
//	Export		:
//	History		:
//		2010.04.27	Hans
//		2007.04.04	Goofy	First draft
//------------------------------------------------------------------------------
#ifndef _ENV_INCLUDE_NXP_NX_TYPE_H_
#define	_ENV_INCLUDE_NXP_NX_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
//------------------------------------------------------------------------------
/// @name Basic data types
///
/// @brief Basic data type define and Data type constants are implemen \n
///	tation-dependent ranges of values allowed for integral data types. \n
/// The constants listed below give the ranges for the integral data types
//------------------------------------------------------------------------------
/// @{
#define S8_MIN			-128				///< signed char min value
#define S8_MAX			127					///< signed char max value
#define S16_MIN			-32768				///< signed short min value
#define S16_MAX			32767				///< signed short max value
#define S32_MIN			-2147483648			///< signed integer min value
#define S32_MAX			2147483647			///< signed integer max value

#define U8_MIN			0					///< unsigned char min value
#define U8_MAX			255					///< unsigned char max value
#define U16_MIN			0					///< unsigned short min value
#define U16_MAX			65535				///< unsigned short max value
#define U32_MIN			0					///< unsigned integer min value
#define U32_MAX			4294967295			///< unsigned integer max value
/// @}


//------------------------------------------------------------------------------
/// @name Basic type's size check
///
/// @brief this is compile time assert for check type's size. if exist don't intended size\n
/// of Basic type then it's occur compile type error
//------------------------------------------------------------------------------
/// @{
#ifndef __GNUC__
#define NX_CASSERT(expr) typedef char __NX_C_ASSERT__[(expr)?1:-1]
#else
#define NX_CASSERT_CONCAT_(a, b) a##b
#define NX_CASSERT_CONCAT(a, b) NX_CASSERT_CONCAT_(a, b)
#define NX_CASSERT(expr) typedef char NX_CASSERT_CONCAT(__NX_C_ASSERT__,__LINE__)[(expr)?1:-1]
#endif

NX_CASSERT	(sizeof(s8)		    == 1);
NX_CASSERT	(sizeof(s16)	    == 2);
NX_CASSERT	(sizeof(s32)	    == 4);
NX_CASSERT	(sizeof(s64)        == 8);
NX_CASSERT	(sizeof(u8)		    == 1);
NX_CASSERT	(sizeof(u16)	    == 2);
NX_CASSERT	(sizeof(u32)	    == 4);
NX_CASSERT	(sizeof(u64)        == 8);
NX_CASSERT	(sizeof(boolean)	== 4);

#ifdef __cplusplus
}
#endif

#endif	// __NX_TYPE_H__
