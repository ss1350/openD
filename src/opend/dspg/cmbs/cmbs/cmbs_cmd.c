/*!
*  \file       cmbs_cmd.c
*  \brief      Implementation of the CMBS internal command protocol
*  \Author     kelbch
*
*  @(#)  %filespec: cmbs_cmd.c~DMZD53#16 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
* 13-Jan-2014 tcmc_asa  -- GIT--  take checksum changes from 3.46.x to 3_x main (3.5x)
* 20-Dec-13 tcmc_asa   GIT  Added CHECKSUM_SUPPORT    
* 16-Feb-09		Kelbch		1		Initialize \n
* 18-Feb-09    Kelbch      2     Update Version information on Hello Reply \n
* 18-Feb-09    Kelbch      3     Adapted Version information on Hello Reply \n
* 19-Feb-09    Kelbch      4     Extend hello-reply procedure to block conditions\n
* 19-Feb-09    Kelbch      5     Add flow control commands\n
* 09-Apr-09    Kelbch      161   Update of Media Configuration during start-up \n
*******************************************************************************/

#if defined(__arm)
#include "tclib.h"
#include "embedded.h"
#include <stdio.h>

#include "bsd09cnf.h"      /* component-globalal, system configuration */
#include "bsd09ddl.h"      /* messages and processes */
#include "bsd02eep.h"
#include "cdr03eep.h"

#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include "cmbs_int.h"      // CMBS internal API structure and defines
#include "cfr_debug.h"     // CMBS debug definition

ST_CAPABLITIES    g_st_CMBSCapabilities;

#if defined ( CMBS_API_TARGET )
	#include "cfr_uart.h"
	#include "tapp.h"
	#include "tapp_log.h"
	extern void _cfr_uartTransmitterReset( void );
	extern void p_cmbs14_SuspendTxRes(u8 u8_Response);
	extern void p_cmbs14_ResumeTxRes(u8 u8_Response);
	extern ST_CMBS_APP g_CMBSApp;

#if defined (CRASH_DUMP)
	extern bool tapp_dsr_IsCrashDumpAvailable(void);
#endif

#if defined (CRASH_DUMP)
#include "csys0reg.h"
#include "csys2vpb.h"
#include "csys5os.h"
#endif

#endif


	extern void tapp_util_ConfigureUARTFlowCTRL(E_CMBS_FLOW_CTRL flowCTRL);
    
void              _cmbs_int_cmd_CapabilitiesReply(u8 *pu8_Buffer);
void              _cmbs_int_cmd_Capabilities(u8 *pu8_Buffer);
void              _cmbs_int_cmd_StoreRAMDump(void);
int InitDuslicXS(void);
int SLIC_init(void);
//		========== _cmbs_int_cmd_Reset ===========
/*!
		\brief				 reset the communication module
		\param[in]		 < none >
		\return			 < none >
*/
void  _cmbs_int_cmd_Reset( void )
{
#ifdef OS_TASK_CONTROL
   extern volatile u32 G_u32_tapp_ParamAreaGetRspFlag;
   extern volatile u32 G_u32_tapp_ParamAreaGetRspOffset;
   extern volatile u32 G_u32_tapp_ParamAreaGetRspDataLen;
#endif // OS_TASK_CONTROL

   CFR_DBG_OUT ( "CMBS-API: Reset communication module \n" ) ;
#if defined ( CMBS_API_TARGET )
   cfr_uartInitalize ( );
#endif
   g_CMBSInstance.e_DestFlowState = E_CMBS_FLOW_STATE_GO;
   g_CMBSInstance.e_OrigFlowState = E_CMBS_FLOW_STATE_GO;

#ifdef OS_TASK_CONTROL
   G_u32_tapp_ParamAreaGetRspFlag = 0;
   G_u32_tapp_ParamAreaGetRspOffset = 0;
   G_u32_tapp_ParamAreaGetRspDataLen = 0;
#endif // OS_TASK_CONTROL
}

//		========== _cmbs_int_cmd_Hello ===========
/*!
		\brief				 reset communication buffer and reply to host, ready to go.

		\param[in,out]		 pu8_Buffer		 pointer to buffer, currently NULL, u16_Size size of buffer

		\return				 < none >

*/
extern void  cmbs_applNewTDMReconfigure ( u8 u8_Master, u8 u8_Speed, u8 u8_Sync, u16 u16_SlotEnable, u8 u8_Loop );

