/* colours.h
   =========
   Author: R.J.Barnes
*/

/* This header defines the colour bars that are used for displaying
 * data.
 *
 * nb. The colours range from zero to ###_COL_MAX INCLUSIVE for both
 * scales. So the actual number of colours is ###_COL_MAX+1
 */


#define PWR_COL_MAX 39
#define VEL_COL_MAX 39

  struct col_map {
    char chr;
    unsigned attr; 
  } pwr_key[]={
    
    {'\xb2',TERM_BLUE},
	{'\xdb',TERM_BLUE},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xdb',TERM_BLUE | TERM_HILIGHT},

	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN},

	{'\xb0',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb2',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN | TERM_HILIGHT},

	{'\xb2',TERM_CYAN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_CYAN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb0',TERM_CYAN | TERM_HILIGHT | TERM_GREEN_BG},
    {'\xb0',TERM_CYAN | TERM_GREEN_BG},
	{'\xdb',TERM_GREEN},

	{'\xb0',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_GREEN | TERM_HILIGHT},

	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_YELLOW | TERM_HILIGHT},

	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_YELLOW},

	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_RED | TERM_HILIGHT},

	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xdb',TERM_RED},
    {'\xb2',TERM_RED},
    {0,0}
};

struct col_map vel_key[]={

	{'\xb0',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_GREEN | TERM_HILIGHT},

	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_YELLOW | TERM_HILIGHT},

	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_YELLOW},

	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_RED | TERM_HILIGHT},

	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_RED_BG},

	{'\xdb',TERM_RED}, 
    {'\xb2',TERM_BLUE}, 

	{'\xdb',TERM_BLUE},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xdb',TERM_BLUE | TERM_HILIGHT},

	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN},

	{'\xb0',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb2',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN | TERM_HILIGHT},
    
    {'\xb0',TERM_WHITE | TERM_HILIGHT | TERM_CYAN_BG},
    {'\xb1',TERM_WHITE | TERM_HILIGHT | TERM_CYAN_BG},

    {'\xb0',TERM_WHITE | TERM_HILIGHT | TERM_WHITE_BG},
    {'\xb1',TERM_WHITE | TERM_HILIGHT | TERM_WHITE_BG},
    {'\xb2',TERM_WHITE | TERM_HILIGHT | TERM_WHITE_BG},
    {'\xdb',TERM_WHITE | TERM_HILIGHT},
        
    {0,0}
};

char gsct_key='\xb0';
