/* shell.c
   =======
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
 $Log: shell.c,v $
 Revision 1.10  2004/06/22 12:58:41  barnes
 QNX4 Code audit.

 Revision 1.9  2004/05/11 15:02:01  barnes
 Added ipcid_t type for IPC calls.

 Revision 1.8  2001/06/27 20:56:14  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:12:07  barnes
 Added Author Name

 Revision 1.6  1999/09/15 02:16:44  barnes
 Added generic int type for 64-bit issues.

 Revision 1.5  1999/03/29 18:56:14  barnes
 Fixed bug in header.

 Revision 1.4  1998/11/09 23:01:11  barnes
 Removed redundant header.

 Revision 1.3  1998/11/04 20:57:45  barnes
 Added extra informational messages.

 Revision 1.2  1998/11/04 19:30:26  barnes
 Fixed bug in decoding strings.

 Revision 1.1  1998/10/28 15:51:50  barnes
 Initial revision

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>

#include "rtypes.h"
#include "radarshell.h"
#define TRUE 1
#define FALSE 0

#define DELIM  " \n"

#define PROMPT ">"
#define MAX_INPUT_LINE 256
  
FILE *input_fp=stdin;
FILE *output_fp=stdout;
	
struct varlist {
  short int type;
  short int cflag;
  void *data;
  long int copy;
  long int next;
};


int shell(char *buffer) {
 /* check for user interruptions */
   int status;
   int input_fd;
   int wait=TRUE;
   
   char input_line[MAX_INPUT_LINE];
   char parse_line[MAX_INPUT_LINE];
   char *variable;
   char *command;
   char *value;
   struct varlist *var_ptr=NULL;   
	  
   input_fd = fileno(input_fp);
   
   tcflush(input_fd,TCIFLUSH);

   while (wait) {
     fprintf(output_fp,PROMPT);
     input_line[0] = NULL;

     /* get a command line */

     fgets(input_line,MAX_INPUT_LINE,input_fp); 
	 strcpy(parse_line,input_line); 
     command=strtok(parse_line,DELIM);
	 
	 if (command !=NULL) {
	   if (strcmp(command,"go")==0) wait=0; /* return to control program */
	   else if (strcmp(command,"show")==0) { /* list variables */
		 variable=strtok(NULL,DELIM);
		 if (variable !=NULL) { /* list specific variable */
           while (variable !=NULL) {
		     var_ptr=(struct varlist *) buffer;
             while (1) {
               if (strcmp(variable,(char *) (var_ptr+1))==0) { 
                 printf("%s=",(char *) (var_ptr+1));
	             switch (var_ptr->type) {
		       case var_SHORT:
		         printf("%d\n",*((short int *) 
			       ( (int) var_ptr+var_ptr->copy)));
         		 break;
                       case var_LONG:
		         printf("%ld\n",*((long int *) 
			       ( (int) var_ptr+var_ptr->copy)));
         		 break;

		       case var_FLOAT:
		       	 printf("%g\n",*((float *) ( (int) var_ptr+
						 	var_ptr->copy)));
         		 break;
		       case var_DOUBLE:
		       	 printf("%g\n",*((double *) ( (int) var_ptr+
						 	var_ptr->copy)));
         		 break;	
		       case var_STRING:
			 if (var_ptr->data == NULL) printf("NULL\n");
			 else printf("%s\n",
				     (char *) ( (int) var_ptr+
					      var_ptr->copy)); 
                         break;
		       default :
		       	 printf("%d\n",*((int *) 
					 ( (int) var_ptr+var_ptr->copy)));
			 break;
		       }
		     }	 	  
		   if (var_ptr->next==0) break;											
	           var_ptr=(struct varlist *) ( (int) var_ptr+var_ptr->next);
             }  
		     variable=strtok(NULL,DELIM);
		   }		   
		 } else { /* list all */
		   var_ptr=(struct varlist *) buffer;
           while (1) {
             printf("%s=",(char *) (var_ptr+1));
	         switch (var_ptr->type) {
		   case var_SHORT:
		     printf("%d\n",*((short int *) ( (int) var_ptr+
				    var_ptr->copy)));
         	     break;
                  case var_LONG:
		     printf("%ld\n",*((long int *) ( (int) var_ptr+
				    var_ptr->copy)));
         	     break;

		   case var_FLOAT:
		     printf("%g\n",*((float *) ( (int) var_ptr+var_ptr->copy)));
         	     break;
		   case var_DOUBLE:
		     printf("%g\n",*((double *) ( (int) var_ptr+var_ptr->copy)));
         	     break;	
		   case var_STRING:
		     if (var_ptr->data == NULL) printf("NULL\n");
				 else printf("%s\n",(char *) ( (int) var_ptr+
				 var_ptr->copy));  
                     break;  	 				   
		     default :
		       printf("%d\n",*((int *) ( (int) var_ptr+
				 								var_ptr->copy)));
			     break;
			 }	 	  

		     if (var_ptr->next==0) break;											
	         var_ptr=(struct varlist *) ( (int) var_ptr+var_ptr->next);  		   
           }
		 }  
       } else if (strchr(input_line,'=') !=NULL) { /* assign variables */	     
         int i;
         strcpy(parse_line,input_line); 
         status=((variable=strtok(parse_line," =\n")) !=NULL);
         status=(status) ? ((value=strtok(NULL,"\n")) !=NULL) : FALSE;
         /* remove trailing spaces from the variable name */
         for (i=0;(variable[i] != ' ') && (variable[i] != 0);i++);
         variable[i]=0;
         for (i=0;(value[i] ==' ') && (value[i] !=0);i++);
         if (i>0) value+=i;

         /* strip the leading spaces of the value argument */

         if (status) { /* do the assignment */
		   var_ptr=(struct varlist *) buffer;
           while (1) {
             if (strcmp(variable,(char *) (var_ptr+1))==0) { 
               var_ptr->cflag=1;
	           switch (var_ptr->type) {
			     case var_SHORT:
		       	   *((short int *) ( (int) var_ptr+var_ptr->copy))=
                     atoi(value);
         		   break;
                            case var_LONG:
		       	   *((long int *) ( (int) var_ptr+var_ptr->copy))=
                     atol(value);
         		   break;

				 case var_FLOAT:
		       	   *((float *) ( (int) var_ptr+var_ptr->copy))=atof(value);
         		   break;
				 case var_DOUBLE:
		       	   *((double *) ( (int) var_ptr+var_ptr->copy))=
				   atof(value);;
         		   break;	
				 case var_STRING:
				   if (var_ptr->data !=NULL) {
				     strncpy( (char *) ( (int) var_ptr+var_ptr->copy),
					   		value,STR_MAX);
				   }
				   break;  		 
				 default :
		       	   *((int *) ( (int) var_ptr+var_ptr->copy))=
                   atoi(value);
				   break;	   
			   }
			 }	 	  
		     if (var_ptr->next==0) break;											
	         var_ptr=(struct varlist *) ( (int) var_ptr+var_ptr->next);
           }  	   		   
         }
	   }
	 } 
   } 
   return 0;
} 		  

 


 
