//------------------------------------------------------------------------------

//	   2012/07/23 Tony    first description
//------------------------------------------------------------------------------
#ifndef _ENV_INCLUDE_NXP_NX_BIT_ACCESSOR_H_
#define _ENV_INCLUDE_NXP_NX_BIT_ACCESSOR_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

u32 NX_BIT_SetBitRange32( u32 OldValue, u32 BitValue, u32 MSB, u32 LSB );
u32 NX_BIT_SetBit32( u32 OldValue, u32 BitValue, u32 BitNumber );
u16 NX_BIT_SetBitRange16( u16 OldValue, u32 BitValue, u32 MSB, u32 LSB );
u16 NX_BIT_SetBit16( u16 OldValue, u32 BitValue, u32 BitNumber );
u32 NX_BIT_GetBitRange32( u32 Value, u32 MSB, u32 LSB );
boolean NX_BIT_GetBit32( u32 Value, u32 BitNumber );
u16 NX_BIT_GetBitRange16( u16 Value, u32 MSB, u32 LSB );
boolean NX_BIT_GetBit16( u16 Value, u32 BitNumber );

#ifdef	__cplusplus
}
#endif  

#endif  // _NX_BIT_ACCESSOR_H_
