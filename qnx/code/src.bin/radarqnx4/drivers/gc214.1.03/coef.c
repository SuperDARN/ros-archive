/* core.c 
   ====== */

/*
 $Log: coef.c,v $
 Revision 1.1  2004/05/10 15:02:58  barnes
 Initial revision

*/

#include <stdlib.h>
#include <stdio.h>
#include <i86.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <sys/pci.h>

#include <signal.h>
#include <time.h>
#include "dma-alloc.h"
#include "registers.h"
#include "io.h"

extern unsigned long  base_GC214;

int cfir_coeffs[11]={12,-93,62,804,1283,-1273,-5197,-3512,8332,24823,32767};

int pfir_coeffs[32]={132,182,1,-309,-330,119,560,343,-424,-834,-278,696,
                    960,149,-916,-1132,-212,1075,1514,421,-1518,-2374,-673,
                    2529,3817,726,-4796,-6888,-650,12967,26898,32767};

int resamp_coeffs[256]={114,-2,-2,-3,-3,-3,-3,-4,-4,-5,-5,-6,-6,-7,-8,
                        -9,-9,-11,-11,-12,-13,-14,-15,-16,-17,-18,-19,-20,
                        -21,-22,-23,-24,-25,-26,-27,-27,-28,-29,-29,
                        -30,-30,
                        -31,-31,-31,-31,-31,-31,-31,-31,-30,-30,-29,-28,
                        -27,-26,-25,-23,-22,-20,-18,-16,-14,-12,-10,
                        -7,-4,-2,1,4,7,11,14,18,21,25,29,32,36,40,44,48,52,
                        56,60,64,68,72,76,79,83,86,90,93,96,99,102,104,
                       107,109,111,112,114,115,115,116,116,116,115,114,113,
                       111,109,107,104,100,97,93,88,83,78,72,66,59,52,45,
                       37,29,21,12,3,-7,-17,-27,-37,-48,-59,-70,-81,-93,
                      -104,-116,-127,-139,-151,-163,-174,-186,-197,-209,
                      -220,-231,-241,-252,-262,-271,-280,-289,-297,-304,-311,
                      -318,-323,-328,-333,-336,-339,-341,-342,-342,-341,-339,
                     -336,-332,-327,-321,-314,-305,-296,-285,-273,-260,-246,
                     -231,-214,-196,-177,-157,-135,-112,-88,-63,-37,-9,19,
                     49,80,112,145,179,214,250,287,324,363,402,442,483,
                     524,566,609,651,695,738,782,826,871,915,960,1004,
                     1048,1092,1136,1180,1223,1266,1308,1350,1391,1431,
                     1470,1509,1547,1584,1619,1654,1688,1720,1751,1781,
                     1809,1836,1862,1886,1908,1929,1949,1966,1982,1996,
                     2009,2020,2029,2036,2042,2045,2047};



void init_GC4016() {
  int i;
  write8(base_GC214+offset_GC4016+reg_GC4016_GENSYNC,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_GLOBALRST,0xF8);
  write8(base_GC214+offset_GC4016+reg_GC4016_NCHANOUT,0x23);
  write8(base_GC214+offset_GC4016+reg_GC4016_MISC,0x02);

  write8(base_GC214+offset_GC4016+reg_GC4016_NMULT,0x07);
  write8(base_GC214+offset_GC4016+reg_GC4016_FILTSLCT,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_FINALSHFT,0x14);
  write8(base_GC214+offset_GC4016+reg_GC4016_CHANMAP,0xE4);
  write8(base_GC214+offset_GC4016+reg_GC4016_ADDTO,0x70);
  write8(base_GC214+offset_GC4016+reg_GC4016_RESAMPCLKDVD,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_RATIOMAP,0xE4);
  write32(base_GC214+offset_GC4016+reg_GC4016_RATIO0,0x04000000);
  write32(base_GC214+offset_GC4016+reg_GC4016_RATIO1,0x04000000);
  write32(base_GC214+offset_GC4016+reg_GC4016_RATIO2,0x04000000);
  write32(base_GC214+offset_GC4016+reg_GC4016_RATIO3,0x04000000);
  write8(base_GC214+offset_GC4016+reg_GC4016_GENSYNC,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_CNTSYNC,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_CNTBYTE0,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_CNTBYTE1,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_TRICNTRL,0xFF);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTFORMAT,0x49);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTMODE,0x6C);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTFRAMECNTRL,0xC7);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTWDSIZE,0xEF);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTCLKCNTRL,0x01);
  write8(base_GC214+offset_GC4016+reg_GC4016_SERMUXCNTRL,0xE4);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTTAGA,0x10);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTTAGB,0x32);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTTAGC,0x54);
  write8(base_GC214+offset_GC4016+reg_GC4016_OUTTAGD,0x76);

  init_GC4016_A();
  init_GC4016_B();
  init_GC4016_C();
  init_GC4016_D();

  /* do sync in here */

  write8(base_GC214+offset_GC4016+reg_GC4016_GLOBALRST,0x08);

  /* now take away sync 1 */

  for(i=0;i<256;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_RESAMPCOEFFS+2*i,
            resamp_coeffs[i]);
  }

  /* write output format */

}


