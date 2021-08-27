/* DIO.h
   ====== */

int DIO_adr(int dio_port);   
int DIO_state(int dio_port);
int DIO_value(int dio_port,unsigned char dio_value,unsigned char dio_mask);
void PUT_DIO(int dio_port,unsigned char dio_value,unsigned char dio_mask);
unsigned char GET_DIO(int dio_port);
