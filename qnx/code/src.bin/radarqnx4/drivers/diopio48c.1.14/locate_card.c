/* main.c
   ======
   Author: J.Thornhill, D.Andre & R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/


#include <i86.h>
#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/kernel.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/pci.h>

#define DEVICE	0x101
#define VENDOR	0x6666

/* 
 $Log: locate_card.c,v $
 Revision 1.2  2005/07/06 19:03:39  barnes
 Modification to use the correct macros to determine base address.

 Revision 1.1  2005/06/16 15:03:35  barnes
 Initial revision

*/



/*
	locates and returns base addresses of PCI8255 card(s)
	This demonstration program prints the base addresses of up to two
	boards. Its purpose is to test the PCI location routines.

	It is assumed that the DIP switches on board 1 are set to all on
	and on board 2, if present, switch bit 1 is set to off.


PCI8255 Address Map

The card reports three address spaces to the BIOS:
	memory mapped space 128 bytes
	i/o mapped space 128 bytes
	i/o mapped space 256 bytes

It is the last space, i/o mapped 256 bytes, that is the base address for the 8254 and 8255 devices. Additionally, the front panel 4 bit DIP switch can be read back from this address space. The purpose of this switch is to act as a physical board identifier. 

All addresses are shown as hex offsets from the base address

8255 number 1
00	port 1A data register
01	port 1B data register
02	port 1C data register
03	port 1 control register

8255 number 2
04	port 2A data register
05	port 2B data register
06	port 2C data register
07	port 2 control register

8254 timer
08	counter 0
09	counter 1
0a 	counter 2
0b	control register

DIP switch
0c	read only register


The DIP switch is connected to the lower 4 bits of the data bus. When a switch is set to off it registers a logical 1, when set to on a 0 is returned.

Thus
All switches set to on returns 0 in location 0c

switch bit 1 set to off returns 1
switch bit 2 set to off returns 2
switch bit 3 set to off returns 4
switch bit 4 set to off returns 8

*/

int locate_card(int *ba1,int *ba2){
  int status;
  unsigned busnum1,busnum2;
  unsigned devfuncnum1,devfuncnum2;
  int number_cards;
  int address;
  unsigned lastbus,version,hardware;
  int sw1,sw2;

  version=lastbus=hardware=0;

  status=_CA_PCI_BIOS_Present(&lastbus,&version,&hardware);
  if(status!=PCI_SUCCESS)return(-2);

  number_cards=0;
  status=_CA_PCI_Find_Device((unsigned)DEVICE,(unsigned)VENDOR,0,
                            &busnum1,&devfuncnum1);
  if(status==PCI_DEVICE_NOT_FOUND) return -1;	/* no board found */
  if(status==PCI_BAD_VENDOR_ID)  return -3;	/* wrong ID */
  number_cards=1;

  status=_CA_PCI_Find_Device((unsigned)DEVICE,(unsigned)VENDOR,1,
                              &busnum2,&devfuncnum2);
  if(status==PCI_SUCCESS) number_cards=2; /* found second card */

  /* find base address of first card */

  status=_CA_PCI_Read_Config_DWord(busnum1,devfuncnum1,
                                   offsetof(struct _pci_config_regs,
                                   Base_Address_Regs[3]),1,(char *)&address);

 
  *ba1= PCI_IO_ADDR( address);

  if (number_cards==1) {	
     *ba2=0;
     return 1;
  }

  /* find base address of second card */

  status=_CA_PCI_Read_Config_DWord(busnum2,devfuncnum2,
                                   offsetof(struct _pci_config_regs,
                                   Base_Address_Regs[3]),1,(char *)&address);

  *ba2= PCI_IO_ADDR( address);


  /* Bow we need to check if the cards have been detected in the 
   * correct order. 
   * The first card should return 0 from location base address+0x0c
   * the second card should return 1, this assumes that the 
   * front panel DIP switches have been set correctly
   */

  sw1=inp((int)(*ba1+0x0c));
  sw2=inp((int)(*ba2+0x0c));


  if ((sw1==0) && (sw2==1)) return 2;
   
  if((sw1==1) && (sw2==0)){
    address=*ba2;   /* swap base address round */
    *ba2=*ba1;
    *ba1=address;
    return 2;
  }
  return -4;
}


