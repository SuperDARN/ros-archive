/* this file contains the external declarations for dt 2828 A/D converter
*/

extern union dt_adcsr_str dt_adcsr;
extern union dt_chancsr_str dt_chancsr;
extern union dt_dacsr_str dt_dacsr;
extern union dt_supcsr_str dt_supcsr;
extern union dt_tmrctr_str dt_tmrctr;

/* added for QNX version 4 */
extern int dt_hint_id;								/* hardware interrupt id */
extern pid_t dt_proxy;								/* proxy */
