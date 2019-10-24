/*!
*  \file       cmbs_fifo.h
*  \brief      Fifo message queue for CMBS host and CMBS target
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                          \n
*  ----------------------------------------------------------------------------\n
*
*******************************************************************************/

#if   !defined( CMBS_FIFO_H )
#define  CMBS_FIFO_H
#include "cmbs_int.h"
#include "cfr_mssg.h"

typedef struct
{
	u32 u32_MaxInUse;
	u32 u32_CountFIFOIsFull;
}
t_st_Fifo_Statistics;


/*! \brief simple FIFO definition */
typedef struct
{
   u16            u16_Count;         // number of elements
   u16            u16_Size;          // buffer size( max number of elements )
   u16             u16_ElemSize;      // element size
   void *         p_Read;           // read pointer
   void *         p_Write;          // write pointer
   u16             u16_Read2End;      // number of unused elements until fifo end
   u16             u16_Write2End;     // number of unused elements until fifo end
   CFR_CMBS_CRITICALSECTION p_cSection;	// Critical Section for the queue
   t_st_Fifo_Statistics s_fifo_statistics; // statistics for queue
} ST_CMBS_FIFO, * PST_CMBS_FIFO;






#if defined( __cplusplus )
extern "C"
{
#endif




/*****************************************************************************
 * Utilities
 *****************************************************************************/

void              cmbs_util_FifoInit( ST_CMBS_FIFO * p_Fifo,
                                     void * pv_Buffer,
                                     const u16 u16_ElementSize,
                                     const u16 u16_Size,
                                     CFR_CMBS_CRITICALSECTION p_cSection);

u8                cmbs_util_FifoPush( ST_CMBS_FIFO * p_Fifo, void * pv_Element );
void *            cmbs_util_FifoGet( ST_CMBS_FIFO * p_Fifo );
void *            cmbs_util_FifoPop( ST_CMBS_FIFO * p_Fifo );
u16		          cmbs_util_FifoClear( ST_CMBS_FIFO * p_Fifo );
u16		          cmbs_util_FifoCount(ST_CMBS_FIFO *p_Fifo);
void  cmbs_util_FifoPrintStatistics(ST_CMBS_FIFO* p_Fifo);




#if defined( __cplusplus )
}
#endif

#endif   // CMBS_FIFO_H

