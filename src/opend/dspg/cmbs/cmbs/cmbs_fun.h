/*!
*   \file       fun_util.h
*   \brief      HAN FUN API
*   \author     ULE BS Team
*
*
*******************************************************************************/

#if	!defined( _CMBS_FUN_H )
#define	_CMBS_FUN_H

#include "cmbs_api.h"

/*Device Management Interface ID 
====================================
This interface is used for registering Devices and Units with their associated data
such as Unit Type and the list of interfaces they are supporting.
It is also used for discovering all device and units by a configuration application
*/

#define DEVICE_MGMT_INTERFACE_ID    0x0001

/* Device Management Interface - Server */
/* Attributes */
#define DEVICE_MGMT_IF_SERVER_ATTR_ID_NUM_OF_DEVICES		   0x0001

/* Commands */
#define DEVICE_MGMT_IF_SERVER_CMD_ID_REGISTER_DEVICE    		0x0001
#define FUN_IF_DEV_MNGT_CMD_REGISTER_DEVICE 				 	0x0001
#define FUN_IF_DEV_MNGT_CMD_DEREGISTER_DEVICE		 			0x0002
#define FUN_IF_DEV_MNGT_CMD_START_SESS_READ_REG_INFO			0x0003
#define FUN_IF_DEV_MNGT_CMD_END_SESS_READ_REG_INFO	 			0x0004
#define FUN_IF_DEV_MNGT_CMD_GET_ENTRIES				 			0x0005

#define 	MAX_NUM_OF_OPTIONAL_INTERFACES_IN_UNIT				8
#define 	MAX_NUM_OF_UNITS_IN_DEVICE								6

/////////////////////////////////
/////////	 Structs	/////////
/////////////////////////////////
typedef  struct
{
	u8			UnitId;
	u16 		UnitType;
	u16 		NumberOfOptionalInterfaces;
	u16			OptionalInterfaces[MAX_NUM_OF_OPTIONAL_INTERFACES_IN_UNIT];
} ST_FUN_UNIT_INFO;

typedef  struct 
{
	u16							DeviceId;
	u16							DeviceEMC;
	u8							NumOfUnits;
	ST_FUN_UNIT_INFO 			Units[MAX_NUM_OF_UNITS_IN_DEVICE];
} ST_FUN_DEVICE_INFO, * PST_FUN_DEVICE_INFO; 

#define CMBS_HAN_IE_FUN_DEVICE_INFO_SIZE			sizeof(ST_FUN_DEVICE_INFO)

// TODO: store request id as cookie and remove constant value
#define CMBS_HAN_FUN_REGISTRATION_REQ_ID			50

///////////////////////////////////////////////////////////////////////////////
// brief	Macro which defines a bitfield mask in u8/u16/u32 value
//
// param	FieldName		Variable prefix
// 			Type 			u8, u16, u32
//			BitOffset		The offset of the bitfield in the value 
//			BitWidth		Number of bits in the bitfield
///////////////////////////////////////////////////////////////////////////////
#define DEFINE_BITFIELD( FieldName, Type, BitOffet, BitWidth )							\
	enum																				\
	{																					\
		FieldName##_OFS			= (BitOffet),											\
		FieldName##_WIDTH		= (BitWidth),											\
		FieldName##_MASK		= (Type)((( (Type)1<<(BitWidth))-1) << (BitOffet)),		\
	};


#define GET_BITFIELD_VAL( Var, FieldName )												\
	( ((Var) & FieldName##_MASK ) >> FieldName##_OFS )			


#define SET_BITFIELD_VAL( Var, FieldName, NewValue )									\
	( ((Var) & (~FieldName##_MASK)) | ((NewValue) << FieldName##_OFS) )


typedef enum
{
	FUN_IF_DEV_MNGT_REG_RESP_UID_TYPE_NONE					= 0x0,
	FUN_IF_DEV_MNGT_REG_RESP_UID_TYPE_IPUI						= 0x1,
	FUN_IF_DEV_MNGT_REG_RESP_UID_TYPE_IEEE_MAC_48			= 0x2,
	FUN_IF_DEV_MNGT_REG_RESP_UID_TYPE_URI						= 0x3,
	
}
t_en_hanIf_DevMngt_Reg_Dev_UID_Type;

typedef enum
{
	FUN_IF_DEV_MNGT_REG_RESP_DISCR_TYPE_NON_PROPRIETARY		= 0x0,
	FUN_IF_DEV_MNGT_REG_RESP_DISCR_TYPE_PROPRIETARY			= 0x1,
}
t_en_hanIf_DevMngt_RegResp_DiscrType;

#define FUN_IF_DEV_MNGT_UID_LENGTH_IPUI					5
#define FUN_IF_DEV_MNGT_UID_LENGTH_IEEE_MAC_48			12

#define OTA_BASE_DEVICE_ID									0
#define OTA_BASE_UNIT_MANAGMENT_UNIT_ID 				0
#define OTA_DST_UNIT_MANAGMENT_UNIT_ID  					0
#define OTA_MSG_TYPE_COMMAND_RESPONSE					3
#define OTA_ADDR_TYPE_INDIVIDUAL							0
#define OTA_INTERFACE_TYPE_SERVER							1
#define FUN_INTERFACE_DEVICE_MGNT						1


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Define bitfields for Device Managment interface													///
//  							FieldName 							Type	BitOffet	BitWidth	///
///////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_BITFIELD( FUN_IF_DEV_MNGT_REG_DISCRIMINATOR_TYPE,			u8,		7,			1 );
DEFINE_BITFIELD( FUN_IF_DEV_MNGT_REG_DEV_UID_TYPE,					u8,		0,			7 );
DEFINE_BITFIELD( FUN_IF_DEV_MNGT_REG_RES_DISCRIMINATOR_TYPE,		u16,	15,			1 );
DEFINE_BITFIELD( FUN_IF_DEV_MNGT_REG_RES_DEVICE_ADDRESS,			u16,	0,			15);





#endif	// _FUN_UTIL_H

/**********************[End Of File]**********************************************************************************************************/