/*!
*  \file       cfr_uart.c
*  \brief      UART implementation of linux host side
*  \author     stein
*
*  @(#)  %filespec: cfr_uart.c~DMZD53#4.1.2 %
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                          \n
*  ----------------------------------------------------------------------------\n
*  14-feb-09   R.Stein    1         Initialize \n
*******************************************************************************/

#include <stdlib.h>
#include <stddef.h>  // for offsetof
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h> // we need <sys/select.h>; should be included in <sys/types.h> ???
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "cmbs_int.h"   /* internal API structure and defines */
#include "cfr_uart.h"   /* packet handler */
#include "cfr_debug.h"  /* debug handling */

//    ========== cfr_uartThread ===========
/*!
      \brief            UART data receive pump. if data is available a message is send
                        to cfr_cmbs task.

      \param[in]        pVoid          pointer to CMBS instance object

      \return           <void *>       return always NULL

*/

void  *           cfr_uartThread( void * pVoid )
{
   PST_CMBS_API_INST
                  pInstance = (PST_CMBS_API_INST)pVoid;
   int            fdDevCtl  = pInstance->fdDevCtl;
   int            msgQId    = pInstance->msgQId;
   fd_set         input_fdset;
	ST_CMBS_LIN_MSG LinMsg;
	int retval;

//   CFR_DBG_OUT( "UART Thread: ID:%lu running\n", (unsigned long)pthread_self() );

	/* 
	Never ending loop.
      Thread will be exited automatically when parent thread finishes.
   */
   while( 1 )
   {
      FD_ZERO( &input_fdset );
      FD_SET( fdDevCtl, &input_fdset);

		/*
		select() returns 0 if timeout, 1 if input available, -1 if error.
		
		A signal can arrive and be handled while an I/O primitive such as select(), open() or read() is waiting for an I/O device.
		When select() returned error of EINTR, it is means that this call did not succeed because it was interrupted.
		However, if you try again, it will probably work.
		
		In other words, EINTR is not a fatal error - it just means you should retry select().
		*/
		retval = select(fdDevCtl+1, &input_fdset, NULL, NULL, NULL);
		if (-1 == retval)
		{
			if (EINTR == errno)
      {
				return NULL;
			}
			
			CFR_DBG_ERROR( "UartThread Error: select() failed with error %d\n",errno );
       /*!\todo exception handling is needed !*/
      }
		else if (0 == retval)
		{
			CFR_DBG_ERROR( "UartThread Error: select() failed on timeout error\n");
		}
      else
      {
         if( FD_ISSET(fdDevCtl, &input_fdset) )
         {
            //memset( &LinMsg.msgData, 0, sizeof(LinMsg.msgData) );

            /* Reading available data from serial interface */
            if( (LinMsg.msgData.nLength = read(fdDevCtl, LinMsg.msgData.u8_Data, sizeof(LinMsg.msgData.u8_Data))) <= 0 )
            {
				CFR_DBG_ERROR( "UartThread Error: read() failed len=%d\n", LinMsg.msgData.nLength);

#if SU_XHOST
#if SU_XHOST_USB
				// Detected disconnection of USB, sending CMBS_EV_DSR_TARGET_UP message to try and reconnect
				LinMsg.msgType = 1;
				LinMsg.msgData.nLength = 12;
				LinMsg.msgData.u8_Data[0] = 0xDA;
				LinMsg.msgData.u8_Data[1] = 0xDA;
				LinMsg.msgData.u8_Data[2] = 0xDA;
				LinMsg.msgData.u8_Data[3] = 0xDA;
				LinMsg.msgData.u8_Data[4] = 0x08;
				LinMsg.msgData.u8_Data[5] = 0x00;
				LinMsg.msgData.u8_Data[6] = 0x00;
				LinMsg.msgData.u8_Data[7] = 0x00;
				LinMsg.msgData.u8_Data[8] = 0x7D;
				LinMsg.msgData.u8_Data[9] = 0x00;
				LinMsg.msgData.u8_Data[10] = 0x00;
				LinMsg.msgData.u8_Data[11] = 0x00;

				if( msgsnd( msgQId, &LinMsg, (LinMsg.msgData.nLength + sizeof(LinMsg.msgData.nLength)), 0 ) < 0 )
				{
			       	CFR_DBG_ERROR( "UartThread Error: reconnect msgsnd ERROR:%d\n", errno );
			    }
				return NULL;
#endif
#endif //SU_XHOST

            }
            else
            {
//               CFR_DBG_OUT( "UartThread: received %d bytes\n", LinMsg.msgData.nLength );

               /* Send what we received to callback thread */
               if( msgQId >= 0 )
               {
                  LinMsg.msgType = 1;
//                  CFR_DBG_OUT( "UartThread: sent %d bytes\n", LinMsg.msgData.nLength + 4 );
                  if( msgsnd( msgQId, &LinMsg, (LinMsg.msgData.nLength + sizeof(LinMsg.msgData.nLength)), 0 ) < 0 )
                  {
                     CFR_DBG_ERROR( "UartThread: msgsnd ERROR:%d\n", errno );
                  }
                  else
                  {
//                     CFR_DBG_OUT( "UartThread: msgsnd sent ok.\n");
                  }
               }
               else
               {
                  CFR_DBG_ERROR( "UartThread: invalid msgQId:%d\n", msgQId );
               }
            }
         }
      }
   }

   return NULL;
}


