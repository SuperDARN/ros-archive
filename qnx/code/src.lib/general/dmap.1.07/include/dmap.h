/* dmap.h
   ====== 
   Author: R.J.Barnes
*/


/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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

/*
 $Log: dmap.h,v $
 Revision 1.1  2004/07/12 21:43:44  barnes
 Initial revision

*/


#ifndef _DMAP_H
#define _DMAP_H


#define DATACHAR 1
#define DATASHORT 2
#define DATAINT 3
#define DATAFLOAT 4
#define DATADOUBLE 8
#define DATASTRING 9


/* Define an unique code value for this version of the library. */

#define DATACODE  0x00010001


union DataMapPointer {
  char *cptr;
  int16 *sptr;
  int32 *lptr;
  float *fptr;
  double *dptr;
  void *vptr;
};

struct DataMapScalar {
  char *name;
  unsigned char type;
  union DataMapPointer data;
};

struct DataMapArray {
    char *name;
    unsigned char type;
    int32 dim;
    int32 *rng;
    union DataMapPointer data;
};

struct DataMap {
  int sze;
  unsigned char *buf;
  int snum;
  int anum;
  struct DataMapScalar **scl;
  struct DataMapArray **arr;
}; 

struct DataMap *DataMapMake();

struct DataMapScalar* DataMapMakeScalar(char *name,int type,
                                                void *data);
 
void DataMapFreeScalar(struct DataMapScalar *ptr);

int DataMapAddScalar(struct DataMap *ptr,
                         char *name,int type,void *data);

struct DataMapArray* DataMapMakeArray(char *name,int type,int dim,
                                              int32 *rng,void *data);
 

void DataMapFreeArray(struct DataMapArray *ptr);
 
int DataMapAddArray(struct DataMap *ptr,
                        char *name,int type,int dim,
                        int32 *rng,void *data);
 
int DataMapSize(struct DataMap *write);

unsigned char *DataMapEncodeBuffer(struct DataMap *ptr,int *size);

int DataMapWrite(int fid,struct DataMap *ptr);
 
void DataMapFree(struct DataMap *ptr);
struct DataMap *DataMapRead(int fid);

int DataMapFwrite(FILE *fp,struct DataMap *ptr);
struct DataMap *DataMapFread(FILE *fp);

struct DataMap *DataMapDecodeBuffer(unsigned char *buf,int size);

#endif
