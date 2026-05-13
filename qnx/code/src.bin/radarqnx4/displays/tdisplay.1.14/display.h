/* display.h
   =========
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

/*
 $Log: display.h,v $
 Revision 1.1  2004/07/13 22:59:44  barnes
 Initial revision

*/

#define TRUE 1
#define FALSE 0

void draw_banner(char *name,int key);
int convert_snr(int power,int noise);
void plot(int x,int y,int scale,double value,double max_val); 
void draw_labels(char *str);
void draw_axis(struct RadarParm *prm,int range);
void draw_acf_banner(char *name,int range);
void draw_frame(int roff, int max_range);
void plot_acf(struct RadarParm *prm,struct RawData *raw,int range,int roff);
void plot_velocity(struct RadarParm *prm,struct FitData *fit,int roff);
void plot_power(struct RadarParm *prm,struct FitData *fit,int roff);
void plot_width(struct RadarParm *prm,struct FitData *fit,int roff);
void plot_param(struct RadarNetwork *network,struct RadarParm *prm,char *text,
                char *data,char *program,int aflg);
void draw_channel();