//    ==========    cfr_wait_till_characters_transmitted ===========
/*!
      \brief             Wait till characters are transmitted

      \param[in,out]     fd    pointer to packet part

      \return            < none >

*/

void cfr_uartWaitPacketPartTransmitFinished(int fd)
{
   int rv;
   fd_set writefds;

   while(1)
   {
      FD_ZERO(&writefds);
      FD_SET(fd, &writefds);

      rv=select(fd+1, NULL, &writefds, NULL, NULL);

      if (rv >= 0) break;
      if (errno != EINTR) break;
   }
}

//    ==========    cfr_uartPacketPartWrite ===========
/*!
      \brief             write partly the packet into communication device

      \param[in,out]     pu8_Buffer    pointer to packet part

      \param[in,out]     u16_Size      size of packet part

      \return            < int >       currently, alway 0

*/

int               cfr_uartPacketPartWrite( u8* pu8_Buffer, u16 u16_Size  )
{
   // int    i;

   // CFR_DBG_OUT( "PacketPartWrite: " );
   // for (i=0; i < u16_Size; i++ )
   // {
   //   CFR_DBG_OUT( "%02x ",pu8_Buffer[i] );
   //   write( g_CMBSInstance.fdDevCtl, pu8_Buffer + i, 1 );
   // }
   // CFR_DBG_OUT( "\n" );

   write( g_CMBSInstance.fdDevCtl, pu8_Buffer, u16_Size);

   cfr_uartWaitPacketPartTransmitFinished(g_CMBSInstance.fdDevCtl);

   return 0;
}
//    ========== cfr_uartPacketWriteFinish ===========
/*!
      \brief             Currently dummy function is not needed on host side

      \param[in,out]     u8_BufferIDX  buffer index

      \return            < none >

*/

void              cfr_uartPacketWriteFinish( u8 u8_BufferIDX )
{
   // For logging sent data packets
   UNUSED_PARAMETER(u8_BufferIDX);   
}

//    ========== cfr_uartPacketPrepare  ===========
/*!
      \brief             Currently dummy function is not needed on host side

      \param[in,out]     u16_size      size of to be submitted packet for transmission

      \return           < CFR_E_RETVAL >

*/

CFR_E_RETVAL      cfr_uartPacketPrepare( u16 u16_size )
{
   // dummy function
   UNUSED_PARAMETER(u16_size);
   
   return CFR_E_RETVAL_OK;
}

//    ========== cfr_uartDataTransmitKick  ===========
/*!
      \brief             Currently dummy function is not needed on host side

      \param[in,out]     < none >

      \return           < CFR_E_RETVAL >

*/

void              cfr_uartDataTransmitKick( void )
{
   // dummy function
}

//    ========== cfr_uartInitialize ===========
/*!
      \brief             open the serial communication interface with relevant parameter sets

      \param[in,out]     pst_Config    pointer to UART configuration

      \return           < int >        if failed returns -1, otherwise 0

*/

