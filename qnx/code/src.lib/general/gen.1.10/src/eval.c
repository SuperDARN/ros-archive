/* eval.c
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
 $Log: eval.c,v $
 Revision 1.3  2001/06/27 20:38:16  barnes
 Added license tag

 Revision 1.2  2001/05/10 15:27:53  barnes
 Removed the decode functions - these must be supplied by the caller.

 Revision 1.1  2001/05/10 15:20:42  barnes
 Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eval.h"

char *operator[]={"==","!=","<=",">=",
                  "||","&&",
                  "|","&","^",
                  "+","-","/","*","%",
                  "!","<",">",0};
char *opcode={"enlgoa|&^+-/*%!<>"};



/* NOTE - The postfix stack uses three arrays. 
 * The first identifies the type of object on the stack, 
 * either an operator or an operand.
 * The second contains the numeric ID of the operator
 * or the index of the operand. 
 * The final array contains the actual operands.
 * The index into the operand array is actually redundant
 * as the last entry of the operand array will always
 * correspond to the top operand put on the stack. 
 * Its just a convenient way of referencing things.
 */


int push_postfix(struct postfix *ptr,int type,void *data) {
  if (ptr->num==0) {
    ptr->type=malloc(1);
    ptr->object=malloc(sizeof(int));
  } else {
    ptr->type=realloc(ptr->type,ptr->num+1);
    ptr->object=realloc(ptr->object,(ptr->num+1)*sizeof(int));
  }
  ptr->type[ptr->num]=type;
  if (type !=0) ptr->object[ptr->num]=*((int *) data);
  else {
    if (ptr->vnum==0) ptr->value=malloc(sizeof(double));
    else ptr->value=realloc(ptr->value,(ptr->vnum+1)*sizeof(double));
    ptr->value[ptr->vnum]=*( (double *) data);
    ptr->object[ptr->num]=ptr->vnum;
    ptr->vnum++;
  }
  ptr->num++;
  return 0;
}

int pop_postfix(struct postfix *ptr,int *type,void *data) {
  if (ptr->num==0) return -1;
  ptr->num--;

  *type=ptr->type[ptr->num];
  if (*type !=0) *((int *) data)=ptr->object[ptr->num];
  else {
    *((double *) data)=ptr->value[ptr->object[ptr->num]];
    ptr->vnum--;
    if (ptr->vnum>0) ptr->value=realloc(ptr->value,ptr->vnum*sizeof(double));
    else free(ptr->value);
  }
  if (ptr->num>0) {
    ptr->type=realloc(ptr->type,ptr->num);
    ptr->object=realloc(ptr->object,sizeof(int)*ptr->num);
  } else {
    free(ptr->type);
    free(ptr->object);
  }
  return 0;
}

int push(struct stack *ptr,unsigned char c) {
  if (ptr->num==0) ptr->buf=malloc(1);
  else ptr->buf=realloc(ptr->buf,ptr->num+1);
  ptr->buf[ptr->num]=c;
  ptr->num++;
  return 0;
}


int pop(struct stack *ptr) {
  int c;
  if (ptr->num==0) return -1;
  ptr->num--;
  c=ptr->buf[ptr->num];
  if (ptr->num>0) ptr->buf=realloc(ptr->buf,ptr->num);
  else free(ptr->buf);
  return c;
}

int top(struct stack *ptr) {
  if (ptr->num==0) return -1; 
  return ptr->buf[ptr->num-1];
}

int precedence(int op) {
  switch (op) {
  case '+': 
  case '-':
    return 1;
    break;
  case '*':
  case '/':
    return 2;
    break;
  default:
    return 3;
  }
  return 3;
}

int optype(int op) {
  if (op=='$') return 1;
  if (op=='@') return 1;
  if (op=='!') return 1;
  return 2;
}

int find_object(char *ptr,int *op,int *step) {
  int k=0,j=-1;
  *op=-1;
  *step=1;
  while (ptr[k] !=0) {
    for (j=0;operator[j] !=NULL;j++) 
      if (strncmp(ptr+k,operator[j],strlen(operator[j]))==0) break;
    if (operator[j] !=NULL) break;
    if (ptr[k]=='(') break;
    if (ptr[k]==')') break;
    k++;
  }
  if (operator[j] !=NULL) {
   *op=opcode[j];
   *step=strlen(operator[j]);
  } else *op=ptr[k];
  return k;
}

