/*
 * FILE: init-common.c
 *
 * ----------------------------------------------------------------------
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz
 * Copyright (c) 1999 Michael Vanier and the Free Software Foundation
 * Copyright (c) 2008, 2013, 2014 Yann Dirson and the Free Software Foundation
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988, 1989, 1990 John Stanback
 * Copyright (c) 1992 Free Software Foundation
 *
 * This file is part of GNU SHOGI.
 *
 * GNU Shogi is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with GNU Shogi; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 */
#include "gnushogi.h"
#include "pattern.h"

unsigned int ttbllimit;

/*
 * ptype is used to separate black and white pawns, like this; ptyp =
 * ptype[side][piece] piece can be used directly in nextpos/nextdir when
 * generating moves for pieces that are not white pawns.
 */

const small_short ptype[2][NO_PIECES] =
{
    {
        ptype_no_piece, ptype_pawn,
#ifndef MINISHOGI
	ptype_lance,  ptype_knight,
#endif
        ptype_silver,   ptype_gold,  ptype_bishop, ptype_rook,
        ptype_gold,
#ifndef MINISHOGI
	ptype_gold,  ptype_gold,
#endif
	ptype_gold,
        ptype_pbishop,  ptype_prook, ptype_king
    },
    {
        ptype_no_piece, ptype_wpawn,
#ifndef MINISHOGI
	ptype_wlance, ptype_wknight,
#endif
        ptype_wsilver,  ptype_wgold, ptype_bishop, ptype_rook,
        ptype_wgold,
#ifndef MINISHOGI
	ptype_wgold, ptype_wgold,
#endif
	ptype_wgold,
        ptype_pbishop,  ptype_prook, ptype_king
    },
};

const small_short promoted[NO_PIECES] =
{
    no_piece, ppawn,
#ifndef MINISHOGI
    plance, pknight,
#endif
    psilver, gold, pbishop, prook,
    ppawn,
#ifndef MINISHOGI
    plance, pknight,
#endif
    psilver, pbishop, prook, king
};

const small_short unpromoted[NO_PIECES] =
{
    no_piece, pawn,
#ifndef MINISHOGI
    lance, knight,
#endif
    silver, gold, bishop, rook,
    pawn,
#ifndef MINISHOGI
    lance, knight,
#endif
    silver, bishop, rook, king
};


/* .... MOVE GENERATION VARIABLES AND INITIALIZATIONS .... */

#ifndef WIN32
#define max(a, b) (((a) < (b))?(b):(a))
#endif
#define odd(a) ((a) & 1)

const small_short piece_of_ptype[NO_PTYPE_PIECES] =
{
    pawn,
#ifndef MINISHOGI
    lance, knight,
#endif
    silver, gold, bishop, rook, pbishop, prook, king,
    pawn,
#ifndef MINISHOGI
    lance, knight,
#endif
    silver, gold
};


/* FIXME: all bishops and rooks are black ? */
const small_short side_of_ptype[NO_PTYPE_PIECES] =
{
    black,
#ifndef MINISHOGI
    black, black,
#endif
    black, black, black, black, black, black, black,
    white,
#ifndef MINISHOGI
    white, white,
#endif
    white, white
};


