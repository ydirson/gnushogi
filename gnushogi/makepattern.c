/*
 * FILE: makepattern.c
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

#define MAX_PATTERN_DATA     5000
#define MAX_OPENING_SEQUENCE 20
#define MAX_PATTERN          200

short xboard = false;

small_short pattern_data[MAX_PATTERN_DATA];

/* minimal ShowMessage to avoid dependency on extraneous display code */
static void
Dummy_ShowMessage(char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}
static struct display dummydsp = {
  .ShowMessage = Dummy_ShowMessage,
};
struct display *dsp = &dummydsp;

#define is_digit(c) (((c) >= '0') && ((c) <= '9'))
#define is_alpha(c) ((((c) >= 'a') && ((c) <= 'z')) \
    || (((c) >= 'A') && ((c) <= 'Z')))
#define eos(s)      ((*s == '\0') || (*s == '\n'))


/* skip blanks and comments in brackets */

static void
skipbb(char **s)
{
    while ((**s == ' ') || (**s == '|') || (**s == '['))
    {
        if (**s == '[')
        {
            while (**s != ']')
                (*s)++;
        }

        (*s)++;
    }
}


/* skip unsigned numbers */

static void
skipi(char **s)
{
    while (is_digit(**s))
        (*s)++;

    skipbb(s);
}


static short
ScanPiece(char **s, small_short *side,
          small_short *piece, small_short *square)
{
    short isp, isw, c, r;

    /* determine promotion status */
    if (**s == '+')
        isp = true, (*s)++;  /* FIXME: split into two lines. */
    else
        isp = false;

    /* determine side and piece */
    for (c = 0; c < NO_PIECES; c++)
    {
        if ((isw = (**s == pxx[c])) || (**s == qxx[c]))
        {
            *piece = isp ? promoted[c] : unpromoted[c];
            *side  = isw;
            (*s)++;
            break;
        }
    }

    if (c == NO_PIECES)
        return 1;

    if (**s == '*')
    {
        /* piece is captured */
        (*s)++;
        *square = NO_SQUARES + *piece;
    }
    else
    {
        /* determine column */
        for (c = 0; c < NO_COLS; c++)
        {
            if (**s == COL_NAME(c))
            {
                (*s)++;
                break;
            }
        }

        if (c >= NO_COLS)
            return 1;

        /* determine row */
        for (r = 0; r < NO_ROWS; r++)
        {
            if (**s == ROW_NAME(r))
            {
                (*s)++;
                break;
            }
        }

        if (r >= NO_ROWS)
            return 1;

        /* determine square */
        *square = r * NO_COLS + c;
    }

    skipbb(s);
    return 0;
}


static short
ScanPattern (char *s, short *pindex)
{
    small_short side, piece, square;
    skipbb(&s); /* skip blanks and comments */

    while (is_digit(*s))
    {
        pattern_data[(*pindex)++] = atoi(s);
        skipi(&s);
    }

    pattern_data[(*pindex)++] = END_OF_LINKS;
    skipbb(&s);

    while (!eos(s))
    {
        if (ScanPiece(&s, &side, &piece, &square))
        {
            return 1;
        }
        else
        {
            pattern_data[(*pindex)++] = piece;
            pattern_data[(*pindex)++] = (side ? -square : square);
        }

    }

    pattern_data[(*pindex)++] = END_OF_FIELDS;
    return 0;
}


void
ReadOpeningSequences (short *pindex, const char* patternfile)
{
    FILE *fd;
    char s[256];
    short max_pattern = 0;
    short max_opening_sequence = 0;

    fd = fopen (patternfile, "r");

    if (fd == NULL) {
        sprintf(s, "no pattern file '%s'", patternfile);
        dsp->ShowMessage(s);
        return;
    }

    *pindex = 0;

    while (fgets (s, 256, fd) != NULL)
    {
        if (*s == '#')
        {
            /* comment, skip line */
        }
        else if (is_alpha(*s))
        {
            if (max_opening_sequence++ > 0)
            {
                pattern_data[(*pindex)++] = END_OF_PATTERNS;
            }

            pattern_data[(*pindex)++] = ValueOfOpeningName(s);
        }
        else
        {
            if (ScanPattern(s, pindex))
            {
                dsp->ShowMessage("error in pattern sequence...");
                exit(1);
            }
            else
            {
                max_pattern++;
            }
        }
    }

    pattern_data[(*pindex)++] = END_OF_PATTERNS;
    pattern_data[(*pindex)++] = END_OF_SEQUENCES;

    sprintf(s,
            "Pattern: %d bytes for %d sequences with %d patterns.\n",
            *pindex, max_opening_sequence, max_pattern);
    dsp->ShowMessage(s);

    fclose(fd);
}


void
WriteOpeningSequences (short pindex, const char* patternincfile)
{
    FILE *fd;
    short n = 0;
    short max_pattern = 0;
    short max_opening_sequence = 0;

    fd = fopen (patternincfile, "w");
    fprintf(fd, "#define MAX_PATTERN_DATA %d\n\n", pindex);
    fprintf(fd, "small_short pattern_data[MAX_PATTERN_DATA] =\n{\n");

    do
    {
        fprintf(fd, "  %d,\n", pattern_data[n++]);

        do
        {
            fprintf(fd, "    ");

            /* write links */
            while (pattern_data[n] != END_OF_LINKS)
            {
                fprintf(fd, "%d, ", pattern_data[n++]);
            };

            fprintf(fd, "%d, ", pattern_data[n++]);

            /* write pattern */
            do
            {
                fprintf(fd, "%d,", pattern_data[n++]);
            }
            while (pattern_data[n] != END_OF_FIELDS);

            fprintf(fd, "%d,\n", pattern_data[n++]);
            max_pattern++;
        }
        while (pattern_data[n] != END_OF_PATTERNS);

        fprintf(fd, "    %d,\n", pattern_data[n++]);
        max_opening_sequence++;
    }
    while (pattern_data[n] != END_OF_SEQUENCES);

    fprintf(fd, "  %d\n}; \n", pattern_data[n++]);
    fprintf(fd, "\n#define MAX_OPENING_SEQUENCE %d\n", max_opening_sequence);
    fprintf(fd, "\n#define MAX_PATTERN %d\n", max_pattern);
    fclose(fd);
}


