/*!
*   \file       apphanvoicecallroutingtable.c
*   \brief      
*   \author     Moria Aharon
*
*   @(#)        apphanvoicecallroutingtable.c~1
*
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apphanvoicecallroutingtable.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_ENTITY_ID     0
#define ANY_ENTITY_ID         (u16)-1

// VoiceCall Routing Table DB 
static st_apphan_VoiceCallRoutingTableRecord g_st_apphan_VoiceCallRoutingTable[VOICE_CALL_ROUTING_TABLE_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool  p_VoiceCallRoutingTable_AddRecordToTable(st_apphan_VoiceCallRoutingTableRecord * pst_apphan_VoiceCallRoutingTableRecord)
{
    u16 u16_Index;
    
    if (!p_VoiceCallRoutingTable_IsRecordValid(pst_apphan_VoiceCallRoutingTableRecord))
    {
        return FALSE;
    }

    for (u16_Index = 0; u16_Index < VOICE_CALL_ROUTING_TABLE_SIZE; u16_Index++)
    {
        // search for the first empty record
        if (g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId == INVALID_ENTITY_ID)
        {
            g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstType = pst_apphan_VoiceCallRoutingTableRecord->u8_DstType;
            g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_DstId = pst_apphan_VoiceCallRoutingTableRecord->u16_DstId;
            g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstUnitId = pst_apphan_VoiceCallRoutingTableRecord->u8_DstUnitId;
            g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcType = pst_apphan_VoiceCallRoutingTableRecord->u8_SrcType;
            g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId = pst_apphan_VoiceCallRoutingTableRecord->u16_SrcId;
            g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcUnitId = pst_apphan_VoiceCallRoutingTableRecord->u8_SrcUnitId;

            if (pst_apphan_VoiceCallRoutingTableRecord->pu8_DstDialStr[0] != 0)
                strcpy((char *)g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr, (const char *)pst_apphan_VoiceCallRoutingTableRecord->pu8_DstDialStr);

            return TRUE;
        }

    }
    
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool  p_VoiceCallRoutingTable_RemoveRecordFromTable(st_apphan_VoiceCallRoutingTableRecord * pst_apphan_VoiceCallRoutingTableRecord)
{
    u16 u16_Index;
    
    for (u16_Index=0; u16_Index<VOICE_CALL_ROUTING_TABLE_SIZE; u16_Index++)
    {
        if (g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstType == pst_apphan_VoiceCallRoutingTableRecord->u8_DstType           &&
                g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_DstId == pst_apphan_VoiceCallRoutingTableRecord->u16_DstId         &&
                g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstUnitId == pst_apphan_VoiceCallRoutingTableRecord->u8_DstUnitId   &&
                g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcType == pst_apphan_VoiceCallRoutingTableRecord->u8_SrcType       &&
                g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId == pst_apphan_VoiceCallRoutingTableRecord->u16_SrcId         &&
                g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcUnitId == pst_apphan_VoiceCallRoutingTableRecord->u8_SrcUnitId   &&
                !strcmp((char *)g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr, (const char *)pst_apphan_VoiceCallRoutingTableRecord->pu8_DstDialStr))
        {
            memset((void*) &g_st_apphan_VoiceCallRoutingTable[u16_Index], 0, sizeof(st_apphan_VoiceCallRoutingTableRecord));
                    
            return TRUE;
        }
    }

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool p_VoiceCallRoutingTable_IsRecordValid (st_apphan_VoiceCallRoutingTableRecord * pst_apphan_VoiceCallRoutingTableRecord)
{
    if (pst_apphan_VoiceCallRoutingTableRecord->u8_SrcType >= ENTITY_TYPE_MAX || 
         pst_apphan_VoiceCallRoutingTableRecord->u8_DstType >= ENTITY_TYPE_MAX ||
         pst_apphan_VoiceCallRoutingTableRecord->u16_SrcId == INVALID_ENTITY_ID || 
         pst_apphan_VoiceCallRoutingTableRecord->u16_DstId == INVALID_ENTITY_ID )

    {
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
u16 p_VoiceCallRoutingTable_CountTableEntries(void)
{
    u16 u16_Index, u16_EntryCount = 0;

    for (u16_Index=0; u16_Index<VOICE_CALL_ROUTING_TABLE_SIZE; u16_Index++)
    {
        if (g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId != INVALID_ENTITY_ID)
        {
            u16_EntryCount++;
        }
    }
    
    return u16_EntryCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void  p_VoiceCallRoutingTable_PrintTable(void)
{
    u16 u16_Index;

    printf("\n=============================\n");    
    printf("     VoiceCall Routing Table\n");    
    printf("=============================\n\n");
        
    for (u16_Index=0; u16_Index<VOICE_CALL_ROUTING_TABLE_SIZE; u16_Index++)
    {
        if (g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId != INVALID_ENTITY_ID)
        {

            printf("Src Type: %s\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcType == DEVICE_ENTITY ? "Device" : "HS");
            printf("Src ID: %d\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId);

            if(g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcType == DEVICE_ENTITY)
            {
                printf("Src Unit ID: %d\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcUnitId);
            }

            printf("Dst Type: %s\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstType==DEVICE_ENTITY ? "Device" : "HS");
            printf("Dst ID: %d\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_DstId);

            if(g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstType == DEVICE_ENTITY)
            {
                printf("Dst Unit ID: %d\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_DstUnitId);
            }

            if (g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr[0])
            {
                printf("Dst Dial Str: %s\n", g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr);
            }

            printf("------------------------------\n");

        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void p_VoiceCallRoutingTable_ClearTable(void)
{
    memset( (void *) &g_st_apphan_VoiceCallRoutingTable[0], 0, sizeof(g_st_apphan_VoiceCallRoutingTable));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool p_VoiceCallRoutingTable_FindMatchForHS(u16 u16_HsNum, const char *pu8_DialStr, st_apphan_VoiceCallRoutingTableRecord *pst_VoiceCallRoutingTableRecord)
{
    u16 u16_Index;
    
    for (u16_Index=0; u16_Index<VOICE_CALL_ROUTING_TABLE_SIZE; u16_Index++)
    {
        // HS entry
        if (g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcType == HS_ENTITY && 
            // HS number
            (g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId == ANY_ENTITY_ID || g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId == u16_HsNum) &&
            // check if dest dial string is the same 
            ((g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr[0] == 0) ||
            (pu8_DialStr && !strcmp(pu8_DialStr, (const char *)g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr))))
        {
            memcpy((void *) pst_VoiceCallRoutingTableRecord, (void *) &g_st_apphan_VoiceCallRoutingTable[u16_Index], sizeof(st_apphan_VoiceCallRoutingTableRecord));
            return TRUE;
        }
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool p_VoiceCallRoutingTable_FindMatchForDevice(u16 u16_DeviceId, u8 u8_UnitId, const char *pu8_DialStr, st_apphan_VoiceCallRoutingTableRecord *pst_VoiceCallRoutingTableRecord)
{
    u16 u16_Index;
    
    for (u16_Index=0; u16_Index<VOICE_CALL_ROUTING_TABLE_SIZE; u16_Index++)
    {
        // Device entry
        if (g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcType == DEVICE_ENTITY && 
            // device ID
            (g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId == ANY_ENTITY_ID || g_st_apphan_VoiceCallRoutingTable[u16_Index].u16_SrcId == u16_DeviceId) &&
            // Unit ID
            (g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcUnitId == (u8)ANY_ENTITY_ID || g_st_apphan_VoiceCallRoutingTable[u16_Index].u8_SrcUnitId == u8_UnitId) &&
            // check if dest dial string is the same or missinig on both sides
            ((g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr[0] == 0) ||
            (pu8_DialStr && !strcmp(pu8_DialStr, (const char *)g_st_apphan_VoiceCallRoutingTable[u16_Index].pu8_DstDialStr))))
        {
            memcpy((void *) pst_VoiceCallRoutingTableRecord, (void *) &g_st_apphan_VoiceCallRoutingTable[u16_Index], sizeof(st_apphan_VoiceCallRoutingTableRecord));
            return TRUE;
        }
    }

    return FALSE;
}


//--------[End of file]---------------------------------------------------------------------------------------------------------------------------------