void _cmbs_int_cmd_Hello( u8 * pu8_Buffer, u16 u16_Size )
{
#if defined ( CMBS_API_TARGET )
   u8 u8_Buffer[5];
#if !defined( CMBS_USB_AUDIO )
   u8 u8_Master = FALSE, u8_Loop = FALSE;
#endif

   if( pu8_Buffer )
   {
      CFR_DBG_OUT ( "CMBS-API: Receive Hello message with Media Information %02x %02x %02x\n",
                    pu8_Buffer[0],
                    pu8_Buffer[1],
                    pu8_Buffer[2]
                  );

#if !defined( CMBS_USB_AUDIO )
      switch ( pu8_Buffer[0])
      {
         case CMBS_TDM_TYPE_MASTER:
            u8_Master = TRUE;
            u8_Loop = FALSE;
            break;
         case CMBS_TDM_TYPE_SLAVE:
            u8_Master = FALSE;
            u8_Loop = FALSE;
            break;
         case CMBS_TDM_TYPE_MASTER_LOOP:
            u8_Master = TRUE;
            u8_Loop = TRUE;
            break;
         case CMBS_TDM_TYPE_SLAVE_LOOP:
            u8_Master = FALSE;
            u8_Loop = TRUE;
            break;
      }
#ifndef NEW_TDM
      cmbs_applTDMReconfigure( u8_Master, pu8_Buffer[1], pu8_Buffer[2], 0xFFFF, u8_Loop );
#else
      cmbs_applNewTDMReconfigure( u8_Master, pu8_Buffer[1], pu8_Buffer[2], 0xFFFF, u8_Loop );
#endif //NEW_TDM
#endif //!defined( CMBS_USB_AUDIO )
			
		if (u16_Size > LEGACY_CMD_HELLO_LENGTH)	//means we also received UART Flow control configuration	
	{ 
#if (defined (UART) && !defined(USB)) || defined (CMBS_UART) 
	   CFR_DBG_OUT ( "CMBS-API: Receive Hello message with UART Configuration %02x\n", pu8_Buffer[5]);
		tapp_util_ConfigureUARTFlowCTRL(pu8_Buffer[5]);
#endif
	}

   }
   else
   {
      CFR_DBG_OUT ( "CMBS-API: Receive Hello message without Media Information\n" );
   }

	// (Re-)Init transmit (to avoid problem occurred with USB, when old info is still in buffer when serial port is closed)
   _cfr_uartTransmitterReset();

   _cmbs_int_cmd_Reset();

   cfr_ie_ser_u16( u8_Buffer, CMBS_API_TARGET_VERSION );
   cfr_ie_ser_u16( u8_Buffer+sizeof(u16),CMBS_TARGET_BUILD );

   u8_Buffer[4] = g_CMBSInstance.e_Mode;

#ifdef QSPIC_EEPROM
	p_dr13_EEPONF_DisableFlashing(); // should call p_cmbs14_ResumeTxRes instead ?
#endif

#ifdef CMBS_TARGET_UP_RETRANSMISSION
	tapp_dsr_UpdateHelloArrived(true);
#endif 

    tapp_util_Release_All_External_Calls(&g_CMBSApp);	// If any Open Call

   cmbs_int_cmd_Send( (u8)CMBS_CMD_HELLO_RPLY, u8_Buffer, sizeof(u8_Buffer) );
#else
	UNUSED_PARAMETER(u16_Size);
#endif // CMBS_API_TARGET



	UNUSED_PARAMETER( pu8_Buffer );
}

//		========== _cmbs_int_cmd_HelloRply ===========
/*!
		\brief				 target reply received, get version info of target

		\param[in,out]		 pu8_Buffer		 pointer to parameter, e.g. version info,etc.

		\return				< none >

*/

