/*!
*  \file       cmbs_voipline_Phoner.h
*  \brief
*  \Author     andriig
*
*  @(#)  %filespec: cmbs_voipline_phoner.h~3 %
*
*  This API is provided for DEMO purposes
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                          \n
*  ----------------------------------------------------------------------------\n
*******************************************************************************/

#if   !defined( CMBS_VOIPLINE_Media5_H )
#define  CMBS_VOIPLINE_Media5_H

#if defined( __cplusplus )
extern "C"
{
#endif

#include "cmbs_voipline.h"
#include "appCallRouter.h"

// Load Phoner DLL and register callbacks
EXTAPP_RC LoadMedia5Lib();


// Unload Phoner DLL
void UnLoadMedia5DLL();

// set Phoner call
EXTVOIP_RC voipapp_Media5_SetupCall(IN int cmbsCallID, IN const char* calledID, IN int callerHandset, IN int lineID, IN EXTVOIP_CODEC* codecList, IN int codecListLength);

// On Incoming Phoner call
EXTVOIP_RC voipapp_Media5_OnSetupCall(IN int calledHandset, IN const char* callerNumber, IN const char* callerName, IN EXTVOIP_CODEC* codecList, IN int codecListLength, OUT int* cmbsCallID);


void TestAppCallback1 ();
void TestAppCallback2 ();
void TestAppCallback3 ();


#if defined( __cplusplus )
}
#endif

#endif   // CMBS_VOIPLINE_Phoner_H
//*/
