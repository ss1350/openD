/*******************************************************************
*
*    DESCRIPTION: Definitions of HAN FUN Protocol elements
*
*    AUTHOR: CMBS Team
*
*    © DSP Group Ltd 2011
*
*******************************************************************/
#ifndef HANFUN_PROTOCOL_H
#define HANFUN_PROTOCOL_H

/*
   Bind Management Interface ID 
====================================
This interface is used for binding Device-Unit-Interface Client with Device-Unit-Interface Server
*/
#define BIND_MGMT_INTERFACE_ID    0x0002

/* Bind Management Interface - Server */
/* Attributes */
/* Commands */
#define BIND_MGMT_IF_SERVER_CMD_ID_ADD_BIND         0x0001
#define BIND_MGMT_IF_SERVER_CMD_ID_REMOVE_BIND      0x0002


/* Bind Management Interface - Client */
/* Attributes */
/* Commands */



/*
   Group Management Interface ID 
====================================
This interface is used for adding Device-Unit to group
*/
#define GROUP_MGMT_INTERFACE_ID     0x0003
#define INTERFACE_ID_MASK    		0x7fff
#define INTERFACE_TYPE_MASK         0x8000
#define INTERFACE_TYPE_SERVER       0x01
#define INTERFACE_TYPE_CLIENT       0x00

/* Group Management Interface - Server */
/* Attributes */
/* Commands */
#define GROUP_MGMT_IF_SERVER_CMD_ID_CREATE_GROUP            0x0001
#define GROUP_MGMT_IF_SERVER_CMD_ID_DELETE_GROUP            0x0002
#define GROUP_MGMT_IF_SERVER_CMD_ID_ADD_TO_GROUP            0x0003
#define GROUP_MGMT_IF_SERVER_CMD_ID_REMOVE_FROM_GROUP       0x0004
#define GROUP_MGMT_IF_SERVER_CMD_ID_GET_GROUP_INFO          0x0005


/*
   Alert Interface ID 
====================================
*/
#define		ALERT_INTERFACE_ID			0x0100
#define		HAN_IF_ALERT_ATTR_STATE		0x01

/* Alert Interface - Server */
/* Attributes */
/* Commands */
#define		ALERT_IF_SERVER_CMD_ALERT_OFF					0x0000
#define		ALERT_IF_SERVER_CMD_ALERT_ON					0x0001

#define		ALERT_IF_SERVER_ATTR_ALERT_STATE				0x0001

/*
   Tamper Interface ID 
====================================
*/
#define		TAMPER_INTERFACE_ID				0x0101
#define		HAN_IF_TAMPER_ATTR_STATE		0x01


/*
   Power Interface ID 
====================================
*/
#define		POWER_INTERFACE_ID								0x0110
#define		HAN_IF_POWER_ATTR_BATTERY_VOLTAGE				0x07
#define		HAN_IF_POWER_ATTR_BATTERY_VOLTAGE_STATUS		0x0d
#define		HAN_IF_POWER_ATTR_BATTERY_VOLTAGE_MIN_TH		0x0e
#define		HAN_IF_POWER_ATTR_BATTERY_VOLTAGE_MAX_TH		0x0f


/*
   RSSI Interface ID 
====================================
*/
#define		RSSI_INTERFACE_ID			0x0111
#define		HAN_IF_RSSI_ATTR_VALUE		0x01


/*
   Test Mode Interface ID 
====================================
*/

#define		TESTMODE_INTERFACE_ID						0x7f01
#define		HAN_IF_TESTMODE_ATTR_BULK_SIZE				0x01
#define		HAN_IF_TESTMODE_ATTR_BULK_INTERVAL			0x02
#define		HAN_IF_TESTMODE_ATTR_PACKET_INTERVAL		0x03




/*
   Price Update Manager Interface ID 
====================================
*/