void  _cmbs_int_cmd_HelloRply( u8 * pu8_Buffer )
{
#if !defined ( CMBS_API_TARGET )
   u16 u16_Version;

   cfr_ie_dser_u16 (pu8_Buffer, &u16_Version );

   g_CMBSInstance.u16_TargetVersion = u16_Version;

   if(u16_Version & 0xF000)
   {
        CFR_DBG_OUT( "TARGET API version: %x.%02x.%x\n", (u16_Version>>12),((u16_Version & 0xFF0)>>4), (u16_Version & 0xF) );
   }
   else
   {
        CFR_DBG_OUT( "TARGET API version: %02x.%02x\n", (u16_Version>>8),(u16_Version &0xFF) );
   }
   
   cfr_ie_dser_u16 (pu8_Buffer+2, &u16_Version );
   g_CMBSInstance.u16_TargetBuild   = u16_Version;
   CFR_DBG_OUT( "TARGET BUILD      : %02x.%02x\n", (u16_Version>>8),(u16_Version &0xFF) );

   CFR_DBG_OUT( "TARGET mode       : %d\n", pu8_Buffer[5] );
   
    if ( g_CMBSInstance.u16_TargetVersion == 0x0001 )
    {   
        // bootloader does not support checksum
        g_st_CMBSCapabilities.u8_Checksum = 0;
    }
    
   _cmbs_int_StartupBlockSignal( &g_CMBSInstance );
#endif
}

//		========== cmbs_int_cmd_FlowNOK ===========
/*!
		\brief				 destination side signal that it's not able to receive any packets.
                         stop transmission for destination.

		\param[in,out]		 pu8_Buffer    pointer to paramete, e.g. last received packet number

		\return				 <none>

*/

void  _cmbs_int_cmd_FlowNOK ( u8 * pu8_Buffer )
{
   u16 u16_Packet;

   cfr_ie_dser_u16 (pu8_Buffer, &u16_Packet );
   g_CMBSInstance.e_DestFlowState = E_CMBS_FLOW_STATE_STOP;
#if !defined ( CMBS_TARGET_API )
   CFR_DBG_WARN( "CMBS-API: Target side is busy, can't receive any more packets\n" );
#endif
}

//		========== cmbs_int_cmd_FlowRestart ===========
/*!
		\brief				 destination side signal that it's able to receive packets.
                         enable transmission for destination.

		\param[in,out]		 pu8_Buffer    pointer to paramete, e.g. last received packet number

		\return				 <none>

*/

void  _cmbs_int_cmd_FlowRestart ( u8 * pu8_Buffer )
{
   u16 u16_Packet;

   cfr_ie_dser_u16 (pu8_Buffer, &u16_Packet );
   g_CMBSInstance.e_DestFlowState = E_CMBS_FLOW_STATE_GO;
#if !defined ( CMBS_TARGET_API )
   CFR_DBG_WARN( "CMBS-API: Target side is free, can receive packets again\n" );
#endif
}

//		========== cmbs_int_cmd_Dispatcher ===========
/*!
		\brief				 dispatch internal CMBS commands

		\param[in]		 u8_Cmd		 internal CMBS Command ID

		\param[in]		 pu8_Buffer	 pointer to parameter

		\param[in]		 u16_Size	 size of parameter

		\return			<none>

*/