void init_GC4016_A(){
  int i;
 
  write16(base_GC214+offset_GC4016+reg_GC4016_A_PHASE,0x0000);
  write32(base_GC214+offset_GC4016+reg_GC4016_A_FREQ,0x2f430313);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_CHRESET,0x0C);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_FREQSYNC,0x77);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_NCOSYNC,0x22);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_ZEROPAD,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_DECSYNC,0x22);
  write16(base_GC214+offset_GC4016+reg_GC4016_A_DECRATIO,0x703f);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_CICSCALE,0x64);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_SPLITIQ,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_CFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_PFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_INPUT,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_PEAKCNTRL,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_A_PEAKCOUNT,00);
  write16(base_GC214+offset_GC4016+reg_GC4016_A_FINEGAIN,0x042e);

  for(i=0;i<11;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_A_CFIRCOEFFS+2*i,
            cfir_coeffs[i]);
  }
  for(i=0;i<32;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_A_PFIRCOEFFS+2*i,
            pfir_coeffs[i]);
  }	
}


void init_GC4016_B(){
  int i;
 
  write16(base_GC214+offset_GC4016+reg_GC4016_B_PHASE,0x0000);
  write32(base_GC214+offset_GC4016+reg_GC4016_B_FREQ,0x2f430313);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_CHRESET,0x0C);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_FREQSYNC,0x77);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_NCOSYNC,0x22);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_ZEROPAD,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_DECSYNC,0x22);
  write16(base_GC214+offset_GC4016+reg_GC4016_B_DECRATIO,0x703f);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_CICSCALE,0x64);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_SPLITIQ,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_CFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_PFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_INPUT,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_PEAKCNTRL,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_B_PEAKCOUNT,00);
  write16(base_GC214+offset_GC4016+reg_GC4016_B_FINEGAIN,0x042e);

  for(i=0;i<11;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_B_CFIRCOEFFS+2*i,
            cfir_coeffs[i]);
  }
  for(i=0;i<32;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_B_PFIRCOEFFS+2*i,
            pfir_coeffs[i]);
  }	
}



void init_GC4016_C(){
  int i;
 
  write16(base_GC214+offset_GC4016+reg_GC4016_C_PHASE,0x0000);
  write32(base_GC214+offset_GC4016+reg_GC4016_C_FREQ,0x2f430313);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_CHRESET,0x0C);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_FREQSYNC,0x77);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_NCOSYNC,0x22);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_ZEROPAD,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_DECSYNC,0x22);
  write16(base_GC214+offset_GC4016+reg_GC4016_C_DECRATIO,0x703f);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_CICSCALE,0x64);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_SPLITIQ,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_CFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_PFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_INPUT,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_PEAKCNTRL,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_C_PEAKCOUNT,00);
  write16(base_GC214+offset_GC4016+reg_GC4016_C_FINEGAIN,0x042e);

  for(i=0;i<11;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_C_CFIRCOEFFS+2*i,
            cfir_coeffs[i]);
  }
  for(i=0;i<32;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_C_PFIRCOEFFS+2*i,
            pfir_coeffs[i]);
  }	
}


void init_GC4016_D(){
  int i;
 
  write16(base_GC214+offset_GC4016+reg_GC4016_D_PHASE,0x0000);
  write32(base_GC214+offset_GC4016+reg_GC4016_D_FREQ,0x2f430313);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_CHRESET,0x0C);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_FREQSYNC,0x77);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_NCOSYNC,0x22);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_ZEROPAD,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_DECSYNC,0x22);
  write16(base_GC214+offset_GC4016+reg_GC4016_D_DECRATIO,0x703f);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_CICSCALE,0x64);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_SPLITIQ,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_CFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_PFIR,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_INPUT,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_PEAKCNTRL,0x00);
  write8(base_GC214+offset_GC4016+reg_GC4016_D_PEAKCOUNT,00);
  write16(base_GC214+offset_GC4016+reg_GC4016_D_FINEGAIN,0x042e);

  for(i=0;i<11;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_D_CFIRCOEFFS+2*i,
            cfir_coeffs[i]);
  }
  for(i=0;i<32;i++){
    write16(base_GC214+offset_GC4016+reg_GC4016_D_PFIRCOEFFS+2*i,
            pfir_coeffs[i]);
  }	
}







