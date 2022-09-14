/* entity.h
   ======== 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/




#ifndef _ENTITY_H
#define _ENTITY_H

struct Entitydata {
  struct {
    int num;
    struct XMLDBbuffer **buf;
    int *state;
  } entity;
  struct {
    int num;
    struct XMLDBbuffer **buf;
    int *state;
  } symbol;
  char *buf;
  int sze;
  int max;
  int stp;
  struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;

  struct {
    int (*trap)(char *,int,void *);
    void *data;
  } decode;

 struct {
    int (*trap)(char *,int,void *);
    void *data;
  } encode;


};

struct Entitydata *EntityMake();

void EntityFree(struct Entitydata *ptr);

void EntityFreeSymbol(struct Entitydata *ptr);
void EntityFreeEntity(struct Entitydata *ptr);

int EntitySetText(struct Entitydata *ptr,
		  int (*text)(char *,int,
			      void *),void *data);


int EntitySetEncodeTrap(struct Entitydata *ptr,
		  int (*text)(char *,int,
			      void *),void *data);

int EntitySetDecodeTrap(struct Entitydata *ptr,
		        int (*text)(char *,int,
			      void *),void *data);



int EntityAddEntity(struct Entitydata *ptr,struct XMLDBbuffer *entity);

int EntityAddSymbol(struct Entitydata *ptr,struct XMLDBbuffer *symbol);


int EntityAddEntityString(struct Entitydata *ptr,char *str);
int EntityAddSymbolString(struct Entitydata *ptr,char *str);

int EntityAddEntityStringArray(struct Entitydata *ptr,char **str);
int EntityAddSymbolStringArray(struct Entitydata *ptr,char **str);

int EntityDecode(char *buf,int sze,void *data);
int EntityEncode(char *buf,int sze,void *data);


struct XMLDBbuffer *EntityDecodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src);
struct XMLDBbuffer *EntityEncodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src);

#endif