void  cmbs_int_cmd_Dispatcher( u8 u8_Cmd, u8 * pu8_Buffer, u16 u16_Size )
{
   if( u16_Size ){}   // unused u16_Size; eliminate "unused parameter" compiler warning

   switch ( u8_Cmd )
   {
#if defined ( CMBS_API_TARGET )
      // start up command of host to target
      case CMBS_CMD_HELLO:
         /// Hello buffer configure also the IOM channel
         if ( u16_Size )
            _cmbs_int_cmd_Hello( pu8_Buffer, u16_Size );
         else
            _cmbs_int_cmd_Hello( NULL, 0 );

 
        // preset with 0, maybe a different host now.
        g_st_CMBSCapabilities.u8_Checksum = 0;

        break;
	 case CMBS_CMD_FLASH_START_RES:
	 	  p_cmbs14_SuspendTxRes( 1 );
	 	 break;
		 
	 case CMBS_CMD_FLASH_STOP_RES:
	 	  p_cmbs14_ResumeTxRes( 1 );
		 break;

        // checksum command of host to target
     case CMBS_CMD_CAPABILITIES:
         /// Capabilities buffer configure also the IOM channel
         if ( u16_Size )
             _cmbs_int_cmd_Capabilities(pu8_Buffer);
         else
             _cmbs_int_cmd_Capabilities(NULL);

         // send own Capabilities in a reply message
         cmbs_int_cmd_SendCapablitiesReply();

        break;
        // checksum command of host to target
     case CMBS_CMD_STORE_RAM_DUMP:
         /// To Forcefully reset the CMBS to capture the RAM Dump
         _cmbs_int_cmd_StoreRAMDump();
        break;

#endif
#if !defined ( CMBS_API_TARGET )
      // start up reply command from target to host
      case CMBS_CMD_HELLO_RPLY:
         _cmbs_int_cmd_HelloRply( pu8_Buffer );
         break;
	  case CMBS_CMD_FLASH_START_REQ:
		  _cmbs_int_SuspendTxCommands();
		  break;
	  case CMBS_CMD_FLASH_STOP_REQ:
		  _cmbs_int_ResumeTxCommands();
		  break;
        // capabilities reply command from target to host
      case CMBS_CMD_CAPABILITIES_RPLY:
          _cmbs_int_cmd_CapabilitiesReply(pu8_Buffer);
          break;
#endif

      case CMBS_CMD_FLOW_NOK:
      // destination side is busy and not able to receive any more packets
      _cmbs_int_cmd_FlowNOK( pu8_Buffer );
        break;

      case CMBS_CMD_FLOW_RESTART:
      // destination side is free and restart to receive packets
      _cmbs_int_cmd_FlowRestart(pu8_Buffer );
        break;

      case CMBS_CMD_RESET:
      _cmbs_int_cmd_Reset();
         break;

      default:
         CFR_DBG_WARN( "ProtCMD Dispatcher: %d is not implemented, yet!\n", u8_Cmd );
   }
}


