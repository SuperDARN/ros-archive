/* badlags.h
   =========
   Author: R.J.Barnes
*/

void badlags(struct fit_prm *ptr,struct badsmp *bptr);
void ckrng(int range,int *badlag,struct badsmp *bptr,
	       struct fit_prm *ptr);
