/* pragma.h
   ========
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
 $Log: pragma.h,v $
 Revision 1.1  2004/07/13 22:53:19  barnes
 Initial revision

*/


/*

	some handy pragma's for low-level work:	

void	ll_debug(void);
	-	issues an int f3h which puts you in to the low level debugger.

void	delay_8X(int count)
	-	delays approximately 8 clocks * count.
		useful for 'little' timing loops

unsigned	pswget(void);
	-	returns the current value of the flags register.

void	restore(unsigned psw)
	-	stores psw into flags register.

unsigned	disable(void);
	-	disables interrupts, and returns previous value of flags.

unsigned	enable(void);
	-	enables interrupts, and returns previous value of flags.

unsigned	str_forward(void);
	-	sets direction flag to forward and returns previous value of flags.

unsigned	str_reverse(void);
	-	sets direction flag to reverse and returns previous value of flags.

unsigned	my_cs(void)
	-	returns current value of 'cs' register.

unsigned	my_ds(void)
	-	returns current value of 'ds' register.

void	io_safe(void);
	-	does 2 "jmp ." to avoid trashing io device.
*/


void	delay_8X(unsigned count);
#pragma aux delay_8X =	 					\
			0xe2 0xfe	/*	loop -2	*/		\
			parm nomemory [cx]				\
			modify exact nomemory [cx];



void	ll_debug(void);
#pragma	aux	ll_debug = \
			0xcd 0xf3	/*	int 0f3h	*/	\
			parm nomemory []         		\
			modify exact nomemory [];

void	io_safe(void);
#pragma	aux	io_safe = \
			0xeb 0x00	/*	jmp .	*/	\
			0xeb 0x00	/*	jmp .	*/	\
			parm nomemory []         		\
			modify exact nomemory [];

unsigned pswget(void);
#pragma aux pswget = \
			0x9c				/*	pushf	*/ \
			0x58				/*	pop ax  */ \
			parm nomemory	[]					\
			modify exact nomemory [ AX ];

unsigned disable(void);
#pragma aux disable = \
			0x9c				/*	pushf	*/ \
			0xfa				/*	cli		*/ \
			0x58				/*	pop ax  */ \
			parm nomemory	[]					\
			modify exact nomemory [ AX ];

unsigned enable(void);
#pragma	aux	enable = \
			0x9c				/*	pushf	*/ \
			0xfb				/*	sti		*/ \
			0x58				/*	pop ax  */ \
			parm nomemory []					\
			modify exact nomemory [ AX ];
			

unsigned str_forward(void);
#pragma	aux str_forward = \
			0x9c				/*	pushf	*/ \
			0xfc				/*	cld		*/ \
			0x58				/*	pop ax  */ \
			parm nomemory	[]					\
			modify exact nomemory [ AX ];

unsigned str_reverse(void);
#pragma	aux cforward = \
			0x9c				/*	pushf	*/ \
			0xfd				/*	cld		*/ \
			0x58				/*	pop ax  */ \
			parm nomemory	[]					\
			modify exact nomemory [ AX ];

void restore(unsigned psw);
#pragma	aux	restore = \
			0x50				/*	push	ax	*/ \
			0x9d				/*	popf		*/ \
			parm nomemory [ AX ]                	\
			modify exact nomemory [];

unsigned short my_cs( void );
#pragma aux my_cs =															\
			0x8c 0xc8			/* mov ax,cs	*/							\
			parm nomemory [] value [ AX ]									\
			modify exact nomemory [ AX ];

unsigned short my_ds( void );
#pragma aux my_ds =															\
			0x8c 0xd8			/* mov ax,ds	*/							\
			parm nomemory [] value [ AX ]									\
			modify exact nomemory [ AX ];

typedef struct {		/* Used to get quotient and remainder out of a 32	*/
	unsigned quot;		/*	by 16 bit divide.								*/
	unsigned rem;
	} udiv_t;

udiv_t div32_16( unsigned long, unsigned );
#pragma aux div32_16 =														\
			0xf7 0xf3			/* div bx	*/								\
			parm nomemory [ DX AX ] [ BX ] value [ DX AX ]					\
			modify exact nomemory [ DX AX ];

void inp_words( char __far *buffer, unsigned count, unsigned port );
#pragma aux inp_words =														\
		0xfc				/*	cld										*/	\
		0xf2 0x6d			/*	rep insw			; Read them in		*/	\
		parm nomemory [ ES DI ] [ CX ] [ DX ]								\
		modify exact nomemory [ CX DI ];

void outp_words( char __far *buffer, unsigned count, unsigned port );
#pragma aux outp_words =													\
		0xfc				/*	cld										*/	\
		0x1e				/*	push ds									*/	\
		0x8e 0xdf			/*	mov ds,di			; Change			*/	\
		0xf2 0x6f			/*	rep outsw			; Write them out	*/	\
		0x1f				/*	pop ds									*/	\
		parm nomemory [ SI DI ] [ CX ] [ DX ]								\
		modify exact nomemory [ CX SI ];


/* most thanks to aboyd... */
 /* dx = selector	ax = offset 	cx = length in bytes */
int verr(void __far *ptr, int len);
#pragma aux verr =					\
		0x0f 0x00 0xe2		/* verr dx		  */ \
		0x75 0x13			/* jnz verr_bad   */ \
		0xe3 0x0c			/* jcxz verr_ok   */ \
 		0x49				/* dec cx		  */ \
		0x03 0xc8			/* add cx,ax	  */ \
		0x72 0x0c			/* jc verr_bad    */ \
 		0x0f 0x03 0xC2		/* lsl ax,dx	  */ \
 		0x3b 0xc1			/* cmp ax,cx	  */ \
 		0x72 0x05			/* jb verr_bad    */ \
							/* verr_ok:       */ \
 		0xb8 0x01 0x00		/* mov ax,#1      */ \
		0xeb 0x02			/* jmp verr_done  */ \
							/* verr_bad:      */ \
 		0x33 0xc0			/* xor ax,ax      */ \
							/* verr_done:     */ \
		parm	nomemory [dx ax] [cx] \
		modify exact nomemory [ax cx];


 /* dx = selector	ax = offset 	cx = length in bytes */
int verw(void __far *ptr, int len);
#pragma aux verw =					\
		0x0f 0x00 0xea		/* verw dx		  */ \
		0x75 0x13			/* jnz verr_bad   */ \
		0xe3 0x0c			/* jcxz verr_ok   */ \
 		0x49				/* dec cx		  */ \
		0x03 0xc8			/* add cx,ax	  */ \
		0x72 0x0c			/* jc verr_bad    */ \
 		0x0f 0x03 0xC2		/* lsl ax,dx	  */ \
 		0x3b 0xc1			/* cmp ax,cx	  */ \
 		0x72 0x05			/* jb verr_bad    */ \
							/* verr_ok:       */ \
 		0xb8 0x01 0x00		/* mov ax,#1      */ \
		0xeb 0x02			/* jmp verr_done  */ \
							/* verr_bad:      */ \
 		0x33 0xc0			/* xor ax,ax      */ \
							/* verr_done:     */ \
		parm	nomemory [dx ax] [cx] \
		modify exact nomemory [ax cx];
