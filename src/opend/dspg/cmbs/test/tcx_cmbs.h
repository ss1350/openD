/*!
    \brief main programm to run XML test cases.

*/
#if   !defined( TCX_CMBS_H )
#define  TCX_CMBS_H

#define CMBS_USB_DONGLE_HARDWARE_ID             "USB\\VID_3006&PID_1977&REV_0100&MI_00"
#define CMBS_USB_DONGLE_HARDWARE_ID_NO_VER      "USB\\VID_3006&PID_1977"
#define CMBS_USB_DONGLE_HARDWARE_ID_NO_VER_LEN  strlen(CMBS_USB_DONGLE_HARDWARE_ID_NO_VER)
#define APPL_VERSION 0x0412
/* 0x0101 : 01 (MSB) stands for RC1, 01 (LSB) for build 1*/
#define APPL_BUILD	0x0001


#endif   // TCX_CMBS_H
