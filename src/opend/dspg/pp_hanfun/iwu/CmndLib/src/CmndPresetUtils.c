#include "CmndPresetUtils.h"
#include "Endian.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u16 p_CmndPresetUtils_u16ToDhx91Endian( u16 u16_Value )
{
	// since DHX91 is little endian, it stores values in eeprom as littile endian
	// so if this machine is also little, no need to convert
	if ( ENDIANNESS_IS_LITTLE ) 
	{
		return u16_Value;
	}
	// convert to little endian
	else 
	{
		return p_Endian_Reorder16(u16_Value);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u32 p_CmndPresetUtils_u32ToDhx91Endian( u32 u32_Value )
{
	// since DHX91 is little endian, it stores values in eeprom as littile endian
	// so if this machine is also little, no need to convert
	if ( ENDIANNESS_IS_LITTLE ) 
	{
		return u32_Value;
	}
	// convert to little endian
	else 
	{
		return p_Endian_Reorder32(u32_Value);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