#define		PRICE_UPDATE_MANAGER_INTERFACE_ID							0x7f03
#define		HAN_IF_PRICE_UPDATE_MANAGER_ATTR_SCHEDULED_TIME				0x01
#define		HAN_IF_PRICE_UPDATE_MANAGER_ATTR_CURRNET_FMF				0x02
#define		HAN_IF_PRICE_UPDATE_MANAGER_ATTR_RETRY						0x03



/*
   GNRGY METERING Interface ID 
====================================
*/

#define		GNRGY_METERING_INTERFACE_ID					0x7f00
#define		HAN_IF_GNRGY_METERING_ATTR_KEEPALIVE		0x01




/*
   SIMPLE POWER METERING Interface ID 
====================================
*/

#define		SIMPLE_POWER_METERING_INTERFACE_ID						0x0300
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_ENERGY				0x01
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_ENERGY_LAST_RESET		0x02
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_TIME_LAST_RESET		0x03
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_INSTANT_POWER			0x04
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_AVG_POWER				0x05
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_AVG_POWER_INTERVAL	0x06
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_VOLTAGE				0x07
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_CURRENT				0x08
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_FREQUENCY				0x09
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_POWER_FACOR			0x0A
#define		HAN_IF_SIMPLE_POWER_METERING_ATTR_KEEPALIVE				0x0B

#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_MILI				0x01
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_MICRO			0x11
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_NANO				0x12
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_PICO				0x13
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_KILO				0x20
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_MEGA				0x21
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_GIGA				0x22
#define		HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_TERA				0x23


/*
   TEMPERATURE Interface ID 
====================================
*/

#define		TEMPERATURE_INTERFACE_ID					0x0301
#define		HAN_IF_TEMPERATURE_ATTR_TEMPERATURE			0x01
#define		HAN_IF_TEMPERATURE_ATTR_MIN_TEMPERATURE		0x02
#define		HAN_IF_TEMPERATURE_ATTR_MAX_TEMPERATURE		0x03
#define		HAN_IF_TEMPERATURE_ATTR_TOLERANCE			0x04


/*
   AIR PRESSURE Interface ID 
====================================
*/

#define		AIR_PRESSURE_INTERFACE_ID							0x0306
#define		HAN_IF_AIR_PRESSURE_ATTR_MEASURED_PRESSURE			0x01
#define		HAN_IF_AIR_PRESSURE_ATTR_MEASURED_MIN_PRESSURE		0x02
#define		HAN_IF_AIR_PRESSURE_ATTR_MEASURED_MAX_PRESSURE		0x03
#define		HAN_IF_AIR_PRESSURE_ATTR_MEASURED_TOLERANCE			0x04



/*
   EVS HUMIDITY Interface ID 
====================================
*/

#define		EVS_HUMIDITY_INTERFACE_ID				0x0302
#define		HAN_IF_EVS_HUMIDITY_ATTR_HUMIDITY		0x01


/*
   BUTTON Interface ID 
====================================
*/

#define		BUTTON_INTERFACE_ID								0x0304
#define		HAN_IF_BUTTON_ATTR_SHORT_MAX_DURAION			0x01
#define		HAN_IF_BUTTON_ATTR_EXTRA_LONG_MAX_DURAION		0x02
#define		HAN_IF_BUTTON_ATTR_DOUBLE_GAP_DURATION			0x03


/*
   THERMOSTAT Interface ID 
====================================
*/

#define		THERMOSTAT_INTERFACE_ID								0x0303
#define		HAN_IF_THERMOSTAT_ATTR_SUPPORTED_MODES				0x01
#define		HAN_IF_THERMOSTAT_ATTR_CURRENT_MODE					0x02
#define		HAN_IF_THERMOSTAT_ATTR_FAN_MODE						0x03
#define		HAN_IF_THERMOSTAT_ATTR_HEAT_MODE_TEMP				0x04
#define		HAN_IF_THERMOSTAT_ATTR_COOL_MODE_TEMP				0x05
#define		HAN_IF_THERMOSTAT_ATTR_AUTO_MODE_HEAT_TEMP			0x06
#define		HAN_IF_THERMOSTAT_ATTR_AUTO_MODE_COOL_TEMP			0x07
#define		HAN_IF_THERMOSTAT_ATTR_HEAT_MODE_TEMP_OFFSET		0x08
#define		HAN_IF_THERMOSTAT_ATTR_COOL_MODE_TEMP_OFFSET		0x09
#define		HAN_IF_THERMOSTAT_ATTR_BOOST_DURATION				0x0A

