/*!
*	\file		cmbs_util.c
*	\brief		This file contains utility functions for CMBS API usage
*	\Author		andriig 
*
*	@(#)	%filespec: cmbs_util.c~DMZD53#2.1.2 %
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================
*  date        name     version   action                                          
*  ---------------------------------------------------------------------------
*
* 17-Oct-14 tcmc_asa  ---GIT-- added CMBS_PARAM_INL_DEL
* 30-Jul-2013 tcmc_asa --GIT-- added CMBS_PARAM_FP_CUSTOM_FEATURES(_LENGTH )
*
*******************************************************************************/

#include "cmbs_util.h"
#include "cfr_debug.h"

u16 cmbs_util_GetParameterLength( E_CMBS_PARAM e_Param )
{
	switch ( e_Param )
	{
	case  CMBS_PARAM_RFPI:
		return CMBS_PARAM_RFPI_LENGTH;
	case  CMBS_PARAM_RVBG:
		return CMBS_PARAM_RVBG_LENGTH;
	case  CMBS_PARAM_RVREF:
		return CMBS_PARAM_RVREF_LENGTH;
	case  CMBS_PARAM_RXTUN:
		return CMBS_PARAM_RXTUN_LENGTH;
	case  CMBS_PARAM_MASTER_PIN:
	case  CMBS_PARAM_AUTH_PIN:
		return CMBS_PARAM_PIN_CODE_LENGTH;
	case  CMBS_PARAM_COUNTRY:
		return CMBS_PARAM_COUNTRY_LENGTH;
	case  CMBS_PARAM_SIGNALTONE_DEFAULT:
		return CMBS_PARAM_SIGNALTONE_LENGTH;
	case  CMBS_PARAM_TEST_MODE:
		return CMBS_PARAM_TEST_MODE_LENGTH;
	case CMBS_PARAM_ECO_MODE:
		return CMBS_PARAM_ECO_MODE_LENGTH;
	case  CMBS_PARAM_AUTO_REGISTER:
		return CMBS_PARAM_AUTO_REGISTER_LENGTH;
	case  CMBS_PARAM_NTP:
		return CMBS_PARAM_NTP_LENGTH;
	case CMBS_PARAM_GFSK:
		return CMBS_PARAM_GFSK_LENGTH;
	case  CMBS_PARAM_RESET_ALL:
		return CMBS_PARAM_RESET_ALL_LENGTH;
	case CMBS_PARAM_SUBS_DATA:
		return CMBS_PARAM_SUBS_DATA_LENGTH;
	case  CMBS_PARAM_AUXBGPROG:
		return CMBS_PARAM_AUXBGPROG_LENGTH;
	case  CMBS_PARAM_AUXBGPROG_DIRECT:
		return CMBS_PARAM_AUXBGPROG_DIRECT_LENGTH;
	case  CMBS_PARAM_ADC_MEASUREMENT:
		return CMBS_PARAM_ADC_MEASUREMENT_LENGTH;
	case  CMBS_PARAM_PMU_MEASUREMENT:
		return CMBS_PARAM_PMU_MEASUREMENT_LENGTH;
	case  CMBS_PARAM_RSSI_VALUE:
		return CMBS_PARAM_RSSI_VALUE_LENGTH;
	case CMBS_PARAM_DECT_TYPE:
		return CMBS_PARAM_DECT_TYPE_LENGTH;
	case CMBS_PARAM_MAX_NUM_ACT_CALLS_PT:
		return CMBS_PARAM_MAX_NUM_ACT_CALLS_PT_LENGTH;
	case CMBS_PARAM_ANT_SWITCH_MASK:
		return CMBS_PARAM_ANT_SWITCH_MASK_LENGTH;
	case CMBS_PARAM_PORBGCFG:
		return CMBS_PARAM_PORBGCFG_LENGTH;
	case  CMBS_PARAM_BERFER_VALUE:
		return CMBS_PARAM_BERFER_VALUE_LENGTH;
	case  CMBS_PARAM_INBAND_COUNTRY:
		return CMBS_PARAM_INBAND_COUNTRY_LENGTH;
	case  CMBS_PARAM_FP_CUSTOM_FEATURES:
		return CMBS_PARAM_FP_CUSTOM_FEATURES_LENGTH;
	case CMBS_PARAM_HAN_DECT_SUB_DB_START:
	case CMBS_PARAM_HAN_DECT_SUB_DB_END:
	case CMBS_PARAM_HAN_ULE_SUB_DB_START:
	case CMBS_PARAM_HAN_ULE_SUB_DB_END:
	case CMBS_PARAM_HAN_FUN_SUB_DB_START:
	case CMBS_PARAM_HAN_FUN_SUB_DB_END:
	case CMBS_PARAM_HAN_FUN_GROUP_LIST_START:
	case CMBS_PARAM_HAN_FUN_GROUP_LIST_END:
	case CMBS_PARAM_HAN_FUN_GROUP_TABLE_START:
	case CMBS_PARAM_HAN_FUN_GROUP_TABLE_END:
	case CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START:
	case CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_END:
		return CMBS_PARAM_HAN_DB_ADDR_LENGTH;
	case CMBS_PARAM_HAN_ULE_NEXT_TPUI:
		return CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH;
	case CMBS_PARAM_DHSG_ENABLE:
		return CMBS_PARAM_DHSG_ENABLE_LENGTH;
	case CMBS_PARAM_PREAM_NORM:
		return CMBS_PARAM_PREAM_NORM_LENGTH;
	case CMBS_PARAM_RF_FULL_POWER:
		return CMBS_PARAM_RF_FULL_POWER_LENGTH;
	case CMBS_PARAM_RF_LOW_POWER:
		return CMBS_PARAM_RF_LOW_POWER_LENGTH;
	case CMBS_PARAM_RF_LOWEST_POWER:
		return CMBS_PARAM_RF_LOWEST_POWER_LENGTH;
	case CMBS_PARAM_RF19APU_MLSE:
		return CMBS_PARAM_RF19APU_MLSE_LENGTH;
	case CMBS_PARAM_RF19APU_KCALOVR:
		return CMBS_PARAM_RF19APU_KCALOVR_LENGTH;
	case CMBS_PARAM_RF19APU_KCALOVR_LINEAR:
		return CMBS_PARAM_RF19APU_KCALOVR_LINEAR_LENGTH;
	case CMBS_PARAM_RF19APU_SUPPORT_FCC:
		return CMBS_PARAM_RF19APU_SUPPORT_FCC_LENGTH;
	case CMBS_PARAM_RF19APU_DEVIATION:
		return CMBS_PARAM_RF19APU_DEVIATION_LENGTH;
	case CMBS_PARAM_RF19APU_PA2_COMP:
		return CMBS_PARAM_RF19APU_PA2_COMP_LENGTH;
	case CMBS_PARAM_RFIC_SELECTION:
		return CMBS_PARAM_RFIC_SELECTION_LENGTH;
	case CMBS_PARAM_MAX_USABLE_RSSI:
		return CMBS_PARAM_MAX_USABLE_RSSI_LENGTH;
	case CMBS_PARAM_LOWER_RSSI_LIMIT:
		return CMBS_PARAM_LOWER_RSSI_LIMIT_LENGTH;
	case CMBS_PARAM_PHS_SCAN_PARAM:
		return CMBS_PARAM_PHS_SCAN_PARAM_LENGTH;
	case CMBS_PARAM_JDECT_LEVEL1_M82:
		return CMBS_PARAM_JDECT_LEVEL1_M82_LENGTH;
	case CMBS_PARAM_JDECT_LEVEL2_M62:
		return CMBS_PARAM_JDECT_LEVEL2_M62_LENGTH;
	case CMBS_PARAM_AUXBGP_DCIN:
		return CMBS_PARAM_AUXBGP_DCIN_LENGTH;
	case CMBS_PARAM_AUXBGP_RESISTOR_FACTOR:
		return CMBS_PARAM_AUXBGP_RESISTOR_FACTOR_LENGTH;
	case CMBS_PARAM_DAC1_VOL:
		return CMBS_PARAM_DAC_VOL_LENGTH;
	case CMBS_PARAM_DAC2_VOL:
		return CMBS_PARAM_DAC_VOL_LENGTH;
	case CMBS_PARAM_INL_DEL:
		return CMBS_PARAM_INL_DEL_LENGTH;
	 case CMBS_PARAM_SYPO_GPIO:
		return CMBS_PARAM_SYPO_GPIO_LENGTH;
	 case CMBS_PARAM_SYPO_WAIT_FOR_SYNC:
		return CMBS_PARAM_SYPO_WAIT_FOR_SYNC_LENGTH;
	 case CMBS_PARAM_SYPO_MODE:
		return CMBS_PARAM_SYPO_MODE_LENGTH;           
	 case CMBS_PARAM_UART_DELAY_TIMER:
		return CMBS_PARAM_UART_DELAY_TIMER_LENGTH;
	 case CMBS_PARAM_MAX_TRANSFER_SIZE:
		return CMBS_PARAM_MAX_TRANSFER_SIZE_LENGTH;
	case CMBS_PARAM_IOM_TEST_MODE:
		return CMBS_PARAM_IOM_TEST_MODE_LENGTH;
	 case CMBS_PARAM_RING_ON_OFF:
		return CMBS_PARAM_RING_ON_OFF_LENGTH;
	 case CMBS_PARAM_NEMO_MODE:
		return CMBS_PARAM_NEMO_MODE_LENGTH;
	 case CMBS_PARAM_HS_CW_DISABLED:
		return CMBS_PARAM_HS_CW_DISABLED_LENGTH;
    case CMBS_PARAM_ENC_DISABLE:
        return CMBS_PARAM_ENC_DISABLE_LENGTH;           
	 case CMBS_PARAM_INL_ADD:
		return CMBS_PARAM_INL_ADD_LENGTH;
	 case CMBS_PARAM_BBD_UPDATE:
		return CMBS_PARAM_BBD_UPDATE_LENGTH;
	 case CMBS_PARAM_CLOCK_MASTER_EDIT:
		return CMBS_PARAM_CLOCK_MASTER_EDIT_LENGTH;
	 case CMBS_PARAM_FXS_CALLEE_REGRET_TIME:
	 case CMBS_PARAM_FXS_FIRST_DIGIT_TIMER:
	 case CMBS_PARAM_FXS_INTER_DIGIT_TIMER:
	 case CMBS_PARAM_FXS_STAR_HASH_CON_TIMER:
		return CMBS_PARAM_FXS_TIMER_LENGTH;
	 case CMBS_PARAM_FXS_TONE_CONFIG:
		return CMBS_PARAM_FXS_TONE_CONFIG_LENGTH;
     case CMBS_PARAM_PREP_QSPI_FOR_HW_RESET:
        return CMBS_PARAM_PREP_QSPI_FOR_HW_RESET_LENGTH;   	
    case CMBS_PARAM_INT_START_CALL_TO_HOST:
	      return CMBS_PARAM_INT_START_CALL_TO_HOST_LENGTH;
	case CMBS_PARAM_SUBS_DATA_EX:
		return CMBS_PARAM_SUBS_DATA_EX_LENGTH;
	
	case CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS:
		return CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH;
        
    case CMBS_PARAM_NEMO_CONTROL:
        return CMBS_PARAM_NEMO_CONTROL_LENGTH;    
               
    case CMBS_PARAM_REPEATER_TYPE:
        return CMBS_PARAM_REPEATER_TYPE_LENGTH;           

     case CMBS_PARAM_REPEATER_SUBS_START:
        return CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH;           

     case CMBS_PARAM_REPEATER_SUBS_END:
        return CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH;           
       
	default:
		break;

	}

	return CMBS_RC_ERROR_PARAMETER;
}

E_CMBS_RC   cmbs_util_ParameterValid( E_CMBS_PARAM e_Param, u16 u16_DataLen )
{
	// get parameter length
	u16  u16_RequiredLength = cmbs_util_GetParameterLength(e_Param);

	// Check parameter length
	if ( !u16_RequiredLength || u16_DataLen != u16_RequiredLength )
	{
        if(e_Param != CMBS_PARAM_BBD_UPDATE) // CMBS_PARAM_BBD_UPDATE is a special case that we do not know the size in advance
        {
		CFR_DBG_ERROR( "Parameter ERROR: Length mismatch. Required:%d <-> Got:%d\n",
			u16_RequiredLength, u16_DataLen );

		return CMBS_RC_ERROR_PARAMETER;
		}
	}

	return   CMBS_RC_OK;
}

bool cmbs_util_RawPayloadEvent( u16 u16_EventID )
{
	switch(u16_EventID)
	{
	case CMBS_EV_DSR_FW_UPD_START:
	case CMBS_EV_DSR_FW_UPD_PACKETNEXT:
	case CMBS_EV_DSR_FW_UPD_END:
		return TRUE;
	default:
		break;
	}

	return FALSE;
}
