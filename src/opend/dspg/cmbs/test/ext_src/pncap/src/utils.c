/** @file utils.c
 *
 *  @brief Helper functions for creating PNCAP messages
 */
#include "utils.h"



/** Converts decimal number to binary coded decimal.
 *  
 *  Example: 14-->0x14; 1-->0x01; 99-->0x99
 *
 *  @param  decimal number that must not be bigger than  99
 *  @return the binary coded decimal (BCD)
 */
unsigned char ucBin2Bcd(unsigned char decimal)
{
  int i;
  unsigned long ulCopy = decimal;

  for( i=0; i<8; i++ )
  {
    if( ( ulCopy & 0x00f00 ) >= 0x00500 )
    {
      ulCopy += 0x00300;
    }
    if( ( ulCopy & 0x00f000 ) >= 0x005000 )
    {
      ulCopy += 0x003000;
    }
    if( ( ulCopy & 0x00f0000 ) >= 0x0050000 )
    {
      ulCopy += 0x0030000;
    }
    //shift
    ulCopy <<= 1;
  }

  return( (unsigned char)(ulCopy >> 8));
}
