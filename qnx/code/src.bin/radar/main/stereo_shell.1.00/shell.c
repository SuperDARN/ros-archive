/*
	$Log: shell.c
	Revision 1.8 2002/10/07 00:00:00
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>

#include <unistd.h>
#include <time.h>
#include "radar_shell.h"

void SetVar(char *variable, char *buffer, int value);

#define TRUE 1
#define FALSE 0

#define DELIM  " \n"
#define ALTER_NAME "/alter"

#define ALTER 'a'

#define PROMPT ">"
#define MAX_INPUT_LINE 256
  
FILE *input_fp=stdin;
FILE *output_fp=stdout;
	
struct varlist
{
	short int type;
	short int cflag;
	void *data;
	long int copy;
	long int next;
};

struct
{
	unsigned short bmA, bmB;
	short bA[10], bB[10];
	short intt;
	short error;
	time_t time;
}
shelldata;

extern int error;

long FindVar(char *buff, char *buffer)
{
	int ret = -1;
	struct varlist *var_ptr = (struct varlist *) buffer;

	while (1)
	{
		if (strcmp(buff, (char *) (var_ptr+1)) == 0)
		{
			return *((long int *) ((int) var_ptr+var_ptr->copy));
		}	 	  

		if (var_ptr->next == 0) break;											
		var_ptr = (struct varlist *) ((int) var_ptr+var_ptr->next);  		   
	}

	return (ret);
}

int shell(char *buffer)
{
	int status;
	int wait=TRUE;
	int ptr;
	int i;

	char input_line[MAX_INPUT_LINE];
//	char parse_line[MAX_INPUT_LINE];
	char *variable;
	char *command;
	char *value;
	struct varlist *var_ptr=NULL;   
	char buff[200];
	  
// find the stereo variable and load into the shelldata structure

	shelldata.intt = FindVar("intt", buffer);

// frequency bands

	for (i = 0; i < 10; i++)
	{
		sprintf(buff, "b%dA", i);
		shelldata.bA[i] = FindVar(buff, buffer);

		sprintf(buff, "b%dB", i);
		shelldata.bB[i] = FindVar(buff, buffer);
	}

// beams

	shelldata.bmA = shelldata.bmB = 0;

	for (i = 0; i < 16; i++)
	{
		sprintf(buff, "bm%dA", i);
		shelldata.bmA |= 1 << FindVar(buff, buffer);

		sprintf(buff, "bm%dB", i);
		shelldata.bmB |= 1 << FindVar(buff, buffer);
	}

	shelldata.time  = time(NULL);
	shelldata.error = error;

// send data to GUI

	write(STDOUT_FILENO, &shelldata, sizeof(shelldata));

// wait for new data from GUI

	if (read(STDIN_FILENO, &shelldata, sizeof(shelldata)) == 0)
	{
		exit(0);
	}

	if (shelldata.error != 0) return 0;

	SetVar("intt",   buffer, shelldata.intt);

	for (i = 0; i < 16; i++)
	{
		if (shelldata.bmA & (1 << i))
		{
			sprintf(buff, "bm%dA", i);

			SetVar(buff, buffer, i);
		}
		else
		{
			sprintf(buff, "bm%dA", i);

			SetVar(buff, buffer, -1);
		}

		if (shelldata.bmB & (1 << i))
		{
			sprintf(buff, "bm%dB", i);

			SetVar(buff, buffer, i);
		}
		else
		{
			sprintf(buff, "bm%dB", i);

			SetVar(buff, buffer, -1);
		}
	}

	for (i = 0; i < 10; i++)
	{
		sprintf(buff, "b%dA", i);

		SetVar(buff, buffer, shelldata.bA[i]);

		sprintf(buff, "b%dB", i);

		SetVar(buff, buffer, shelldata.bB[i]);
	}

	return 1;
}

// assignment

void SetVar(char *variable, char *buffer, int val)
{
	struct varlist *var_ptr=NULL;  
//	char value[20];
	int i;

//	sprintf(value, "%d", val); 

//	if (strchr(input_line,'=') != NULL)
//	{
//		int i;

//		strcpy(parse_line,input_line); 
//		status=((variable=strtok(parse_line," =\n")) != NULL);
//		status=(status) ? ((value=strtok(NULL,"\n")) != NULL) : FALSE;

	/* remove trailing spaces from the variable name */

//		for (i=0;(variable[i] != ' ') && (variable[i] != 0);i++);

//		variable[i]=0;

//		for (i=0;(value[i] ==' ') && (value[i] !=0);i++);

//		if (i>0) value += i;

	/* strip the leading spaces of the value argument */

//		if (status)
		{
			var_ptr = (struct varlist *) buffer;

			while (1)
			{
				if (strcmp(variable,(char *) (var_ptr+1)) == 0)
				{
					var_ptr->cflag = 1;

					switch (var_ptr->type)
					{
/*
						case var_SHORT:
							*((short int *) ((int) var_ptr+var_ptr->copy)) = atoi(value);
							break;
*/
						case var_LONG:
							*((long int *) ((int) var_ptr+var_ptr->copy)) = val;
							break;
/*
						case var_FLOAT:
							*((float *) ((int) var_ptr+var_ptr->copy))=atof(value);
							break;
*/
/*
						case var_DOUBLE:
							*((double *) ((int) var_ptr+var_ptr->copy)) = atof(value);
							break;
*/
/*
						case var_STRING:
							if (var_ptr->data !=NULL)
							{
								strncpy((char *) ((int) var_ptr+var_ptr->copy), value, STR_MAX);
							}
							break;  		 
*/
/*
						default:
							*((int *) ((int) var_ptr+var_ptr->copy)) = atoi(value);
							break;	   
*/
						default:	break;
					}
				}	 	  

				if (var_ptr->next==0) break;											

				var_ptr=(struct varlist *) ((int) var_ptr+var_ptr->next);
			}  	   		   
		}
//	} 

//	return 0;
} 		  
