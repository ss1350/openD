/*!
*   \file       apphan.c
*   \brief      HAN API
*   \author     CMBS Team
*
*
*******************************************************************************/

#include <string.h>
#include <stdio.h>

#include "apphanDb.h"
#include "apphan.h"

#include "cfr_mssg.h"
#include "appcmbs.h"

#include "cmbs_fifo.h"

#define 		FUN_MSG_FIFO_SIZE	20		// max messages per HAN device


void han_applUleMsgFiFoInitialize();

////////////////////////////////////////////////////

// Message Queue per ULE device -- definitions

static ST_FUN_MSG		g_CMBS_UleMsgBuffer[MAX_ULE_DEVICES_HOST][FUN_MSG_FIFO_SIZE];
static ST_CMBS_FIFO	g_UleMsgFifo[MAX_ULE_DEVICES_HOST];

static bool			WaitForReady[MAX_ULE_DEVICES_HOST];		// TxRequest was sent, we ae waiting for TxReady indication
static bool			ReleaseLink[MAX_ULE_DEVICES_HOST];		// Dynamically updated by application: If to send TxEnd to device when its fifo is empty, to save link

// Consecutive Delete requests should be serialized.
static st_DeleteRequestQueue 	PendingDeleteReqests[MAX_ULE_DEVICES_HOST+1];		// last entry is for Delete ALL pending request

static bool			DuringDeleteReq=FALSE;

static CFR_CMBS_CRITICALSECTION HanServerCriticalSection;

//////////////////////////////////////////////////

ST_CMBS_FIFO* app_HanDbGetUleMsgFifo()
{
	return g_UleMsgFifo;
}

bool app_HanDbPrintFifoStatistic(u16 u16_DeviceID)
{
	//print only devices that used fifo
	if(g_UleMsgFifo[u16_DeviceID].s_fifo_statistics.u32_MaxInUse > 0)
	{
		printf("\n\n==== FIFO STATISTICS FOR DEVICE:   %d ====\n",u16_DeviceID);
		cmbs_util_FifoPrintStatistics(&g_UleMsgFifo[u16_DeviceID]);
		return TRUE;
	}
	return FALSE;
}


void app_HanDbInitialize()
{
	han_applUleMsgFiFoInitialize();
	CFR_CMBS_INIT_CRITICALSECTION(HanServerCriticalSection);	// init of this CS is NOT conditioned by HanServer flag

	SetDuringDeleteReq(FALSE);
}

void			  han_applUleMsgFiFoInitialize ()
{
	u32 u32_Index;
	
	for ( u32_Index = 0; u32_Index < MAX_ULE_DEVICES_HOST; ++u32_Index )
	{
		cmbs_util_FifoInit( &g_UleMsgFifo[u32_Index],
							g_CMBS_UleMsgBuffer[u32_Index],
							(u16)sizeof(ST_FUN_MSG),
							FUN_MSG_FIFO_SIZE,
							HanServerCriticalSection);	// use same critical section for all devices

		WaitForReady[u32_Index] = FALSE;
	}
}
///////////////////////////////////////////////////////////////

PST_CMBS_FIFO Get_UleMsgFifo(u16 u16_deviceId)
{
		return &g_UleMsgFifo[u16_deviceId];
}

///////////////////////////////////////////////////////////////

void SetReleaseLink(u16 u16_deviceId, bool ReleaseLinkMode)
{
	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	ReleaseLink[u16_deviceId] = ReleaseLinkMode;
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);
}

bool GetReleaseLink(u16 u16_deviceId)
{
	bool ret;

	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	ret = ReleaseLink[u16_deviceId];
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);
	
	return ret;
}

///////////////////////////////////////////////////////////////

void SetWaitForReady(u16 u16_deviceId, bool Mode)
{
	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	WaitForReady[u16_deviceId] = Mode;
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);
}

bool GetWaitForReady(u16 u16_deviceId)
{
	bool ret;

	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	ret = WaitForReady[u16_deviceId];
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);

	return ret;
}
///////////////////////////////////////////////////////////////

void SetDuringDeleteReq(bool status)
{
	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	DuringDeleteReq = status;
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);
}

// This function protects the Delete Request: 
// Set only if not already during previous request
// return TRUE iff request was set now
bool SetIfNotAlreadyDuringDeleteReq(void)
{
	bool ret = FALSE;

	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	if (!DuringDeleteReq)
	{
		DuringDeleteReq=TRUE;
		ret = TRUE;
	}
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);

	return ret;
}

// return u16_deviceId, which may be also ALL_ULE_DEVICES_HOST
u16 app_HanPopPendingDeviceToDelete()
{
	int i;
	u16 u16_deviceId = NO_ULE_DEVICE_HOST;

	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);

	for (i = 0; i <= MAX_ULE_DEVICES_HOST; i++)	// 
	{
		if (PendingDeleteReqests[i].b_PendingRequest)
		{
			PendingDeleteReqests[i].b_PendingRequest = FALSE;
			if (i == MAX_ULE_DEVICES_HOST)
			{
				u16_deviceId = ALL_ULE_DEVICES_HOST;
			}
			else
			{
				u16_deviceId = i;

				if (PendingDeleteReqests[i].b_BlackListDeleteRequest)
				{
					PendingDeleteReqests[i].b_BlackListDeleteRequest = FALSE;
					
					u16_deviceId |= 0x8000;
				}
			}
			// anyhow break
			break;
		}
	}

	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);
	return u16_deviceId;
}

// input u16_deviceId may be ALL_ULE_DEVICES_HOST
void app_HanPushPendingDeviceToDelete(u16 u16_deviceId, bool b_BlackList)
{
	CFR_CMBS_ENTER_CRITICALSECTION(HanServerCriticalSection);
	if (u16_deviceId == ALL_ULE_DEVICES_HOST)
	{
		PendingDeleteReqests[MAX_ULE_DEVICES_HOST].b_PendingRequest = TRUE;
	}
	else
	{
		PendingDeleteReqests[u16_deviceId].b_PendingRequest= TRUE;
		
		PendingDeleteReqests[u16_deviceId].b_BlackListDeleteRequest = b_BlackList ? TRUE : FALSE;
	}
	CFR_CMBS_LEAVE_CRITICALSECTION(HanServerCriticalSection);
}

u8 app_HanDbGetFunMsgQSizeForDevice()
{
	return FUN_MSG_FIFO_SIZE;
}
