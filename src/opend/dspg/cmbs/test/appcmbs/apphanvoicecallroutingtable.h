/*!
*   \file       apphanvoicecallroutingtable.h
*   \brief      
*   \author     Moria Aharon
*
*   @(#)        apphanvoicecallroutingtable.h~1
*
*******************************************************************************/

#if    !defined( _APPHANVOICECALLROUTINGTABLE_H )
#define    _APPHANVOICECALLROUTINGTABLE_H

#include "cmbs_api.h"


#define DIAL_STR_LEN    16
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct  
{
    u8    u8_SrcType; 
    u16   u16_SrcId;
    u8    u8_SrcUnitId;
    u8    u8_DstType; 
    u16   u16_DstId;
    u8    u8_DstUnitId;
    u8    pu8_DstDialStr[DIAL_STR_LEN];
} st_apphan_VoiceCallRoutingTableRecord;


typedef enum
{
    HS_ENTITY = 0,
    DEVICE_ENTITY,

    ENTITY_TYPE_MAX,
    
} t_en_VoiceCallRoutingTable_EntityType;

// VoiceCall Routing Table Record size
#define VOICE_CALL_ROUTING_TABLE_SIZE 20


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool  p_VoiceCallRoutingTable_AddRecordToTable(st_apphan_VoiceCallRoutingTableRecord * pst_apphan_VoiceCallRoutingTableRecord);
bool  p_VoiceCallRoutingTable_RemoveRecordFromTable(st_apphan_VoiceCallRoutingTableRecord * pst_apphan_VoiceCallRoutingTableRecord);
bool p_VoiceCallRoutingTable_IsRecordValid (st_apphan_VoiceCallRoutingTableRecord * pst_apphan_VoiceCallRoutingTableRecord);
u16 p_VoiceCallRoutingTable_CountTableEntries(void);
void  p_VoiceCallRoutingTable_PrintTable(void);
void p_VoiceCallRoutingTable_ClearTable(void);
bool p_VoiceCallRoutingTable_FindMatchForHS(u16 u16_HsNum, const char *pu8_DialStr, st_apphan_VoiceCallRoutingTableRecord *pst_VoiceCallRoutingTableRecord);
bool p_VoiceCallRoutingTable_FindMatchForDevice(u16 u16_DeviceId, u8 u8_UnitId, const char *pu8_DialStr, st_apphan_VoiceCallRoutingTableRecord *pst_VoiceCallRoutingTableRecord);

#endif // _APPHANAUDIOROUTINGTABLE_H