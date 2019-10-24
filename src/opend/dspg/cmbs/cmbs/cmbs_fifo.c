/*!
*  \file       cmbs_fifo.c
*  \brief      Fifo message queue for CMBS host and CMBS target
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                      \n
*  ---------------------------------------------------------------------------\n
*
*******************************************************************************/

#include "cmbs_api.h"
#include "cmbs_int.h"
#include "cfr_ie.h"
#include "cfr_debug.h"
#include "cmbs_fifo.h"
#include "cfr_mssg.h"

#if defined(CMBS_API_TARGET)
#include "cg0type.h"
#include "csys0reg.h"
#include "cos00int.h"				 // Needed for critical section

#if defined (CSS)
#include "plicu.h"
#include "priorities.h"
#endif

#endif

#if defined( __linux__ )
#include <stdio.h>
#include <string.h>
#endif






// Brief
//------------
// Init the FIFO
void              cmbs_util_FifoInit(ST_CMBS_FIFO *p_Fifo,
                                     void *pv_Buffer,
                                     const u16 u16_ElementSize,
                                     const u16 u16_Size,
                                     CFR_CMBS_CRITICALSECTION p_cSection)
{
	//printf("cmbs_util_FifoInit-->\n");
    p_Fifo->u16_Count    = 0;
    p_Fifo->u16_ElemSize = u16_ElementSize;

    p_Fifo->p_Read  = p_Fifo->p_Write = pv_Buffer;
    p_Fifo->u16_Size = p_Fifo->u16_Read2End = p_Fifo->u16_Write2End = u16_Size;
	p_Fifo->p_cSection = p_cSection;

	//init statistics
	p_Fifo->s_fifo_statistics.u32_MaxInUse = 0;
	p_Fifo->s_fifo_statistics.u32_CountFIFOIsFull= 0;
	
	CFR_CMBS_INIT_CRITICALSECTION ( p_Fifo->p_cSection );
}


// Brief
//------------
// Push a message to FIFO
u8                cmbs_util_FifoPush(ST_CMBS_FIFO *p_Fifo, void *pv_Element)
{
    u16             u16_Write2End;
    u8 *p_Write;

	//printf("cmbs_util_FifoPush-->\n");

	CFR_CMBS_ENTER_CRITICALSECTION ( p_Fifo->p_cSection );

    if ( p_Fifo->u16_Count >= p_Fifo->u16_Size )
    {
    	p_Fifo->s_fifo_statistics.u32_CountFIFOIsFull++;
		p_Fifo->s_fifo_statistics.u32_MaxInUse = 0;
		CFR_CMBS_LEAVE_CRITICALSECTION ( p_Fifo->p_cSection );

		CFR_DBG_WARN("cmbs_util_FifoPush: FIFO full\n");

		return 0;
    }

    p_Write = p_Fifo->p_Write;

    memcpy(p_Write, pv_Element, p_Fifo->u16_ElemSize);

    p_Write += p_Fifo->u16_ElemSize;

    u16_Write2End = p_Fifo->u16_Write2End;

    if ( --u16_Write2End == 0 )
    {
        u16_Write2End = p_Fifo->u16_Size;
        p_Write -= u16_Write2End * p_Fifo->u16_ElemSize;
    }

    p_Fifo->u16_Write2End = u16_Write2End;
    p_Fifo->p_Write = p_Write;

	p_Fifo->u16_Count++;

	if(p_Fifo->u16_Count > p_Fifo->s_fifo_statistics.u32_MaxInUse)
	{
		p_Fifo->s_fifo_statistics.u32_MaxInUse = p_Fifo->u16_Count;
	}
	
	CFR_CMBS_LEAVE_CRITICALSECTION ( p_Fifo->p_cSection );

    return 1;
}


// Brief
//------------
// Get a message from FIFO
void*            cmbs_util_FifoGet(ST_CMBS_FIFO *p_Fifo)
{
	//printf("cmbs_util_FifoGet-->\n");

	CFR_CMBS_ENTER_CRITICALSECTION ( p_Fifo->p_cSection );

    if ( p_Fifo->u16_Count == 0 )
    {
        //      CFR_DBG_WARN( "cmbs_util_FifoGet: FIFO empty\n" );
		CFR_CMBS_LEAVE_CRITICALSECTION ( p_Fifo->p_cSection );
        return NULL;
    }


	CFR_CMBS_LEAVE_CRITICALSECTION ( p_Fifo->p_cSection );

    return p_Fifo->p_Read;
}

// Brief
//------------
// Pop (extract) a message from a FIFO
void*            cmbs_util_FifoPop(ST_CMBS_FIFO *p_Fifo)
{
    u16             u16_Read2End;
    u8 *p_Read;
    void *p_Element;

	//printf("cmbs_util_FifoPop-->\n");

	CFR_CMBS_ENTER_CRITICALSECTION ( p_Fifo->p_cSection );

    if ( p_Fifo->u16_Count == 0 )
    {
        CFR_DBG_WARN( "cmbs_util_FifoGet: FIFO empty\n" );
		CFR_CMBS_LEAVE_CRITICALSECTION ( p_Fifo->p_cSection );
        return NULL;
    }

    u16_Read2End = p_Fifo->u16_Read2End;
    p_Read      = p_Fifo->p_Read;
    p_Element   = p_Read;

    p_Read += p_Fifo->u16_ElemSize;

    if ( --u16_Read2End == 0 )
    {
        u16_Read2End = p_Fifo->u16_Size;
        p_Read -= u16_Read2End * p_Fifo->u16_ElemSize;
    }

    p_Fifo->p_Read = p_Read;
    p_Fifo->u16_Read2End = u16_Read2End;

    p_Fifo->u16_Count--;

	CFR_CMBS_LEAVE_CRITICALSECTION ( p_Fifo->p_cSection );

    return p_Element;
}

// Brief
//------------
// Pop all messages from a FIFO. Return no. of popped messages
u16  cmbs_util_FifoClear(ST_CMBS_FIFO *p_Fifo)
{
	u16	u16_count=0;

	while (cmbs_util_FifoPop(p_Fifo))	// pop untill return NULL
	{
		u16_count++;
	}
	return u16_count;
}

void  cmbs_util_FifoPrintStatistics(ST_CMBS_FIFO* p_Fifo)
{
	CFR_CMBS_ENTER_CRITICALSECTION(p_Fifo->p_cSection);
	
	printf("\n\tCurrent use in fifo: %d",p_Fifo->u16_Count);
	printf("\n\tMax use in fifo:     %d",p_Fifo->s_fifo_statistics.u32_MaxInUse);
	printf("\n\tCount of full fifo:  %d\n\n",p_Fifo->s_fifo_statistics.u32_CountFIFOIsFull);

	CFR_CMBS_LEAVE_CRITICALSECTION(p_Fifo->p_cSection);
	
}


u16 cmbs_util_FifoCount(ST_CMBS_FIFO *p_Fifo)
{

    u16 u16_count;

    CFR_CMBS_ENTER_CRITICALSECTION(p_Fifo->p_cSection);

    u16_count = p_Fifo->u16_Count;

    CFR_CMBS_LEAVE_CRITICALSECTION(p_Fifo->p_cSection);

    return u16_count;
}
/*[----------- End Of File -------------------------------------------------------------------------------------------------------------]*/
