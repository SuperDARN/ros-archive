/* wait_boundary.c
   =============== */

/*
 $Log: wait_boundary.c,v $
 Revision 1.1  1999/03/31 19:59:42  barnes
 Initial revision

 */

int wait_boundary(float bnd) {
  int sleep_time;
  int yr,mo,dy,hr,mt,sc,ms,us;
  read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);

   sleep_time=(int) (bnd*1000)-
               ((((mt*60+sc)*1000)+ms) % 
                (int)(bnd*1000));
   sleep_time-=20;
   if (sleep_time<0) sleep_time=0;
   sleep((unsigned int) (sleep_time/1000));
   delay((unsigned int) (sleep_time % 1000)); 
   return 0;

}

 