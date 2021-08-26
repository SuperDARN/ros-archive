/* pci.c 
   ===== 
   Author: R.J.Barnes
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

#include <stdlib.h>
#include <stdio.h>
#include <i86.h>
#include <conio.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <sys/pci.h>
#include "registers.h"

/*
 $Log: pci.c,v $
 Revision 1.3  2006/11/07 21:48:13  code
 Fixed bug in PCI code.

 Revision 1.2  2006/10/24 17:56:09  code
 Todd Parris's modification to prevent bus halts.

 Revision 1.1  2005/07/25 15:12:45  barnes
 Initial revision

*/

int initPCI9080(int *ad_irq,long *BASE1,long *BASE2,long *BASE3,long *PCICR){
  struct _pci_config_regs pci_reg;
  unsigned lastbus, version, hardware, bus, device;
  int pci_index=0;
  long unsigned temp,temp_out,temp_in;
  long write_PCI9080_PCICR=0x107;
  long write_PCI9080_CNTRL=0x1007e;
  
  if(_CA_PCI_BIOS_Present(&lastbus, &version, &hardware) !=PCI_SUCCESS){
    perror("Cannot find PCI BIOS");
    return -1;
  }
  if(_CA_PCI_Find_Device(DEVICE_ID, VENDOR_ID, pci_index,
                         &bus, &device) != PCI_SUCCESS) {
    perror("Cannot find GC214-PCI Digital Receiver Card");
    return -1;
  }
  if(_CA_PCI_Read_Config_DWord(bus,device, 0, 16, 
                              (char *) &pci_reg) != PCI_SUCCESS){
    perror("Cannot read configuration of the Digital Receiver Card");
    return -1;
  }

  *ad_irq=(int) pci_reg.Interrupt_Line;
  *BASE1=(long) pci_reg.Base_Address_Regs[0];
  *BASE2=(long) pci_reg.Base_Address_Regs[1]-1;
  *BASE3=(long) pci_reg.Base_Address_Regs[2];
  temp=_CA_PCI_Write_Config_Word(bus,device,reg_PCI9080_PCICR,1,
                                 (char *) &write_PCI9080_PCICR);
  temp=_CA_PCI_Read_Config_Word(bus,device,4,1,
                                (char *) PCICR);

  temp_out=outpd(*BASE2+reg_PCI9080_BIGEND,0x00);
  temp_in=inpd(*BASE3+reg_PCI9080_BIGEND);
  temp_out=outpd(*BASE2+reg_PCI9080_CNTRL,write_PCI9080_CNTRL);
  temp_in=inpd(*BASE2+reg_PCI9080_CNTRL);
  temp_in=inpd(*BASE2+reg_PCI9080_INTCSR);
  temp_out=outpd(*BASE2+reg_PCI9080_INTCSR,temp_in | 0x00080000);  
  return write_PCI9080_CNTRL;
}
