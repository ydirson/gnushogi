#ifdef DEBUG10
	  else
	    {
	      FILE *D;
	      int r, c, l;
	      struct leaf *xnode;
              short side;

	      D = fopen ("/tmp/DEBUG", "w");
	      pnt = TrPnt[2];
	      fprintf (D, "hashfile failure\n");
	      algbr (PV >> 8, PV & 0xff, 0);
	      fprintf (D, "inout move is %s\n", mvstr);
	      fprintf (D, "legal move are \n");
	      for (r = TrPnt[ply]; r < TrPnt[ply + 1]; r++)
		{
		  xnode = &Tree[r];
		  algbr (xnode->f, xnode->t, (short) xnode->flags);
		  fprintf (D, "%s %s %s %s\n", mvstr[0], mvstr[1], mvstr[2], mvstr[3]);
		}
	      fprintf (D, "\n current board is\n");
	      for (r = 8; r >= 0; r--)
		{
		  for (c = 0; c <= 8; c++)
		    {
		      l = locn (r, c);
                      if ( is_promoted[board[l]] )
                        fprintf (D, "+");
                      else 
                        fprintf (D, " ");
		      if (color[l] == neutral)
			fprintf (D, "-");
		      else if (color[l] == black)
			fprintf (D, "%c", qxx[board[l]]);
		      else
			fprintf (D, "%c", pxx[board[l]]);
		    }
		  fprintf (D, "\n");
		}
	      fprintf (D, "\n");
	      for (side = black; side <= white; side++)
	  	{ short piece, c; 
	    	  fprintf(D, (side==black)?"black ":"white ");
            	  for (piece = pawn; piece <= king; piece++)
	      	    if (c = Captured[side][piece]) 
		      fprintf (D, "%i%c ",c,pxx[piece]);
                  fprintf (D, "\n");
                };
	      fclose (D);
	    }
#endif