int eval(char *expression,double *ptr,
         int (*dvar)(char *ptr,double *val,void *data),
         void *vdata,
         int (*dfun)(char *ptr,int argnum,double *argptr,
                     double *val,void *data),

         void *fdata) {

  int i=0,j=0,k=0;
  int c;
  int op;
  double value=0;
  double x,y;
  int type;
  int state=0;
  int step;

  struct stack stack;
  struct postfix output;
  struct postfix evaluate;

  unsigned char buf[8];
  char *sum=NULL;

  memset(&stack,0,sizeof(struct stack));
  memset(&output,0,sizeof(struct postfix));
  memset(&evaluate,0,sizeof(struct postfix));


  /* pre-parse the sum removing white space */

  sum=malloc(strlen(expression)+1);
  while (expression[i] !=0) {
    if (expression[i] !=' ') {
     sum[j]=expression[i];
     j++;
    }
    i++;
  }
  sum[j]=0;
  i=0;

 

  

  while (sum[i] !=0) {
   
    k=find_object(sum+i,&op,&step);
  
    if (k>0) {


      /* we have an operand.
       * This can be either a number, eg 3.413
       * A variable name, eg xmin
       * or a function call cos(....)
       */


      if (op=='(') { /* function call */
	/* find matching bracket and pass substring to 
           function processor */

        int l,m,n,s=1;
        char *argbuf=NULL;
        char *name=NULL;
        double *args=NULL;
        int argnum=0;

        l=k+1;
        while ((sum[i+l] !=0) && (s !=0)) {
          if (sum[i+l]=='(') s++;
          if (sum[i+l]==')') s--;
          l++;
	}
        if ((l-k-1)>0) {
          argbuf=malloc(l-k-1);
          memcpy(argbuf,sum+i+k+1,l-k-2);
          argbuf[l-k-2]=0;
          m=0;n=0;

          while (argbuf[n] !=0) {
            if (argbuf[n]==',') {
               argbuf[n]=0;
               if (args==NULL) args=malloc(sizeof(double));
               else args=realloc(args,sizeof(double)*(argnum+1));
               s=eval(argbuf+m,&args[argnum],dvar,vdata,dfun,fdata);
               if (s !=0) {
  		  free(sum);
                  return s;
	       }
               argnum++;
               m=n+1;
            }
            n++; 
	  }
      
  
         if (m !=n) {
            if (args==NULL) args=malloc(sizeof(double));
            else args=realloc(args,sizeof(double)*(argnum+1));
            s=eval(argbuf+m,&args[argnum],dvar,vdata,dfun,fdata);
            if (s !=0) {
	       free(sum);
               return s;
	    }
            argnum++;
         }
         free(argbuf);
	}
        name=malloc(k+1);
        memcpy(name,sum+i,k);
        name[k]=0;


	s=(*dfun)(name,argnum,args,&value,fdata);
        free(name);
        if (s !=0) {
 	   free(sum);
           return 4;
	}
	k=l;
      } else {
	int s;

        char *name=NULL;
        name=malloc(k+1);
        memcpy(name,sum+i,k);
        name[k]=0;

        s=(*dvar)(name,&value,vdata);
        free(name);
        if (s !=0) {
          free(sum);
          return 3;
	}
      }
      
      push_postfix(&output,0,&value);
      state=1;
    } else if (op=='(') { 
      push(&stack,'(');
      state=2;
    } else if (op==')') {

      c=pop(&stack);
      while ((stack.num !=0) && (c !='(')) {
        push_postfix(&output,optype(c),&c);
        c=pop(&stack);
      }
      if ((c !='(') && (stack.num==0)) {
        free(sum);
        return 2;
      }
      state=3;
    } else {
      if (state==0) {
        if (op=='+') op='@';
        else if (op=='-') op='$';
      } 
      while ((stack.num !=0) && (top(&stack) !='(') &&
             (precedence(top(&stack))>=precedence(op))) {
	c=pop(&stack);
        push_postfix(&output,optype(c),&c);
      }
      push(&stack,op);
      state=0;
    }
    if (k==0) k+=step;
    i+=k;
  }
 
 

  while (stack.num !=0) {
    c=pop(&stack);
    if (c=='(') {
       free(sum);
       return 2;
    }
    push_postfix(&output,optype(c),&c);
  }

  for (i=0;i<output.num;i++) {
    if (output.type[i]==0) push_postfix(&evaluate,0,
                                        &output.value[output.object[i]]);
   
    else if (output.type[i]==1) {
      pop_postfix(&evaluate,&type,buf);
      x=*((double *) buf);
      switch (output.object[i]) {
        case '@' :
          value=+x;
          break;
        case '$' :
          value=-x;
          break;
        case '!' : 
          value=!x;
        }
        push_postfix(&evaluate,0,&value);
    } else {
    
      pop_postfix(&evaluate,&type,buf);
   
      x=*((double *) buf);
        
      pop_postfix(&evaluate,&type,buf);
      y=*((double *) buf);
     
      switch (output.object[i]) {
      case '*' :
        value=y*x;
        break;
      case '/' :
        value=y/x;
        break;
      case '%' :
        value=y- (int) (y / x);
        break;
      case '+' :
        value=y+x;
        break;
      case '-' :
        value=y-x;
        break;
      case '<' :
        value=y<x;
        break;
      case '>' :
        value=y>x;
        break;
      case 'e' :
        value=(y==x);
        break;
      case 'n' :
        value=(y!=x);
        break;
      case 'l' :
        value=(y<=x);
        break;
      case 'g' :
        value=(y>=x);
        break;
      case '|' :
        value=(int) y | (int) x;
        break;
      case '&' :
        value=(int) y & (int) x;
        break;
      case '^' :
        value=(int) y ^ (int) x;
        break;
     case 'a' :
        value=(int) y && (int) x;
        break;
      case 'o' :
        value=(int) y || (int) x;
        break;


      }
      push_postfix(&evaluate,0,&value);
    }
  }
  *ptr=evaluate.value[0];
  free(sum);
  return 0;
}