/*
   DEVICE STATISTICS Interface ID 
====================================
*/

#define		DEVICE_STATISTICS_INTERFACE_ID						0x7f09
#define		HAN_IF_DEVICE_STATISTICS_ATTR_TX_SUCC				0x01
#define		HAN_IF_DEVICE_STATISTICS_ATTR_TX_FAIL				0x02
#define		HAN_IF_DEVICE_STATISTICS_ATTR_RX_PACKETS			0x03
#define		HAN_IF_DEVICE_STATISTICS_ATTR_LOCK_SUCC				0x04
#define		HAN_IF_DEVICE_STATISTICS_ATTR_LOCK_FAIL				0x05
#define		HAN_IF_DEVICE_STATISTICS_ATTR_PINGS					0x06
#define		HAN_IF_DEVICE_STATISTICS_ATTR_ADD_QUEUE_FAIL		0x07
#define		HAN_IF_DEVICE_STATISTICS_ATTR_SETUP_AGAIN			0x08



/*
   WEEKLY SCHEDULE Interface ID 
====================================
*/

#define		WEEKLY_SCHEDULE_INTERFACE_ID						0x0009
#define		HAN_IF_WEEKLY_SCHEDULE_ATTR_MAX_NUM_OF_ENTRIES		0x01
#define		HAN_IF_WEEKLY_SCHEDULE_ATTR_NUM_OF_ENTRIES			0x02
#define		HAN_IF_WEEKLY_SCHEDULE_ATTR_STATUS					0x03



/*
   On-Off Interface ID 
====================================
*/
#define		ON_OFF_INTERFACE_ID		0x0200

#define		ON_OFF_IF_SERVER_CMD_TURN_ON					0x0001
#define		ON_OFF_IF_SERVER_CMD_TURN_OFF					0x0002
#define		ON_OFF_IF_SERVER_CMD_TOGGLE						0x0003

#define		ON_OFF_IF_SERVER_ATTR_STATE						0x0001
/* Group Management Interface - Client */
/* Attributes */
/* Commands */

/*
   Level Control Interface ID 
====================================
*/

#define		LEVEL_CONTROL_INTERFACE_ID					0x0201
#define		LEVEL_CONTROL_ATTR_LEVEL_INTERFACE_TYPE 	0x01
#define		HAN_IF_LEVEL_CONTROL_ATTR_LEVEL 			0x01



/*
   Device Information Interface ID 
====================================
*/
#define	DEVICE_INFORMATION_INTERFACE_ID						0x5

#define	HAN_IF_DEVICE_INFORMATION_ATTR_HF_RELEASE_VER		0x0001
#define	HAN_IF_DEVICE_INFORMATION_ATTR_PROF_RELEASE_VER		0x0002
#define	HAN_IF_DEVICE_INFORMATION_ATTR_IF_RELEASE_VER		0x0003
#define	HAN_IF_DEVICE_INFORMATION_ATTR_PAGING_CAPS			0x0004
#define	HAN_IF_DEVICE_INFORMATION_ATTR_MIN_SLEEP_TIME		0x0005
#define	HAN_IF_DEVICE_INFORMATION_ATTR_ACT_RESP_TIME		0x0006
#define	HAN_IF_DEVICE_INFORMATION_ATTR_APPL_VER				0x0007
#define	HAN_IF_DEVICE_INFORMATION_ATTR_HW_VER				0x0008
#define	HAN_IF_DEVICE_INFORMATION_ATTR_EMC					0x0009
#define	HAN_IF_DEVICE_INFORMATION_ATTR_IPUE					0x000A
#define	HAN_IF_DEVICE_INFORMATION_ATTR_MANUF_NAME			0x000B
#define	HAN_IF_DEVICE_INFORMATION_ATTR_LOCATION				0x000C
#define	HAN_IF_DEVICE_INFORMATION_ATTR_DEV_ENABLE			0x000D
#define	HAN_IF_DEVICE_INFORMATION_ATTR_FRIENDLY_NAME		0x000E
#define	HAN_IF_DEVICE_INFORMATION_ATTR_UID					0x000F
#define	HAN_IF_DEVICE_INFORMATION_ATTR_SERIAL_NUMBER		0x0010