u32 cmbs_cmd_parser( CMBS_CMD id, u8* pu8_Buffer, u8* pu8_Buffer_parse, u32 u32_Pos)	
{
	switch ( id )
	{
		case CMBS_CMD_HELLO:		
			// byte 0 = TDM config
			switch(pu8_Buffer[0])
				{
					case CMBS_TDM_TYPE_MASTER: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n TDM config = %s ", "CMBS_TDM_TYPE_MASTER");
					break;
					case CMBS_TDM_TYPE_SLAVE: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n TDM config = %s ", "CMBS_TDM_TYPE_SLAVE");
					break;
					case CMBS_TDM_TYPE_MASTER_LOOP: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n TDM config = %s ", "CMBS_TDM_TYPE_MASTER_LOOP");
					break;
					case CMBS_TDM_TYPE_SLAVE_LOOP: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n TDM config = %s ", "CMBS_TDM_TYPE_SLAVE_LOOP");	
					break;
				}
			// byte 1 = speed config
			switch(pu8_Buffer[1])
				{
					case CMBS_TDM_PCM_512: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_PCM_512");
					break;
					case CMBS_TDM_PCM_768: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_PCM_768");
					break;
					case CMBS_TDM_PCM_1536: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_PCM_1536");
					break;
					case CMBS_TDM_PCM_2048: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_PCM_2048");	
					break;
					case CMBS_TDM_IOM_512: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_IOM_512");
					break;
					case CMBS_TDM_IOM_768: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_IOM_768");
					break;
					case CMBS_TDM_IOM_1536: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_IOM_1536");
					break;
					case CMBS_TDM_IOM_2048: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n speed config = %s ", "CMBS_TDM_IOM_2048");	
					break;					
				}
			// byte 2 = sync config
			switch(pu8_Buffer[2])
				{
					case CMBS_TDM_SYNC_SHORT_FR: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n sync config = %s ", "CMBS_TDM_SYNC_SHORT_FR");
					break;
					case CMBS_TDM_SYNC_SHORT_FF: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n sync config = %s ", "CMBS_TDM_SYNC_SHORT_FF");
					break;
					case CMBS_TDM_SYNC_SHORT_LF: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n sync config = %s ", "CMBS_TDM_SYNC_SHORT_LF");
					break;
					case CMBS_TDM_SYNC_LONG: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n sync config = %s ", "CMBS_TDM_SYNC_LONG");	
					break;					
				}

			u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos), "\n SlotEnable =  0x%04x", (pu8_Buffer[4] << 8) | pu8_Buffer[3]);

			switch(pu8_Buffer[5])
				{
					case CMBS_FLOW_CTRL_NONE: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n device type = %s ", "CMBS_FLOW_CTRL_NONE");
					break;
					case CMBS_FLOW_CTRL_XON_XOFF: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n device type = %s ", "CMBS_FLOW_CTRL_XON_XOFF");
					break;
					case CMBS_FLOW_CTRL_RTS_CTS: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n device type = %s ", "CMBS_FLOW_CTRL_RTS_CTS");
					break;
					case CMBS_FLOW_CTRL_DSR_DTR: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n device type = %s ", "CMBS_FLOW_CTRL_DSR_DTR");	
					break;					
				}
		break;

		case CMBS_CMD_HELLO_RPLY:
			u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos), "\n CMBS_API_TARGET_VERSION = 0x%04x", (pu8_Buffer[1] << 8) | pu8_Buffer[0]);
			u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos), "\n CMBS_TARGET_BUILD = 0x%04x ", (pu8_Buffer[3]<<8) | pu8_Buffer[2]);
			switch(pu8_Buffer[4])
				{
					case CMBS_MODE_MLB: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n mode = %s ", "CMBS_MODE_MLB");
					break;
					case CMBS_MODE_AMLB: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n mode = %s ", "CMBS_MODE_AMLB");
					break;
					case CMBS_MODE_DECT_PIPE: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n mode = %s ", "CMBS_MODE_DECT_PIPE");
					break;
					case CMBS_MODE_DECT_MAC_PIPE: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n mode = %s ", "CMBS_MODE_DECT_MAC_PIPE");	
					break;					
					case CMBS_MODE_MAX: u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n mode = %s ", "CMBS_MODE_MAX");	
					break;					
				}
		break;

		
		case CMBS_CMD_CAPABILITIES:
		case CMBS_CMD_CAPABILITIES_RPLY:
			if(pu8_Buffer[4] & 1)
				{
					u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n  %s ", "CHECKSUM_SUPPORT");
				}
				else
				{
					u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos),"\n  %s ", "CHECKSUM_NOT_SUPPORT");
				}
					
		break;

		case CMBS_CMD_FLOW_NOK:
		case CMBS_CMD_FLOW_RESTART:		
			u32_Pos+=sprintf((char*)(pu8_Buffer_parse+u32_Pos), "\n packet = 0x%04x", (pu8_Buffer[0] << 8) | pu8_Buffer[1]);
		break;		

		case CMBS_CMD_FLASH_START_RES:
		case CMBS_CMD_FLASH_STOP_RES:	
		case CMBS_CMD_FLASH_START_REQ:
		case CMBS_CMD_FLASH_STOP_REQ:
		case CMBS_CMD_RESET:
		case CMBS_CMD_STORE_RAM_DUMP:
	 	break;	
	 	
	}
	return u32_Pos	        ;
}

//		========== cmbs_int_cmd_FlowNOKHandle  ===========
/*!
		\brief				 send information to other side to stop transmission

		\param[in,out]		 void

		\return				< none >

*/

void  cmbs_int_cmd_FlowNOKHandle ( u16 u16_Packet )
{
   u8 u8_Buffer[2];

   if ( g_CMBSInstance.e_OrigFlowState == E_CMBS_FLOW_STATE_GO )
   {
      g_CMBSInstance.e_OrigFlowState = E_CMBS_FLOW_STATE_STOP;

      cfr_ie_ser_u16 ( u8_Buffer, u16_Packet );

      cmbs_int_cmd_Send( (u8)CMBS_CMD_FLOW_NOK, u8_Buffer, sizeof(u8_Buffer) );

   }

}

//		========== cmbs_int_cmd_FlowRestartHandle  ===========
/*!
		\brief				 send information to other side to re-enable transmission

		\param[in,out]		 void

		\return				void     cmbs_int_cmdFlowStop			 description

*/

void  cmbs_int_cmd_FlowRestartHandle ( u16 u16_Packet )
{
   u8 u8_Buffer[2];

   if ( g_CMBSInstance.e_OrigFlowState == E_CMBS_FLOW_STATE_STOP )
   {
      g_CMBSInstance.e_OrigFlowState = E_CMBS_FLOW_STATE_GO;
      cfr_ie_ser_u16 ( u8_Buffer, u16_Packet );

      cmbs_int_cmd_Send( (u8)CMBS_CMD_FLOW_RESTART, u8_Buffer, sizeof(u8_Buffer) );
   }
}

