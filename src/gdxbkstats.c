#include <stdio.h>
#include <fcntl.h>
#include "gnushogi.h"
#undef rxx
#undef cxx
#undef scanz
#undef printz

#define rxx "ihgfedcba"
#define cxx "987654321"

static char pchar[NO_PIECES] =
 { 'P','L','N','S','G','B','R','P','L','N','S','B','R','K' };

char xmvstr[12];


char *
cvt (m,flags)
     unsigned int m;
     short flags;
{
    unsigned int f, t;
    short piece = 0, flag = 0;
    char *s;
    f = m >> 8 & 0x7f;
    t = m & 0xff;
/* algebraic notation */
    if ( f > NO_SQUARES )
      { short piece = f - NO_SQUARES;
        if ( f > NO_PIECES ) f -= NO_PIECES;
        flag = (dropmask | piece );
      }
    if ( t & 0x80 )
      {
        flag |= promote;
        t &= 0x7f;
      }
    s = xmvstr;
    if ( flag & dropmask )
      { 
	*s = pchar[piece]; s++;
        *s = '*'; s++;
        *s = cxx[column (t)]; s++;
        *s = rxx[row (t)]; s++;
      }
    else
      {
        *s = cxx[column (f)]; s++;
        *s = rxx[row (f)]; s++;
        *s = cxx[column (t)]; s++;
        *s = rxx[row (t)]; s++;
        if ( flag & promote )
          {
            *s = '+'; s++;
          }
      }
    if (m & DONTUSE)
      {
	*s = '?'; s++;
      }
    if (flags & LASTMOVE)
      {
	*s = '.'; s++;
      }
    *s = '\0';
    return xmvstr;
}

/* #define lts(x) (x>>16) */		/* long to short to convert hashkey to short */

#ifdef LONG64
#define lts(x) (((x>>48)&0xfffe)|(x&0x1))
#else
#define lts(x) (((x>>16)&0xfffe)|(x&0x1))
#endif

int gfd;
struct gdxadmin
{
    unsigned int bookcount;
    unsigned int booksize;
    unsigned long maxoffset;
} ADMIN;

#define N 2
struct gdxdata
{
    unsigned int hashbd;
    unsigned short hashkey;
    unsigned short bmove;
    unsigned short flags;
    unsigned short hint;
    unsigned short count;
} DATA;
void 
usage (char *x)
{
    printf ("usage %s binbookfile [ -h key bd]\n", x);
    exit ();
}

int i;
int c1 = 0;
int in = 0;
int max = -9999;
int min = 9999999;
int n = 0;
int sum = 0;
int sumc = 0;
unsigned long key, bd;
int hk = false;
main (argc, argv)
     int argc;
     char **argv;
{
    if (argc == 5)
      {
	  if (strcmp (argv[2], "-h") != 0)
	      usage (argv[0]);
	  key = strtol (argv[3], NULL, 16);
	  bd = strtol (argv[4], NULL, 16);
	  hk = true;
      }
    else if (argc != 2)
	usage (argv[0]);
    gfd = open (argv[1], O_RDONLY);
    if (gfd >= 0)
      {
	  read (gfd, &ADMIN, sizeof (struct gdxadmin));
	  printf ("entrysize %d\nbooksize %d\nbookcount %d\nmaxoffset %ld\n", sizeof (struct gdxdata), ADMIN.booksize, ADMIN.bookcount, ADMIN.maxoffset);
	  for (i = 0; i < ADMIN.booksize; i++)
	    {
		if (0 > read (gfd, &DATA, sizeof (struct gdxdata)))
		  {
		      perror ("fread");
		      exit (1);
		  }
		if (hk)
		  {  
#if 1 
		      if (DATA.count)
			{
			    printf ("bd = %ld key = %ld: ",DATA.hashbd,DATA.hashkey);
#else
		      if (DATA.count && DATA.hashbd == bd && lts (key) == DATA.hashkey)
			{
#endif
			    printf ("%s ", cvt (DATA.bmove,DATA.flags));
			    printf ("%s ", cvt (DATA.hint,0));
			    printf ("%d\n", DATA.count);
			}
		  }
		if (in && DATA.bmove)
		  {
		      c1++;
		  }
		else if (DATA.bmove)
		  {
		      in = 1;
		      c1 = 1;
		  }
		else if (c1)
		  {
		      /*printf ("out %d\n", c1);*/
		      n++;
		      if (c1 < min)
			  min = c1;
		      if (c1 > max)
			  max = c1;
		      sum += c1;
		      c1 = 0;
		      in = 0;
		  }
	    }
	  close (gfd);
	  if (in)
	    {
		/*printf ("out %d\n", c1);*/
		n++;
		if (c1 < min)
		    min = c1;
		if (c1 > max)
		    max = c1;
		sum += c1;
	    }
	  printf ("max %d\nmin %d\navg %f\nsumc %d\n", max, min, (float) sum / (float) n, sumc);

      }
}