#define	DEV_INF_IF_ATTR_UID_UID_TYPE_NONE	0x0
#define	DEV_INF_IF_ATTR_UID_UID_TYPE_DECT	0x1
#define	DEV_INF_IF_ATTR_UID_UID_TYPE_MAC	0x2
#define	DEV_INF_IF_ATTR_UID_UID_TYPE_URI	0x3

#define CMBS_HAN_DEV_INFO_RESP_VAL_OK								0x00
#define CMBS_HAN_DEV_INFO_RESP_VAL_FAIL_NOT_SUPPORTED				0x03
#define CMBS_HAN_DEV_INFO_RESP_VAL_FAIL_UNKNOWN						0xFF

/*
   Attribute Reporting Interface ID 
====================================
*/
#define	ATTRIBUTE_REPORTING_INTERFACE_ID		0x6

#define	HAN_IF_ATTR_REPORT_ATTR_NUM_OF_REPORTS				0x01
#define	HAN_IF_ATTR_REPORT_ATTR_NUM_OF_PERIODIC_REPORTS		0x02
#define	HAN_IF_ATTR_REPORT_ATTR_NUM_OF_EVENT_REPORTS		0x03


#define CMBS_HAN_ATTR_PACK_TYPE_ALL_MANDATORY						0x00
#define CMBS_HAN_ATTR_PACK_TYPE_ALL_MANDATORY_AND_OPT				0xFE
#define CMBS_HAN_ATTR_PACK_TYPE_DYNAMIC								0xFF	

#define CMBS_HAN_ATTR_REPORT_REPORT_TYPE_COV						0x00
#define CMBS_HAN_ATTR_REPORT_REPORT_TYPE_HT							0x01
#define CMBS_HAN_ATTR_REPORT_REPORT_TYPE_LH							0x02
#define CMBS_HAN_ATTR_REPORT_REPORT_TYPE_EQUAL						0x03

#define CMBS_HAN_ATTR_REPORT_ATTRIB_ID_NUM_OF_ENTRIES				0x01
#define CMBS_HAN_ATTR_REPORT_ATTRIB_ID_NUM_OF_PERIODIC_ENTRIES		0x02
#define CMBS_HAN_ATTR_REPORT_ATTRIB_ID_NUM_OF_EVENT_ENTRIES			0x03

#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_CREATE_PERIODIC_REP				0x01
#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_CREATE_EVENT_REP				0x02
#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_ADD_ENTRY_PERIODIC_REP			0x03
#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_ADD_ENTRY_EVENT_REP				0x04
#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_DELETE_REP						0x05
#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_GET_PERIODIC_REP_ENTRIES		0x06
#define CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_GET_EVENT_REP_ENTRIES			0x07

#define CMBS_HAN_ATTR_REPORT_S2C_CMD_ID_PERIODIC_REPORT_NOTIFICATION	0x1
#define CMBS_HAN_ATTR_REPORT_S2C_CMD_ID_EVENT_REPORT_NOTIFICATION		0x2

