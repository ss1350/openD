/*!
*  \file       appHanDb.h
*  \brief      Fifo message queue for CMBS host and CMBS target
*  \autor      HAN Team
*
*******************************************************************
*/

#if   !defined( _APPHANDB_H )
#define  _APPHANDB_H

#include "cmbs_fifo.h"


ST_CMBS_FIFO* app_HanDbGetUleMsgFifo();
bool app_HanDbPrintFifoStatistic(u16 u16_DeviceID);

typedef struct st_DeleteRequestQueue
{
	bool 	b_PendingRequest;
	bool	b_BlackListDeleteRequest;
}st_DeleteRequestQueue, *pst_DeleteRequestQueue;

#endif   // _APPHANDB_H

