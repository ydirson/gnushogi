/*
 * FILE: pat2inc.c
 *
 *     Convert GNU Shogi pattern textfile to an include file.
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


extern void ReadOpeningSequences(short *pindex, const char* patternfile);
extern void WriteOpeningSequences(short pindex, const char* patternincfile);

small_short board[NO_SQUARES];
small_short color[NO_SQUARES];

int
main(int argc, char **argv)
{
    short sq, side, max_pattern_data;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s gnushogi.pat pattern.inc\n", argv[0]);
        exit(1);
    }
    char* patternfile = argv[1];
    char* patternincfile = argv[2];

#ifdef TEST_DISTANCE
    short d;
    char  s[80];
#endif

    Initialize_data();

    for (sq = 0; sq < NO_SQUARES; sq++)
    {
        board[sq] = no_piece;
        color[sq] = neutral;
    }

    ClearCaptured();

    for (side = 0; side <= 1; side++)
        PieceCnt[side] = -1;

#ifdef TEST_DISTANCE
    strcpy(s, "g6i k5i g4i p9g p8g r* s3h p7g b8h B* S5f");

    if (string_to_board_color(s))
    {
        printf("ERROR IN string_to_board_color");
        exit(1);
    }
    else
    {
        UpdateDisplay(0, 0, 1, 0);
    }

    d = pattern_distance(black, &pattern);

    printf("distance = %d\n", d);

#endif

    ReadOpeningSequences(&max_pattern_data, patternfile);
    WriteOpeningSequences(max_pattern_data, patternincfile);

    return 0;
}
