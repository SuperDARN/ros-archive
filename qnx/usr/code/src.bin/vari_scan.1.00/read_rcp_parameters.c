#include <stdio.h>
#include <stdlib.h>

void read_rcp_parameters( char *par_fname,
			  char *program_name,
			  char *program_id,
			  int *cp_id,
			  int *scan_period,
			  int *nloop,
			  int *bmnum_arr,
			  int *frang_arr,
			  int *rsep_arr,
			  int *nrang_arr,
			  int *intt_arr,
			  int *xcf_arr,
			  int *mpinc_arr,
			  int *day_freq_arr,
			  int *night_freq_arr
			)

{
char line_buffer[500];
int  iloop, idum, ib;
FILE *par_file;

  par_file= fopen( par_fname, "r");
  if (par_file == NULL)
    {
      printf( "%s not found!\n", par_fname);
      exit( 1);
    }
  do { ib= 0; do { line_buffer[ib]= fgetc( par_file); ib++; } while ( line_buffer[ib-1] != '\012'); } while ( line_buffer[0] == '#');
  sscanf( line_buffer, "%s", program_name);
  do { ib= 0; do { line_buffer[ib]= fgetc( par_file); ib++; } while ( line_buffer[ib-1] != '\012'); } while ( line_buffer[0] == '#');
  sscanf( line_buffer, "%s", program_id);
  do { ib= 0; do { line_buffer[ib]= fgetc( par_file); ib++; } while ( line_buffer[ib-1] != '\012'); } while ( line_buffer[0] == '#');
  sscanf( line_buffer, "%d", cp_id);
  do { ib= 0; do { line_buffer[ib]= fgetc( par_file); ib++; } while ( line_buffer[ib-1] != '\012'); } while ( line_buffer[0] == '#');
  sscanf( line_buffer, "%d", scan_period);
  do { ib= 0; do { line_buffer[ib]= fgetc( par_file); ib++; } while ( line_buffer[ib-1] != '\012'); } while ( line_buffer[0] == '#');
  sscanf( line_buffer, "%d", nloop);
  for ( iloop=0; iloop < *nloop; iloop++ )
    {
      do { ib= 0; do { line_buffer[ib]= fgetc( par_file); ib++; } while ( line_buffer[ib-1] != '\012'); } while ( line_buffer[0] == '#');
      sscanf( line_buffer, "%d %d %d %d %d %d %d %d %d %d", &idum, &bmnum_arr[ iloop], &frang_arr[ iloop], &rsep_arr[ iloop], &nrang_arr[ iloop],
        &intt_arr[ iloop], &xcf_arr[ iloop], &mpinc_arr[ iloop], &day_freq_arr[ iloop], &night_freq_arr[ iloop] );
    }
  fclose( par_file);
  
} /* read_rcp_parameter */


