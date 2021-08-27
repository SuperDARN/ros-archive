/* freq_lib.h
   ========== */


#define DEFAULT_FREQ 13000

struct freq_table {
  int num;
  int dfrq;
  int *start;
  int *end;
};

struct freq_table *load_ftable(FILE *fp);
	   	

