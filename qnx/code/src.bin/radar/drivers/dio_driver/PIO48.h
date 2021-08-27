

/*	This header file defines the functions of the PIO48C i/o cards */

/* define the control bits for the i8522 chips					      */

#define i8522_MODE_A_0 0x00
#define i8522_MODE_B_0 0x00
#define i8522_A_IN 0x10
#define i8522_A_OUT 0x00
#define i8522_B_IN 0x02
#define i8522_B_OUT 0x00
#define i8522_C_IN 0x09
#define i8522_C_OUT 0x00
#define i8522_C_UPPER_IN 0x08
#define i8522_C_UPPER_OUT 0x00
#define i8522_C_LOWER_IN 0x01
#define i8522_C_LOWER_OUT 0x00
#define i8522_MODE_SET 0x80

/* ----------------------------------------------------------------- */
/* define the control bits for the i8254 clock chip                  */

#define i8254_MODE_0 0x00
#define i8254_MODE_1 0x02
#define i8254_MODE_2 0x04
#define i8254_MODE_3 0x06
#define i8254_MODE_4 0x08
#define i8254_MODE_5 0x0a
#if DEBUG_MODE
#define i8254_SC0 0x80		/* use clock 2 when in DEBUG mode */
#else
#define i8254_SC0	0x00    /* select clock 0 */
#endif
#define i8254_SC1	0x40	/* select clock 1 */
#define i8254_SC2	0x80	/* select clock 2 */
#define i8254_RLL	0x10	/* set least significant byte */
#define i8254_RLM	0x20	/* set most significant byte */
#define i8254_RLLM	0x30	/* both bytes, LSB first */

/*	we assume that the external clock rate is 5 MHz and the internal
	rate we want for the clock is 100 kHz (10 microseconds).  The clock
	counter is therefore 50 */
#define COUNT_10MICRO	50
#if DEBUG_MODE
#define TIME20 200
#else
#define TIME20 20
#endif
#define Flocal_remote_bit 0x08	/* remote/local control mode bit */
								/* note that remote control is this bit LOW */

