/* rplot.c
   ======= */

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


#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"
#include "rps.h"
#include "rplot.h"
#include "rplotin.h"
#include "fontdb.h"
#include "imagedb.h"
#include "imagedb.h"
#include "fbrplot.h"
#include "psrplot.h"
#include "rplotrplot.h"

#include "hlpstr.h"

/*
 $Log: rplot.c,v $
 Revision 1.4  2005/08/05 20:25:05  barnes
 Added option to dump the command line options

 Revision 1.3  2004/06/21 21:34:33  barnes
 Added missing header.

 Revision 1.2  2004/05/13 19:53:55  barnes
 Code Audit.

 Revision 1.1  2003/03/14 18:18:11  barnes
 Initial revision

*/

#ifdef _XLIB_
#include "xwin.h"
#endif

struct OptionData opt;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}


int clean_newline(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  if ((sze==1) && (buf[0]==0x0a)) return 0;
  fwrite(buf,sze,1,stdout);
  return 0;
}

void loadppm(struct FrameBufferDB *imagedb,struct OptionText *ppm) {
 FILE *fp;
 int i;
 struct FrameBuffer *img=NULL;
 if (ppm==NULL) return;
 for (i=0;i<ppm->num;i++) {
    fp=fopen(ppm->txt[i],"r");
    if (fp==NULL) continue;
    img=FrameBufferLoadPPM(fp,ppm->txt[i]);
    fclose(fp);
    if (img==NULL) continue;
    FrameBufferDBAdd(imagedb,img);
  }
}

void loadppmx(struct FrameBufferDB *imagedb,struct OptionText *ppmx) {
 FILE *fp;
 int i;
 struct FrameBuffer *img=NULL;
 if (ppmx==NULL) return;
 for (i=0;i<ppmx->num;i++) {
    fp=fopen(ppmx->txt[i],"r");
    if (fp==NULL) continue;
    img=FrameBufferLoadPPMX(fp);
    fclose(fp);
    if (img==NULL) continue;
    FrameBufferDBAdd(imagedb,img);
  }
}


void loadxml(struct FrameBufferDB *imagedb,struct OptionText *xml) {
  struct FrameBuffer *img=NULL;
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;

  FILE *fp;
  char lbuf[256];
  int i;
  int s=0;

  if (xml==NULL) return;

  xmldata=XMLMake();
  xmlimg.xml=xmldata;
  xmlimg.img=&img;

  XMLSetStart(xmldata,FrameBufferXMLStart,&xmlimg);
  XMLSetEnd(xmldata,FrameBufferXMLEnd,&xmlimg);

  for (i=0;i<xml->num;i++) {
    fp=fopen(xml->txt[i],"r");
    if (fp==NULL) continue;
    while(fgets(lbuf,255,fp) !=NULL) {
      s=XMLDecode(xmldata,lbuf,strlen(lbuf));
      if (s !=0) break;
    }
    fclose(fp);
    if (s !=0) continue;
    if (img==NULL) continue;
    FrameBufferDBAdd(imagedb,img);
    img=NULL;
  }
  XMLFree(xmldata);
}




int main(int argc,char *argv[]) {
 

#ifdef _XLIB_
  char *name="rplot";
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  int xdoff=-1;
  int ydoff=-1;
#endif


  int s=0;
  int arg=0;
  FILE *fp;
  char lbuf[256];
  struct FrameBuffer *img=NULL;
  struct FrameBufferRplot *fbplot=NULL;
  struct PostScriptRplot *psplot=NULL;
  struct PostScript *psdata=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  struct FrameBufferDB *imagedb=NULL;
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  struct RplotRplot *rplotrplot=NULL;
  struct Rplot *rplot=NULL;
 
  unsigned char land=0;
  float xpoff=40,ypoff=40;
  
  char *fntdbfname=NULL;
  unsigned char psflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char rflg=0;
  unsigned char gflg=0;
  unsigned char pflg=0;

  unsigned char help=0;
  unsigned char option=0;

 
  struct OptionText *ppmnme=NULL;
  struct OptionText *ppmxnme=NULL;
  struct OptionText *xmlnme=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"ps",'x',&psflg);
  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);
  OptionAdd(&opt,"land",'x',&land);
  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);

  OptionAdd(&opt,"ippm",'a',&ppmnme);
  OptionAdd(&opt,"ippmx",'a',&ppmxnme);
  OptionAdd(&opt,"ixml",'a',&xmlnme);
 
#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
#endif

  OptionAdd(&opt,"fdb",'t',&fntdbfname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (psflg) pflg=1;
  if (xmlflg) gflg=1; 
  if (ppmflg) gflg=1;
  if (ppmxflg) gflg=1;

#ifdef _XLIB_ 
   if (xd !=0) {
     pflg=0; 
     gflg=1;
   }
#endif
     
  if (pflg) gflg=0;

  if ((pflg==0) && (gflg==0)) rflg=1;

  if (gflg) {
    if (fntdbfname==NULL) fntdbfname=getenv("FONTDB");
    if (fntdbfname !=NULL) {
      fp=fopen(fntdbfname,"r");
      if (fp !=NULL) {
        fontdb=FrameBufferFontDBLoad(fp);
        fclose(fp);
      }
    }
    if (fontdb==NULL) {
      fprintf(stderr,"Could not load fonts.\n");
      exit(-1);
    }
  }
   
  imagedb=FrameBufferDBMake();
  loadxml(imagedb,xmlnme);
  loadppm(imagedb,ppmnme);
  loadppmx(imagedb,ppmxnme);

  xmldata=XMLMake();
  rplotin=RplotInMake(xmldata);

  if (gflg) {
      fbplot=FrameBufferRplotMake(fontdb,imagedb);
      FrameBufferRplotSetAll(rplotin,fbplot);
  }
  if (pflg) {
    psdata=PostScriptMake();
    PostScriptSetText(psdata,stream,stdout);
    psplot=PostScriptRplotMake(psdata,imagedb);
    PostScriptRplotSetAll(rplotin,psplot);
    psplot->land=land;
    psplot->xoff=xpoff;
    psplot->yoff=ypoff;
  }
  if (rflg) {
    rplot=RplotMake();
    RplotSetText(rplot,stream,stdout);
    rplotrplot=RplotRplotMake(rplot,imagedb);
    RplotRplotSetAll(rplotin,rplotrplot);
    RplotInSetPass(rplotin,clean_newline,stdout);
  }

  XMLSetStart(xmldata,RplotInStart,rplotin);
  XMLSetEnd(xmldata,RplotInEnd,rplotin);
  XMLSetText(xmldata,RplotInText,rplotin);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  while(fgets(lbuf,255,fp) !=NULL) {
          
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;

  }
  if (s !=0) {
    fprintf(stderr,"Error parsing document.\n");
    exit(0);
  }
 
  if (arg<argc) fclose(fp);
  if (fbplot==NULL) exit(0);
  if (fbplot->img==NULL) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }
  img=fbplot->img;

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-img->wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-img->hgt)/2;


    win=XwinMakeWindow(xdoff,ydoff,img->wdt,img->hgt,0,
                       dp,name,
                       name,argv[0],name,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }

    XwinFrameBufferWindow(img,win);

    XwinShowWindow(win);

    XwinDisplayEvent(dp,1,&win,1,NULL);

    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  } else {
    if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
    else FrameBufferSavePPMX(img,stdout);
  }
  #else 
  if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  #endif
  return 0;
}