void  cmbs_int_cmd_ReceiveEarly ( u8 * pu8_Buffer )
{
   PST_CMBS_SER_MSG p_Mssg = (PST_CMBS_SER_MSG)pu8_Buffer;
//   int i;

//   CFR_DBG_OUT ( "CMBS-API: message Early Receive\n" );
//   CFR_DBG_OUT ( "CMBS-API: packet => ");
// 	for ( i=0; i < 6; i++ )
// 		CFR_DBG_OUT ( "%02x ", pu8_Buffer[i] );
// 	CFR_DBG_OUT ( "\n" );

  if ( (p_Mssg->st_MsgHdr.u16_EventID & CMBS_CMD_MASK) == CMBS_CMD_MASK)
  {
//      cmbs_dbg_CmdTrace ( pu8_Buffer, 0 );

      cmbs_int_cmd_Dispatcher( (u8)p_Mssg->st_MsgHdr.u16_EventID, (u8*)p_Mssg->u8_Param, 0 );
  }
}

u8    cmbs_int_cmd_FlowStateGet ( void )
{
   return (u8)g_CMBSInstance.e_DestFlowState;
}
//		========== _cmbs_int_cmd_Capabilities ===========
/*!
        \brief				 Capabilities of the Host received, e.g. checksum.

        \param[in,out]		 pu8_Buffer		 pointer to buffer, currently NULL

        \return				 < none >

*/

void              _cmbs_int_cmd_Capabilities(u8 *pu8_Buffer)
{
#if defined ( CMBS_API_TARGET )
    // u8 u8_Buffer[5];
    if ( pu8_Buffer )
    {
        // Curretnyl 4 Bytes of capabilities are sent, but it could be extented for future use
        // first byte is the length.
        if ( pu8_Buffer[4] & CMBS_CAPABILITY_MASK )
        {
            // currently the only supported capability
            // Store this capability
            g_st_CMBSCapabilities.u8_Checksum = 1;
        }
        else
        {
            g_st_CMBSCapabilities.u8_Checksum = 0;
        }
    }
#endif // CMBS_API_TARGET

  if( pu8_Buffer ){}   // pu8_Buffer irrelevat here; eliminate "unused parameter" compiler warning
}



//		========== _cmbs_int_cmd_CapabilitiesReply ===========
/*!
        \brief				 Capabilities of the Target received, e.g. checksum.

        \param[in,out]		 pu8_Buffer		 pointer to buffer, currently NULL

        \return				 < none >

*/

void _cmbs_int_cmd_CapabilitiesReply(u8 *pu8_Buffer)
{
#if !defined ( CMBS_API_TARGET )
    if ( pu8_Buffer )
    {
        // Curretnyl 4 Bytes of capabilities are sent, but it could be extented for future use
        // first byte is the length.
        if ( pu8_Buffer[4] & CMBS_CAPABILITY_MASK )
        {
            // curretnlyx the only supported capability
            // Store this capability
            g_st_CMBSCapabilities.u8_Checksum = 1;
            CFR_DBG_INFO("\n CHECKSUM_SET received\n");
        }
    }

    // notify upper layer
    _cmbs_int_StartupBlockSignal(&g_CMBSInstance);
#endif // CMBS_API_TARGET

  if( pu8_Buffer ){}   // pu8_Buffer irrelevat here; eliminate "unused parameter" compiler warning
}

//		========== _cmbs_int_cmd_StoreRAMDump ===========
/*!
        \brief				 To forcefully reset the CMBS to capture RAM Dump

        \param[in,out]		 < none >

        \return				 < none >
*/

void              _cmbs_int_cmd_StoreRAMDump(void)
{
#if defined ( CMBS_API_TARGET ) && defined ( CRASH_DUMP )
	typedef void (*pfn_Crash)(void);
	
		{
			pfn_Crash p_Crashfn;

			p_Crashfn = (pfn_Crash)0xFFFFFFFF;
			p_Crashfn();
		}

	
	// wait for WD reset
	while (1);
#endif // CMBS_API_TARGET
}

//*/