int
Initialize_data(void)
{
    size_t n;
    int i;
    char buffer[60];
    int doit = true;

    {
        small_short x = -1;

        if (x >= 0)
        {
            dsp->ShowMessage("datatype 'small_short' is unsigned; "
                        "check gnushogi.h\n");
            return 1;
        }
    }

    n = sizeof(struct leaf) * (size_t)TREE;
    Tree = malloc(n);

    if (!Tree)
    {
        sprintf(buffer, "Cannot allocate %ld bytes for search tree",
                (long)n);
        dsp->ShowMessage(buffer);
        return 1;
    }

    n = sizeof(hashcode_array);
    hashcode = malloc(n);

    if (!hashcode)
    {
        sprintf(buffer, "Cannot allocate %ld bytes for hashcode", (long)n);
        dsp->ShowMessage(buffer);
        return 1;
    }

    n = sizeof(drop_hashcode_array);
    drop_hashcode = malloc(n);

    if (!drop_hashcode)
    {
        sprintf(buffer,
                "Cannot allocate %ld bytes for drop_hashcode",
                (long)n);
        dsp->ShowMessage(buffer);
        return 1;
    }

    n = sizeof(struct GameRec) * (size_t)(MAXMOVES + MAXDEPTH);
    GameList = malloc(n);

    if (!GameList)
    {
        sprintf(buffer,
                "Cannot allocate %ld bytes for game record",
                (long)n);
        dsp->ShowMessage(buffer);
        return 1;
    }

#if !defined SAVE_NEXTPOS
    n = sizeof(next_array);

    for (i = 0; i < NO_PTYPE_PIECES; i++)
    {
        nextdir[i] = use_nextpos ? malloc(n) : NULL;

        if (!nextdir[i])
        {
            if (use_nextpos)
            {
                sprintf(buffer, "cannot allocate %ld space for nextdir %d",
                        (long)(n), i);
                dsp->ShowMessage(buffer);
            }

            nextdir[i] = NULL;
            use_nextpos = false;
        }

        nextpos[i] = use_nextpos ? malloc(n) : NULL;

        if (!nextpos[i])
        {
            if (use_nextpos)
            {
                sprintf(buffer, "cannot allocate %ld space for nextpos %d",
                        (long)(n), i);
                dsp->ShowMessage(buffer);
            }

            use_nextpos = false;
        }
    }

    if (!use_nextpos)
    {
        return 1;
    }
#endif

    n = sizeof(value_array);
    value = malloc(n);

    if (!value)
    {
        dsp->ShowMessage("cannot allocate value space");
        return 1;
    }

    n = sizeof(fscore_array);
    fscore = malloc(n);

    if (!fscore)
    {
        dsp->ShowMessage("cannot allocate fscore space");
        return 1;
    }

#if defined HISTORY
    n = sizeof_history;
    history = malloc(n);

    if (!history)
    {
        sprintf(buffer, "Cannot allocate %ld bytes for history table",
                (long)sizeof_history);
        dsp->ShowMessage(buffer);
        use_history = false;
    }
#endif

#if defined CACHE
    n = sizeof(struct etable) * (size_t)ETABLE;

    for (i = 0; i < 2; i++)
    {
        etab[i] = use_etable ? malloc(n) : 0;

        if (!etab[i])
        {
            sprintf(buffer, "Cannot allocate %ld bytes for cache table %ld",
                    (long)n, (long)i);
            dsp->ShowMessage(buffer);
            use_etable = false;
        }
    }
#endif

#if ttblsz

    if (rehash < 0)
        rehash = MAXrehash;

    n = sizeof(struct hashentry)*(ttblsize + rehash);

    while (doit && ttblsize > MINTTABLE)
    {
        ttable[0] = malloc(n);  /* FIXME: cast to the correct type. */
        ttable[1] = ttable[0] ? malloc(n) : NULL;

        if (!ttable[0] || !ttable[1])
        {
            if (!ttable[0])
                free(ttable[0]);

            if (!ttable[1])
                free(ttable[1]);

            ttblsize = ttblsize >> 1;
            n = sizeof(struct hashentry) * (ttblsize + rehash);
        }
        else
        {
            doit = false;
        }
    }

    if (ttblsize <= MINTTABLE)
    {
        use_ttable = false;
    }

    if (use_ttable)
    {
        /* CHECKME: is the precedence here correct? */
        /* ttbllimit = ttblsize << 1 - ttblsize >> 2; */
        ttbllimit = (ttblsize << 1) - (ttblsize >> 2);
    }
    else
    {
        sprintf(buffer, "Cannot allocate %ld bytes for transposition table",
                (long)(2 * n));
        dsp->ShowMessage(buffer);
        ttable[0] = ttable[1] = NULL;
    }
#endif /* ttblsz */

#if !defined SAVE_DISTDATA
    n = sizeof(distdata_array);
    distdata = malloc(n);

    if (!distdata)
    {
        dsp->ShowMessage("cannot allocate distdata space...");
        use_distdata = false;
    }
#endif

#if !defined SAVE_PTYPE_DISTDATA
    n = sizeof(distdata_array);

    for (i = 0; i < NO_PTYPE_PIECES; i++)
    {
        ptype_distdata[i] = use_ptype_distdata ? malloc(n) : 0;

        if (!ptype_distdata[i])
        {
            sprintf(buffer,
                    "cannot allocate %ld bytes for ptype_distdata %d...",
                    (long)n, i);
            use_ptype_distdata = false;
        }
    }
#endif

    return 0;
}


