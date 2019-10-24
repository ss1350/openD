/*!
*   \file       appfun.h
*   \brief      HAN FUN API
*   \author     ULE BS Team
*
*
*
*******************************************************************************/

#if	!defined( _APPFUN_H )
#define	_APPFUN_H

#include "cmbs_api.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Methods												////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ==========  app_fun_ParseFUNRegistrationMessage  ===========
/*!
      \brief
       This function parses FUN registration msg received from device stIe_Msg
	and stores FUN registration data in pst_FunInfoEepromStructure
		
	void app_fun_ParseFUNRegistrationMessage(IN u8 *stIe_Msg, OUT PST_FUN_DEVICE_INFO pst_FunInfoEepromStructure)

      \param[in]		stIe_Msg 						    reference pointer to CMBS msg stores FUN Registration msg 

      \param[out]		pst_FunInfoEepromStructure      	    pointer to a struct used to store FUN registration data   

      \return           void

*/
void app_fun_ParseFUNRegistrationMessage(IN u8 *stIe_Msg, OUT PST_FUN_DEVICE_INFO pst_FunInfoEepromStructure);



// ==========  app_fun_BuildFUNRegistrationResponse  ===========
/*!
      \brief
       This function builds FUN registration msg response to be sent to adevice p_ResMsg and p_ResMsgCtl
		
	void app_fun_BuildFUNRegistrationResponse(IN u8 *p_Msg, OUT u8 *p_ResMsg, OUT u8 *p_ResMsgCtl)

      \param[in]		p_Msg				reference pointer CMBS msg stores FUN Registration msg
      
      \param[out]		p_ResMsg 			reference pointer to CMBS msg to store FUN Registration msg response
      \param[out]		p_ResMsgCtl      	   	reference pointer to CMBS control msg  FUN Registration msg control info

      \return           void

*/
void app_fun_BuildFUNRegistrationResponse(IN u8 *p_Msg, OUT u8 *p_ResMsg, OUT u8 *p_ResMsgCtl);

#endif	// _APPFUN_H 

/**********************[End Of File]**********************************************************************************************************/


/*---------[End Of File]---------------------------------------------------------------------------------------------------------------------------*/