#define CMBS_HAN_ATTR_REPORT_RESP_VAL_OK							0x00
#define CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_NOT_AUTHORIZED			0x01
#define CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_INV_ARG					0x02
#define CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_NOT_ENOUGH_RES			0xFE
#define CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_UNKNOWN					0xFF


/* Group Management Interface - Client */
/* Attributes */
/* Commands */




/*
   Keep-Alive Interface ID 
====================================
*/
#define		KEEP_ALIVE_INTERFACE_ID				0x0115
#define		HAN_IF_KEEP_ALIVE_ATTR_INTERVAL		0x01


/* Group Management Interface - Client */
/* Attributes */
/* Commands */

/*///////////////////////////////////////////////
/////////////////////////////////////////////////
			Proprietary Interfaces
/////////////////////////////////////////////////
///////////////////////////////////////////////*/

/*
   SUOTA
====================================
*/

#define CMBS_HAN_SUOTA_INTERFACE_ID								0x7f05
#define CMBS_HAN_SUOTA_1_2_INTERFACE_ID							0x0400


#define CMBS_HAN_SUOTA_CTS_CMD_ID_CHECK_FOR_VERSION				0x1

#define CMBS_HAN_SUOTA_CTS_CMD_ID_UPGRADE_COMPLETE				0x2
#define CMBS_HAN_SUOTA_CTS_CMD_ID_UPGRADE_COMPLETE_RESULT_OK	0x0

#define CMBS_HAN_SUOTA_STC_CMD_ID_NEW_VERSION_AVAILABLE			0x1


/*
   SUPERMARKET
====================================
*/

#define CMBS_HAN_SUPERMARKET_MGMT_INTERFACE_ID									0x7f03


#define CMBS_HAN_SUPERMARKET_SERVER_ATTR_ID_SCHEDULE_WINDOW						0x01
#define CMBS_HAN_SUPERMARKET_SERVER_ATTR_ID_BASE_FRAME_MF						0x02
#define CMBS_HAN_SUPERMARKET_SERVER_ATTR_ID_PRICE_MESSAGE_RETRY_NUMBER			0x03

#define CMBS_HAN_SUPERMARKET_CTS_CMD_ID_SCHEDULE_CONNECTION						0x01
#define CMBS_HAN_SUPERMARKET_CTS_CMD_ID_PRICE_UPDATING_MSG						0x02
#define CMBS_HAN_SUPERMARKET_CTS_CMD_ID_DELETE_PENDING_MSGS						0x03
#define CMBS_HAN_SUPERMARKET_CTS_CMD_ID_PRICE_ALL_DEVICES_MSGS					0x04

#define CMBS_HAN_SUPERMARKET_STC_CMD_ID_DEVICE_STATUS							0x01
#define CMBS_HAN_SUPERMARKET_STC_CMD_ID_SCHEDULE_COMMAND_RES					0x02

/*
   SIZES
====================================
*/

#define ENTRY_UNIT_ID_SIZE_IN_BYTES						1
#define ENTRY_IF_SIZE_IN_BYTES							2
#define ENTRY_NUM_OF_ATTRIBUTES_SIZE_IN_BYTES			1
#define ENTRY_ATTRIBUTE_ID_SIZE_IN_BYTES				1

#define CMBS_HAN_FUN_INTERFACE_AUDIO											0x7F11

#define CMBS_HAN_FUN_IF_AUDIO_CMD_MAKE_CALL 									0x1

enum
{
	CMBS_HAN_PREFERRED_CODEC_FIELD_ID			= 0x1,
	CMBS_HAN_DIGITS_FIELD_ID					= 0x2,
	CMBS_HAN_OTHER_PARTY_TYPE_FIELD_ID			= 0x3,
	CMBS_HAN_OTHER_PARTY_NAME_FIELD_ID			= 0x4,
	CMBS_HAN_OTHER_PARTY_ID_FIELD_ID			= 0x5,
	CMBS_HAN_CALL_TYPE_FIELD_ID					= 0x6,
};

#endif // HANFUN_PROTOCOL_H
