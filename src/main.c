/*
 * main.c - C source for GNU SHOGI based on GNU CHESS
 *
 * Copyright (c) 1988,1989,1990 John Stanback (GNU Chess)
 * Copyright (c) 1992 Free Software Foundation 
 * Copyright (c) 1993,1994,1995 Matthias Mutz (GNU Shogi)
 *
 * This file is part of GNU SHOGI.
 *
 * GNU Shogi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Shogi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "version.h"
#include "gnushogi.h"

#include <signal.h>


#if defined THINK_C
#include <console.h>
#include <time.h>
#endif




#ifdef DEBUG_INITS

/* print all possible moves for all pieces from all squares */

void DebugInits ()
{                    
   register short u, sq;
   register unsigned char *ppos, *pdir;
   char s[10];
   short piece, ptyp;

   for ( piece = 0; piece < NO_PIECES; piece++ ) {
     printf("move list for piece %i\n",piece);
     for ( sq = 0; sq < NO_SQUARES; sq++ ) {
       printf("  from square %i to ",sq);
       ptyp = ptype[black][piece];
       ppos = (*nextpos)[ptyp][sq];
       u = ppos[sq]; 
       do {
          printf("%i",u);
          u = ppos[u];
          if (u != sq) printf(", ");
       } while (u != sq); 
       printf("\n");
     };
     /* pdir = (*nextdir)[ptyp][sq]; */
     printf("\n");
     scanf("%s",s);
     if ( strcmp(s,"exit") == 0 )
       exit(0);
   };
}

#endif  