#ifdef SAVE_PTYPE_DISTDATA
short
piece_distance(short side, short piece, short f, short t)
{
    return ((f > NO_SQUARES)
            ? (short)1
            : (short)ptype_distance(ptype[side][piece], f, t));
}
#else
short
piece_distance(short side, short piece, short f, short t)
{
    return ((f > NO_SQUARES)
            ? (short)1
            : (use_ptype_distdata
               ? (short)(*ptype_distdata[ptype[side][piece]])[f][t]
               : (short)ptype_distance(ptype[side][piece], f, t)));
}
#endif


/*
 * Determine the minimum number of moves for a piece from
 * square "f" to square "t". If the piece cannot reach "t",
 * the count is set to CANNOT_REACH.
 */

#define csquare(sq) ((side == black) ? sq : (NO_SQUARES - 1 - sq))
#define crow(sq) row(csquare(sq))
#define ccol(sq) column(csquare(sq))

short
ptype_distance(short ptyp, short f, short t)
{
    short side, piece;
    short colf, colt, rowf, rowt, dcol, drow;

    if (f == t)
        return 0;

    piece = piece_of_ptype[ptyp];
    side  = side_of_ptype[ptyp];

    dcol = (colt = ccol(t)) - (colf = ccol(f));
    drow = (rowt = crow(t)) - (rowf = crow(f));

    switch (piece)
    {
    case pawn:
        if ((dcol != 0) || (drow < 1))
            return CANNOT_REACH;
        else
            return drow;

#ifndef MINISHOGI
    case lance:
        if ((dcol != 0) || (drow < 1))
            return CANNOT_REACH;
        else
            return 1;

    case knight:
        if (odd(drow) || (odd(drow / 2) != odd(dcol)))
            return CANNOT_REACH;
        else if ((drow == 0) || ((drow / 2) < abs(dcol)))
            return CANNOT_REACH;
        else
            return (drow / 2);
#endif

    case silver:
        if (drow > 0)
        {
            if (odd(drow) == odd(dcol))
            {
                return max(abs(drow), abs(dcol));
            }
            else
            {
                if (abs(dcol) <= drow)
                    return drow;
                else
                    return (max(abs(drow), abs(dcol)) + 1);
            }
        }
        else
        {
            if (odd(drow) == odd(dcol))
                return (max(abs(drow), abs(dcol)));
            else
                return (max(abs(drow) + 1, abs(dcol)) + 1);
        };

    case gold:
    case ppawn:
#ifndef MINISHOGI
    case pknight:
    case plance:
#endif
    case psilver:
        if (abs(dcol) == 0)
            return (abs(drow));
        else if (drow >= 0)
            return max(drow, abs(dcol));
        else
            return (abs(dcol) - drow);

    case bishop:
        if (odd(dcol) != odd(drow))
            return CANNOT_REACH;
        else
            return ((abs(dcol) == abs(drow)) ? 1 : 2);

    case pbishop:
        if (odd(dcol) != odd(drow))
        {
            if ((abs(dcol) <= 1) && (abs(drow) <= 1))
                return 1;
            else if (abs(abs(dcol) - abs(drow)) == 1)
                return 2;
            else
                return 3;
        }
        else
        {
            return ((abs(dcol) == abs(drow)) ? 1 : 2);
        }

    case rook:
        if ((dcol == 0) || (drow == 0))
            return 1;
        else
            return 2;

    case prook:
        if ((dcol == 0) || (drow == 0))
            return 1;
        else if ((abs(dcol) == 1) && (abs(drow) == 1))
            return 1;
        else
            return 2;

    case king:
        return max(abs(drow), abs(dcol));

    default:
        /* should never occur */
        return (CANNOT_REACH);
    }
}
