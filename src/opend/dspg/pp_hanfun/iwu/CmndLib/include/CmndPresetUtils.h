#ifndef _CMND_PRESET_UTILS_H
#define _CMND_PRESET_UTILS_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "TypeDefs.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
/// @brief		Adjust Endian ForDhx91
///////////////////////////////////////////////////////////////////////////////
u16 p_CmndPresetUtils_u16ToDhx91Endian( u16 u16_Value );
u32 p_CmndPresetUtils_u32ToDhx91Endian( u32 u32_Value );

extern_c_end

#endif  // _CMND_PRESET_UTILS_H