int          cfr_uartInitialize( PST_UART_CONFIG pst_Config )
{
   char           szDevName[128] = {0};
   int            fd;
   struct termios term_attr;
   unsigned int baud = B115200; /* defaults */
   // generate device name
   if( pst_Config->psz_ComDevName )
   {
      sprintf( szDevName, "/dev/%s", pst_Config->psz_ComDevName );
   }
   else if( pst_Config->u8_Port < 1 )
   {
//      CFR_DBG_WARN( "Warning: Invalid port COM%d. Using default COM1\n", pst_Config->u8_Port );
      sprintf( szDevName, "/dev/ttyS0" );
   }
   else
   {
     sprintf( szDevName, "/dev/ttyS%d", pst_Config->u8_Port );
   }
   // open device
   if( (fd = open(szDevName, O_RDWR | O_NOCTTY | O_NDELAY)) == -1 )
   {
      CFR_DBG_ERROR( "Error: Can't open device %s\n", szDevName );
      return -1;
   }
   else
   {
      CFR_DBG_OUT( "Opened %s\n", szDevName );

      /* Configure terminal attributes */
      if( tcgetattr(fd, &term_attr) != 0 )
      {
         CFR_DBG_ERROR( "Error: tcgetattr() for TERM_DEVICE failed\n" );
       close ( fd );
         return -1;
      }
     /* setup the serial device communication properties */
      term_attr.c_cflag = CS8|CREAD|CLOCAL;//|CRTSCTS;
                                                               /* Control flag
                                                               CS8     : 8n1 (8bit,no parity,1 stopbit)
                                                               CREAD   : enable receiving characters
                                                               CLOCAL  : local connection, no modem control
                                                               CRTSCTS : output hardware flow control
                                                               */
      /* Raw data transmission; No pre- or post-processing */
      term_attr.c_iflag = 0;                                   // Input flag
      term_attr.c_oflag = 0;                                   // Output flag
      term_attr.c_lflag = 0;                                   // Local flag

	  switch(pst_Config->u32_BaudRate)
	  {
	  	case 4800:
			baud = B4800;
			break;
	  	case 9600:
			baud = B9600;
			break;
		case 19200:
			baud = B19200;
			break;
		case 57600:
			baud = B57600;
			break;
		case 115200:
			baud = B115200;
			break;
		case 230400:
			baud = B230400;
			break;
		
		default :
			baud = B115200;
			break;
	  }

      if( cfsetispeed(&term_attr, baud) == -1 )
      {
         CFR_DBG_ERROR( "Error: failed to set input baud rate\n" );
         close (fd);
       return -1;
      }

      if( cfsetospeed(&term_attr, baud) == -1 )
      {
         CFR_DBG_ERROR( "Error: failed to set output baud rate\n" );
         close (fd);
       return -1;
      }

      if( tcsetattr(fd, TCSAFLUSH, &term_attr) != 0 )
      {
         CFR_DBG_ERROR( "Error: tcsetattr() for TERM_DEVICE failed\n" );
         close (fd);
       return -1;
      }
   }

   return   fd;
}


//todo: rename ST_UART_CONFIG to ST_SERIAL_CONFIG
int          cfr_usbInitialize( PST_UART_CONFIG pst_Config )
{
	char           szDevName[128];
	int            fd;
	struct termios term_attr;
	unsigned int baud = B115200; /* defaults */

   if(pst_Config->psz_ComDevName)
   {
      sprintf( szDevName, "/dev/%s", pst_Config->psz_ComDevName );
      fd = open(szDevName, O_RDWR | O_NOCTTY | O_NDELAY);
   }
   else
   {
      // generate device name /dev/ttyACM
      sprintf( szDevName, "/dev/ttyACM%d", pst_Config->u8_Port );
      // open device
      fd = open(szDevName, O_RDWR | O_NOCTTY | O_NDELAY);

      if(fd == -1)
      {
         // generate device name /dev/ttyUSB
         sprintf( szDevName, "/dev/ttyUSB%d", pst_Config->u8_Port );
         // open device
         fd = open(szDevName, O_RDWR | O_NOCTTY | O_NDELAY);
      }
   }

	if(fd == -1)
	{
		CFR_DBG_ERROR( "Error: Can't open device /dev/ttyACM%d or /dev/ttyUSB%d\n", pst_Config->u8_Port, pst_Config->u8_Port );
		return -1;
	}

	CFR_DBG_OUT( "Opened %s\n", szDevName );

	/* Configure terminal attributes */
	if( tcgetattr(fd, &term_attr) != 0 )
	{
		CFR_DBG_ERROR( "Error: tcgetattr() for TERM_DEVICE failed\n" );
		close ( fd );
		return -1;
	}
    
	/* setup the serial device communication properties */
	term_attr.c_cflag = CS8|CREAD|CLOCAL;//|CRTSCTS;
                                                               /* Control flag
                                                               CS8     : 8n1 (8bit,no parity,1 stopbit)
                                                               CREAD   : enable receiving characters
                                                               CLOCAL  : local connection, no modem control
                                                               CRTSCTS : output hardware flow control
                                                               */
	/* Raw data transmission; No pre- or post-processing */
	term_attr.c_iflag = 0;                                   // Input flag
	term_attr.c_oflag = 0;                                   // Output flag
	term_attr.c_lflag = 0;                                   // Local flag

	switch(pst_Config->u32_BaudRate)
	  {
	  	case 4800:
			baud = B4800;
			break;
	  	case 9600:
			baud = B9600;
			break;
		case 19200:
			baud = B19200;
			break;
		case 57600:
			baud = B57600;
			break;
		case 115200:
			baud = B115200;
			break;
		case 230400:
			baud = B230400;
			break;
		
		default :
			baud = B115200;
			break;
	  }

	if( cfsetispeed(&term_attr, baud) == -1 )
	{
		CFR_DBG_ERROR( "Error: failed to set input baud rate\n" );
		close (fd);
		return -1;
	}

	if( cfsetospeed(&term_attr, baud) == -1 )
	{
		CFR_DBG_ERROR( "Error: failed to set output baud rate\n" );
		close (fd);
		return -1;
	}

	if( tcsetattr(fd, TCSAFLUSH, &term_attr) != 0 )
	{
		CFR_DBG_ERROR( "Error: tcsetattr() for TERM_DEVICE failed\n" );
		close (fd);
		return -1;
	}

	return   fd;
}