int
main (int argc, char **argv)
{

#ifdef THINK_C
  console_options.ncols = 100;
  cshow(stdout);
#ifdef NONDSP
  ccommand(&argv);
#endif
#endif    

  if (argc > 2)
    {
      if (argv[1][0] == '-' && argv[1][1] == 'L')
	{
	  Lang = argv[2];
	  argv += 2;
	  argc -= 2;
	}
    }
    
  while (argc > 1 && ((argv[1][0] == '-') || (argv[1][0] == '+')))
    {
      switch (argv[1][1])
	{
	case 'a':
	  ahead = ((argv[1][0] == '-') ? false : true);
	  break;
	case 'b':
	  argv++;
	  argc--;
	  if (argc > 1)
	    {
	      bookfile = argv[1];
#ifdef BINBOOK
	      binbookfile = NULL;
#endif
	    }
	  break;
#ifdef BINBOOK
	case 'B':
	  argv++;
	  argc--;
	  if (argc > 1)
	    binbookfile = argv[1];
	  break;
#endif
	case 'h':
	  hash = ((argv[1][0] == '-') ? false : true);
	  break;
	case 's':
	  argc--;
	  argv++;
	  if (argc > 1)
	    strcpy (savefile, argv[1]);
	  break; 
	case 'l':
	  argc--;
	  argv++;
	  if (argc > 1)
	    strcpy (listfile, argv[1]);
	  break;
	case 'S':
	  argc--;
	  argv++;
	  if(argc > 1)booksize = atoi(argv[1]);
	  break;
	case 'P':
	  argc--;
	  argv++;
	  if(argc > 1)bookmaxply = atoi(argv[1]);
	  break;

#if ttblsz
	case 'r':
	  if (argc > 2)
	    rehash = atoi (argv[2]);
	  argc--;
	  argv++;
	  if (rehash > MAXrehash)
	    rehash = MAXrehash;
	  break;
	case 'T':
	  if (argc > 2)
	    ttblsize = atoi (argv[2]);
	  argc--;
	  argv++;
	  if ((ttblsize <= MINTTABLE)) ttblsize = (MINTTABLE)+1;
	  break;
#ifdef HASHFILE
	case 't':	/* create or test persistent transposition
				 * table */
	  hashfile = fopen (HASHFILE, RWA_ACC);
	  if (hashfile)
	    {
	      fseek (hashfile, 0L, SEEK_END);
	      filesz = (ftell (hashfile) / sizeof (struct fileentry)) - 1;
	    }
	  if (hashfile != NULL)
	    {
	      long i, j;
	      int nr[MAXDEPTH];
	      struct fileentry n;

	      printf (CP[49]);
	      for (i = 0; i < MAXDEPTH; i++)
		nr[i] = 0;
	      fseek (hashfile, 0L, SEEK_END);
	      i = ftell (hashfile) / sizeof (struct fileentry);
	      fseek (hashfile, 0L, SEEK_SET);
	      for (j = 0; j < i + 1; j++)
		{
		  fread (&n, sizeof (struct fileentry), 1, hashfile);
if(n.depth >MAXDEPTH) {printf("ERROR\n");exit(1);}
		  if (n.depth)
		    {
		      nr[n.depth]++;
		      nr[0]++;
		    }
		}
	      printf (CP[109],
		      nr[0], i);
	      for (j = 1; j < MAXDEPTH; j++)
		printf ("%d ", nr[j]);
	      printf ("\n");
	    }
	  return 0;
	case 'c':		/* create or test persistent transposition
				 * table */
	  if (argc > 2)
	    filesz = atoi (argv[2]);
	  else
	    filesz = vfilesz;
	  if (filesz > 0 && filesz < 24)
	    filesz = (1 << filesz) - 1 + MAXrehash;
	  else
	    filesz = filesz + MAXrehash;
#ifdef HASHFILE
	  if ((hashfile = fopen (HASHFILE, RWA_ACC)) == NULL)
	    hashfile = fopen (HASHFILE, WA_ACC);
	  if (hashfile != NULL)
	    {
	      long j;
	      struct fileentry n;

	      printf (CP[66]);
	      n.f = n.t = 0;
	      n.flags = 0;
	      n.depth = 0;
	      n.sh = n.sl = 0;
	      for (j = 0; j < filesz + 1; j++)
		fwrite (&n, sizeof (struct fileentry), 1, hashfile);
	      fclose (hashfile);
	    }
	  else
	    printf (CP[50], HASHFILE);
#endif
	  return (0);
#endif /* HASHFILE */
#endif /* ttblsz */
	case 'x':
	  xwin = &argv[1][2];
	  break;
	case 'v':
	  fprintf (stderr, CP[102], version, patchlevel);
	  exit (1);
	default:
	  fprintf (stderr, CP[113]);
	  exit (1);
	}
      argv++;
      argc--;
    }
    
  if (argc == 2)
    {
      char *p;
      
      MaxResponseTime = 100L * strtol (argv[1], &p, 10);
      if (*p == ':')
	MaxResponseTime = 60L * MaxResponseTime +
	  100L * strtol (++p, (char **) NULL, 10);
      TCflag = false;
      TCmoves = 0;
      TCminutes = 0;
      TCseconds = 0;
    }
    
  if (argc >= 3)
    {
      char *p;
      if (argc > 9)
	{
	  printf ("%s\n", CP[220]);
	  exit (1);
	}
      TCmoves = atoi (argv[1]);
      TCminutes = (short)strtol (argv[2], &p, 10);
      if (*p == ':')
	TCseconds = (short)strtol (p + 1, (char **) NULL, 10);
      else
	TCseconds = 0;
      TCflag = true;
      argc -= 3;
      argv += 3;
      while (argc > 1)
	{
	  XCmoves[XC] = atoi (argv[0]);
	  XCminutes[XC] = (short)strtol (argv[1], &p, 10);
	  if (*p == ':')
	    XCseconds[XC] = (short)strtol (p + 1, (char **) NULL, 10);
	  else
	    XCseconds[XC] = 0;
	  if (XCmoves[XC] && (XCminutes[XC] || XCseconds[XC]))
	    XC++;
	  else
	    {
	      printf (CP[220]);
	      exit (1);
	    }
	  argc -= 2;
	  argv += 2;
	}
      if (argc)
	{
	  printf ("%s\n", CP[220]);
	  exit (1);
	}
    }
    
  if ( InitMain() != 0 )
    exit(1);
  
#ifdef DEBUG_INITS
  DebugInits ();
#endif

  while (!(flag.quit))
    {
      oppptr = (oppptr + 1) % MINGAMEIN;
      if (flag.bothsides && !flag.mate) {
	SelectMove (opponent, FOREGROUND_MODE);
      } else
	InputCommand (NULL);
      if (opponent == white)
	if (flag.gamein || TCadd)
	  {
	    TimeCalc ();
	  }
	else if (TimeControl.moves[opponent] == 0)
	  {
	    if (XC)
	      if (XCmore < XC)
		{
		  TCmoves = XCmoves[XCmore];
		  TCminutes = XCminutes[XCmore];
		  TCseconds = XCseconds[XCmore];
		  XCmore++;
		}
	    SetTimeControl ();
	  }

      compptr = (compptr + 1) % MINGAMEIN;
      if (!(flag.quit || flag.mate || flag.force))
	{ 
#ifdef INTTERRUPT_TEST
	  printf("starting search...\n");
#endif
	  SelectMove (computer, FOREGROUND_MODE);
	  if (computer == white)
	    if (flag.gamein)
	      {
		TimeCalc ();
	      }
	    else if (TimeControl.moves[computer] == 0)
	      {
		if (XC)
		  if (XCmore < XC)
		    {
		      TCmoves = XCmoves[XCmore];
		      TCminutes = XCminutes[XCmore];
		      TCseconds = XCseconds[XCmore];
		      XCmore++;
		    }
		SetTimeControl ();
	      }
	}
    }
    
  ExitMain ();
  
  return (0);
}


