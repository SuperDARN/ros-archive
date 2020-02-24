/*
	$Log: main.c
	Revision 1.8 2002/10/07 00:00:00
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <string.h>
#include <sys/sendmx.h>
#include <termios.h>
#include <unistd.h>
#include "print_info.h"
#include "task_name.h"
#include "hlpstr.h"

#define SHELL_SEND 's'
#define SHELL_SEND_MSG 'm'
#define SHELL_SEND_ERR 'e'
#define SHELL_REPLY 'r'

extern FILE *input_fp;
extern FILE *output_fp;

char *buffer=NULL;
char control_name[32]={CONTROL_NAME};

struct _data
{
	unsigned short bmA;
	unsigned short bmB;
	signed short bA[10];
	signed short bB[10];
	short intt;
	short error;
	time_t time;
};

extern struct _data shelldata;

int error = 0;

void main(int argc,char *argv[])
{
	char msg;
	pid_t control_pid;
	int size = 0;
	struct _mxfer_entry smx[1];
	struct _mxfer_entry rmx[3];

	shelldata.error = 0;

// locate control process

	if ((control_pid = qnx_name_locate(0, control_name, 0, 0)) == -1)
	{
		shelldata.error = 1;

		write(STDOUT_FILENO, &shelldata, sizeof(shelldata));
		exit(0);
	}

// find the size of the buffer required

	msg = SHELL_SEND;

	_setmx(&smx[0],  &msg, sizeof(msg));
	_setmx(&rmx[0], &size, sizeof(size));

	if (Sendmx(control_pid, 1, 1, smx, rmx) == -1)
	{
		shelldata.error = 2;
	}  
	else
	{
		if (size != 0)
		{
			buffer = malloc(size);
		}

		if (buffer == NULL)
		{
			shelldata.error = 3;
		}
	}

	if (shelldata.error != 0)
	{
		msg = SHELL_SEND_ERR;
		_setmx(&smx[0], &msg, sizeof(msg));
		Sendmx(control_pid, 1, 0, smx, NULL);

		write(STDOUT_FILENO, &shelldata, sizeof(shelldata));
		exit(0);
	}

	while (1)
	{
		msg = SHELL_SEND_MSG;
  
		_setmx(&smx[0], &msg, sizeof(msg));
		_setmx(&rmx[0], buffer, size);

		if (Sendmx(control_pid, 1, 1, smx, rmx) != 0) error = 4;
     
	/* enter the shell */

		if (shell(buffer))
		{      
			msg = SHELL_REPLY;

			_setmx(&rmx[0],&msg,sizeof(msg));
			_setmx(&rmx[1],&size,sizeof(size));
			_setmx(&rmx[2],buffer,size);
	
			if (Sendmx(control_pid,3,0,rmx,0) != 0) error = 4;
		}

	/* request parameters */

		msg = SHELL_SEND;

		_setmx(&smx[0],  &msg, sizeof(msg));
		_setmx(&rmx[0], &size, sizeof(size));

		if (Sendmx(control_pid, 1, 1, smx, rmx) == -1)
		{
			shelldata.error = 4;
		}  
	}

	exit(0);
}
