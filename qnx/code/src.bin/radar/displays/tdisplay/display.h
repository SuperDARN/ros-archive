/* display.h */

#define TRUE 1
#define FALSE 0

void draw_axis(struct rawdata *,int range);
void draw_frame(int max_range);
void plot_velocity(struct fitdata *fit_data);
void plot_power(struct fitdata *fit_data);
void plot_width(struct fitdata *fit_data);
void plot_param(struct radar_id *rid,
		        struct radar_parms *parms,char *text,char *data,
				char *program);
void draw_labels(char *str);
void plot_acf(struct rawdata *raw_data,int);
