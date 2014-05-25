/*
 * FILE: book.c
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

#define O_BINARY 0

#if HAVE_UNISTD_H
/* Declarations of read(), write(), close(), and lseek(). */
#include <unistd.h>
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "book.h"

unsigned booksize = BOOKSIZE;
unsigned short bookmaxply = BOOKMAXPLY;
unsigned bookcount = 0;

#ifdef BOOK
char *bookfile = BOOK;
#else
char *bookfile = NULL;
#endif

#ifdef BINBOOK
char *binbookfile = BINBOOK;
#else
char *binbookfile = NULL;
#endif

static char bmvstr[3][7];

static ULONG bhashbd;
static ULONG bhashkey;


/*
 * Balgbr(f, t, flags)
 *
 * Generate move strings in different formats.
 */

static void
Balgbr(short f, short t, short flags)
{
    short promoted = false;

    if ((f & 0x80) != 0)
    {
        f &= 0x7f;
        promoted = true;
    }

    if (f > NO_SQUARES)
    {
        short piece;
        piece = f - NO_SQUARES;

        if (f > (NO_SQUARES + NO_PIECES))
            piece -= NO_PIECES;

        flags = (dropmask | piece);
    }

    if ((t & 0x80) != 0)
    {
        flags |= promote;
        t &= 0x7f;
    }

    if ((f == t) && ((f != 0) || (t != 0)))
    {
        /*
         * error in algbr: FROM=TO=t
         */

        bmvstr[0][0] = bmvstr[1][0] = bmvstr[2][0] = '\0';
    }
    else
    {
        if ((flags & dropmask) != 0)
        {
            /* bmvstr[0]: P*3c bmvstr[1]: P'3c */
            short piece = flags & pmask;
            bmvstr[0][0] = pxx[piece];
            bmvstr[0][1] = '*';
            bmvstr[0][2] = COL_NAME(column(t));
            bmvstr[0][3] = ROW_NAME(row(t));
            bmvstr[0][4] = bmvstr[2][0] = '\0';
            strcpy(bmvstr[1], bmvstr[0]);
            bmvstr[1][1] = '\'';
        }
        else
        {
            if ((f != 0) || (t != 0))
            {
                /* algebraic notation */
                /* bmvstr[0]: 7g7f bmvstr[1]:
                 * (+)P7g7f(+) bmvstr[2]: (+)P7f(+) */
                bmvstr[0][0] = COL_NAME(column(f));
                bmvstr[0][1] = ROW_NAME(row(f));
                bmvstr[0][2] = COL_NAME(column(t));
                bmvstr[0][3] = ROW_NAME(row(t));
                bmvstr[0][4] = '\0';

                if (promoted)
                {
                    bmvstr[1][0] = bmvstr[2][0] = '+';
                    bmvstr[1][1] = bmvstr[2][1] = pxx[board[f]];
                    strcpy(&bmvstr[1][2], &bmvstr[0][0]);
                    strcpy(&bmvstr[2][2], &bmvstr[0][2]);
                }
                else
                {
                    bmvstr[1][0] = bmvstr[2][0] = pxx[board[f]];
                    strcpy(&bmvstr[1][1], &bmvstr[0][0]);
                    strcpy(&bmvstr[2][1], &bmvstr[0][2]);
                }

                if (flags & promote)
                {
                    strcat(bmvstr[0], "+");
                    strcat(bmvstr[1], "+");
                    strcat(bmvstr[2], "+");
                }
            }
            else
            {
                bmvstr[0][0] = bmvstr[1][0] = bmvstr[2][0] = '\0';
            }
        }
    }
}


#ifndef QUIETBOOKGEN
static void
bkdisplay(char *s, int cnt, int moveno)
{
    static short pnt;
    struct leaf  *node;
    int r, c, l;

    pnt = TrPnt[2];
    printf("matches = %d\n", cnt);
    printf("inout move is :%s: move number %d side %s\n",
            s, moveno / 2 + 1, (moveno & 1) ? "white" : "black");

#ifndef SEMIQUIETBOOKGEN
    printf("legal moves are \n");

    while (pnt < TrPnt[3])
    {
        node = &Tree[pnt++];

        if (is_promoted[board[node->f]] )
            Balgbr(node->f | 0x80, node->t, (short) node->flags);
        else
            Balgbr(node->f, node->t, (short) node->flags);

        printf("%s %s %s\n",
               bmvstr[0], bmvstr[1], bmvstr[2]);
    }

    printf("\n current board is\n");

    for (r = (NO_ROWS - 1); r >= 0; r--)
    {
        for (c = 0; c <= (NO_COLS - 1); c++)
        {
            char pc;

            l = locn(r, c);
            pc = (is_promoted[board[l]] ? '+' : ' ');

            if (color[l] == neutral)
                printf(" -");
            else if (color[l] == black)
                printf("%c%c", pc, qxx[board[l]]);
            else
                printf("%c%c", pc, pxx[board[l]]);
        }

        printf("\n");
    }

    printf("\n");
    {
        short color;

        for (color = black; color <= white; color++)
        {
            short piece, c;

            printf((color == black) ? "black " : "white ");

            for (piece = pawn; piece <= king; piece++)
            {
                if ((c = Captured[color][piece]))
                    printf("%i%c ", c, pxx[piece]);
            }

            printf("\n");
        };
    }
#endif /* SEMIQUIETBOOKGEN */
}
#endif /* QUIETBOOKGEN */


/*
 * BVerifyMove(s, mv, moveno)
 *
 * Compare the string 's' to the list of legal moves available for the
 * opponent. If a match is found, make the move on the board.
 */

static int
BVerifyMove(char *s, unsigned short *mv, int moveno)
{
    static short pnt, tempb, tempc, tempsf, tempst, cnt;
    static struct leaf xnode;
    struct leaf  *node;

    *mv = 0;
    cnt = 0;
    MoveList(opponent, 2, -2, true);
    pnt = TrPnt[2];

    while (pnt < TrPnt[3])
    {
        node = &Tree[pnt++];

        if (is_promoted[board[node->f]] )
            Balgbr(node->f | 0x80, node->t, (short) node->flags);
        else
            Balgbr(node->f, node->t, (short) node->flags);

        if (strcmp(s, bmvstr[0]) == 0 || strcmp(s, bmvstr[1]) == 0 ||
            strcmp(s, bmvstr[2]) == 0)
        {
            cnt++;
            xnode = *node;
        }
    }

    if (cnt == 1)
    {
        short blockable;

        MakeMove(opponent, &xnode, &tempb,
                 &tempc, &tempsf, &tempst, &INCscore);

        if (SqAttacked(PieceList[opponent][0], computer, &blockable))
        {
            UnmakeMove(opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
            /* Illegal move in check */
#if !defined QUIETBOOKGEN
            puts("Illegal move (in check): %s");
            bkdisplay(s, cnt, moveno);
#endif
            return false;
        }
        else
        {
            *mv = (xnode.f << 8) | xnode.t;

            if (is_promoted[board[xnode.t]] )
                Balgbr(xnode.f | 0x80, xnode.t, 0);
            else
                Balgbr(xnode.f, xnode.t, 0);

            return true;
        }
    }

    /* Illegal move */
#if !defined QUIETBOOKGEN
    printf("Illegal move (no match): %s\n", s);
    bkdisplay(s, cnt, moveno);
#endif
    return false;
}


/*
 * RESET()
 *
 * Reset the board and other variables to start a new game.
 *
 */

static void
RESET(void)
{
    short l;

    flag.illegal = flag.mate = flag.quit
        = flag.reverse = flag.bothsides = flag.onemove = flag.force
        = false;

    flag.post &= xboard; /* [HGM] xboard: do not clear in XBoard mode */

    flag.material = flag.coords = flag.hash = flag.easy
        = flag.beep = flag.rcptr
        = true;

    flag.stars = flag.shade = flag.back = flag.musttimeout = false;
    flag.gamein = false;
    GenCnt   = 0;
    GameCnt  = 0;
    CptrFlag[0] = TesujiFlag[0] = false;
    opponent = black;
    computer = white;

    for (l = 0; l < NO_SQUARES; l++)
    {
        board[l]   = Stboard[l];
        color[l]   = Stcolor[l];
        Mvboard[l] = 0;
    }

    ClearCaptured();
    InitializeStats();
    hashbd = hashkey = 0;
}


static int
Vparse (FILE * fd, USHORT *mv, USHORT *flags, int moveno)
{
    int c, i;
    char s[255];

    *flags = 0;

    while (true)
    {
        while (((c = getc(fd)) == ' ')
               || (c == '!') || (c == '/') || (c == '\n'));

        if (c == '(')
        {
            /* amount of time spent for the last move */
            while (((c = getc(fd)) != ')') && (c != EOF));

            if (c == ')')
            {
                while (((c = getc(fd)) == ' ') || (c == '\n'));
            }
        }

        if (c == '[')
        {
            /* comment for the actual game */
            while (((c = getc(fd))) != ']' && (c != EOF));

            if (c == ']')
            {
                while (((c = getc(fd))) == ' ' || (c == '\n'));
            }
        }

        if (c == '\r')
            continue;

        if (c == '#')
        {
            /* comment */
            do
            {
                c = getc(fd);

                if (c == '\r')
                    continue;
                /* goes to end of line */

                if (c == '\n')
                    return 0;

                if (c == EOF)
                    return -1;
            }
            while (true);
        }

        s[i = 0] = (char) c;

        while ((c >= '0') && (c <= '9'))
        {
            c = getc(fd);
            s[++i] = (char) c;
        }

        if (c == '.')
        {
            while (((c = getc(fd)) == ' ') || (c == '.') || (c == '\n'));
            s[i = 0] = (char) c;
        }

        while (((c = getc(fd)) != '?') && (c != '!') && (c != ' ')
               && (c != '(') && (c != '\n') && (c != '\t') && (c != EOF))
        {
            if (c == '\r')
                continue;

            if ((c != 'x') && (c != '-') && (c != ',')
                && (c != ';') && (c != '='))
            {
                s[++i] = (char) c;
            }
        }

        s[++i] = '\0';

        if (c == '(')
        {
            while (((c = getc(fd)) != ')') && (c != EOF));

            if (c == ')')
                c = getc(fd);
        }

        if (c == EOF)
            return -1;

        if (s[0] == '#')
        {
            while ((c != '\n') && (c != EOF))
                c = getc(fd);

            if (c == EOF)
                return -1;
            else
                return 0;
        }

        if (strcmp(s, "draw") == 0)
            continue;
        else if (strcmp(s, "1-0") == 0)
            continue;
        else if (strcmp(s, "0-1") == 0)
            continue;
        else if (strcmp(s, "Resigns") == 0)
            continue;
        else if (strcmp(s, "Resigns.") == 0)
            continue;
        else if (strcmp(s, "Sennichite") == 0)
            continue;
        else if (strcmp(s, "Sennichite.") == 0)
            continue;
        else if (strcmp(s, "Jishogi") == 0)
            continue;
        else if (strcmp(s, "Jishogi.") == 0)
            continue;

        bhashkey = hashkey;
        bhashbd  = hashbd;

        i = BVerifyMove(s, mv, moveno);

        if (c == '?')
        {
            /* Bad move, not for the program to play */
            *flags |= BADMOVE;  /* Flag it ! */
            while (((c = getc(fd)) == '?') || (c == '!') || (c == '/'));
        }
#ifdef EASY_OPENINGS
        else if (c == '~')
        {
            /* Do not use by computer */
            *flags |= BADMOVE;  /* Flag it ! */

            while (((c = getc(fd)) == '?') || (c == '!') || (c == '/'));
        }
#endif
        else if (c == '!')
        {
            /* Good move */
            *flags |= GOODMOVE; /* Flag it ! */

            while (((c = getc(fd)) == '?') || (c == '!') || (c == '/'));
        }
        else if (c == '\r')
        {
            c = getc(fd);
        }

        if (c == '(' )
            while (((c = getc(fd)) != ')') && (c != EOF));

        if (!i)
        {
            /* flush to start of next */
            while (((c = getc(fd)) != '#') && (c != EOF));

            if (c == EOF)
            {
                return -1;
            }
            else
            {
                ungetc(c, fd);
                return i;
            }
        }

        return i;
    }
}


static struct gdxadmin ADMIN;
struct gdxadmin B;
static struct gdxdata DATA;

/* lts(l) returns most significant 16 bits of l */

#if SIZEOF_LONG == 8  /* 64-bit long i.e. 8 bytes */
#  define lts(x) (USHORT)(((x >> 48) & 0xfffe) | side)
#else
#  if defined USE_LTSIMP
static USHORT ltsimp(long x)
{
    USHORT n;
    n = (((x >> 16) & 0xfffe));
    return n;
}
#    define lts(x) (USHORT)(ltsimp(x) | side)
#  else
#    define lts(x) (USHORT)(((x >> 16)&0xfffe) | side)
#  endif
#endif


/* #define HashValue(l) lts(l) */
#define HashValue(l) (USHORT)(l & 0xffff)

static int gfd;

#define MAXOFFSET(B) ((B.booksize - 1) * sizeof_gdxdata + sizeof_gdxadmin)

static ULONG HashOffset(ULONG hashkey, struct gdxadmin *B)
{
    return (hashkey % B->booksize) * sizeof_gdxdata + sizeof_gdxadmin;
}


static ULONG NextOffset(struct gdxadmin *B, ULONG offset)
{
    offset += sizeof_gdxdata;
    if (offset > B->maxoffset)
        offset = sizeof_gdxadmin;
    return offset;
}


static void WriteAdmin(void)
{
    lseek(gfd, 0, SEEK_SET);
    write(gfd, (char *)&ADMIN, sizeof_gdxadmin);
}

static void WriteData(ULONG offset, int *mustwrite)
{
    if (!*mustwrite)
        return;

    lseek(gfd, offset, SEEK_SET);
    write(gfd, (char *)&DATA, sizeof_gdxdata);
    *mustwrite = false;
}

static int ReadAdmin(void)
{
    lseek(gfd, 0, SEEK_SET);
    return (sizeof_gdxadmin == read(gfd, (char *)&ADMIN, sizeof_gdxadmin));
}

static int ReadData(ULONG offset, struct gdxdata *DATA)
{
    lseek(gfd, offset, SEEK_SET);
    return (sizeof_gdxdata == read(gfd, (char *)DATA, sizeof_gdxdata));
}


/*
 * GetOpenings()
 *
 * CHECKME: is this still valid esp. wrt gnushogi.book?
 *
 * Read in the Opening Book file and parse the algebraic notation for a move
 * into an unsigned integer format indicating the from and to square. Create
 * a linked list of opening lines of play, with entry->next pointing to the
 * next line and entry->move pointing to a chunk of memory containing the
 * moves. More Opening lines of up to 100 half moves may be added to
 * gnushogi.book. But now it's a hashed table by position which yields a move
 * or moves for each position. It no longer knows about openings per se only
 * positions and recommended moves in those positions.
 *
 */

void
GetOpenings(void)
{
    ULONG currentoffset = 0;
    short i;
    int first;
    unsigned short side;
    short c;
    USHORT mv, flags;
    unsigned int x;
    unsigned int games = 0;
    LONG collisions = 0;
    char msg[80];

    FILE *fd;

    if ((fd = fopen(bookfile, "r")) == NULL)
        fd = fopen("gnushogi.tbk", "r");

    if (fd != NULL)
    {
        /* yes add to book */
        /* open book as writer */
        gfd = open(binbookfile, O_RDONLY | O_BINARY);

        if (gfd >= 0)
        {
            if (ReadAdmin())
            {
                B.bookcount = ADMIN.bookcount;
                B.booksize = ADMIN.booksize;
                B.maxoffset = ADMIN.maxoffset;

                if (B.booksize && !(B.maxoffset == MAXOFFSET(B)))
                {
                    printf("bad format %s\n", binbookfile);
                    exit(1);
                }
            }
            else
            {
                printf("bad format %s\n", binbookfile);
                exit(1);
            }
            close(gfd);
            gfd = open(binbookfile, O_RDWR | O_BINARY);

        }
        else
        {
            gfd = open(binbookfile, O_RDWR | O_CREAT | O_BINARY, 0644);

            ADMIN.bookcount = B.bookcount = 0;
            ADMIN.booksize = B.booksize = booksize;
            B.maxoffset = ADMIN.maxoffset = MAXOFFSET(B);
            DATA.hashbd = 0;
            DATA.hashkey = 0;
            DATA.bmove = 0;
            DATA.flags = 0;
            DATA.hint = 0;
            DATA.count = 0;
            WriteAdmin();
            printf("creating bookfile %s %ld %ld\n",
                    binbookfile, B.maxoffset, B.booksize);

            for (x = 0; x < B.booksize; x++)
            {
                int mustwrite = true;
                WriteData(sizeof_gdxadmin + x* sizeof_gdxdata, &mustwrite);
            }
        }

        if (gfd >= 0)
        {
            int mustwrite = false;
            /* setvbuf(fd, buffr, _IOFBF, 2048); */
            side = black;
            hashbd = hashkey = 0;
            i = 0;

            while ((c = Vparse(fd, &mv, &flags, i)) >= 0)
            {
                if (c == 1)
                {
                    /*
                     * If this is not the first move of an opening and
                     * if it's the first time we have seen it then
                     * save the next move as a hint.
                     */
                    i++;

                    if (i < bookmaxply + 2)
                    {
                        if (i > 1 && !(flags & BADMOVE))
                            DATA.hint = mv;

                        if (i < bookmaxply + 1)
                        {
                            /*
                             * See if this position and move already
                             * exist from some other opening.
                             */

                            WriteData(currentoffset, &mustwrite);
                            currentoffset = HashOffset(bhashkey, &B);
                            first = true;

                            while (true)
                            {
                                if (!ReadData(currentoffset, &DATA))
                                    break; /* corrupted binbook file */

                                if (DATA.bmove == 0)
                                    break;  /* free entry */

                                if (DATA.hashkey == HashValue(bhashkey)
                                    && DATA.hashbd == bhashbd)
                                {
                                    if (DATA.bmove == mv)
                                    {
                                        /*
                                         * Yes, so just bump count - count
                                         * is used to choose the opening
                                         * move in proportion to its
                                         * presence in the book.
                                         */

                                        DATA.count++;
                                        DATA.flags |= flags;
                                        mustwrite = true;
                                        break;
                                    }
                                    else
                                    {
                                        if (first)
                                            collisions++;

                                        if (DATA.flags & LASTMOVE)
                                        {
                                            DATA.flags &= (~LASTMOVE);
                                            mustwrite = true;
                                            WriteData(currentoffset, &mustwrite);
                                        }
                                    }
                                }

                                currentoffset = NextOffset(&B, currentoffset);
                                first = false;
                            }

                            /*
                             * Doesn't exist so add it to the book.
                             */

                            if (!mustwrite)
                            {
                                B.bookcount++;

                                if ((B.bookcount % 1000) == 0)
                                {
                                    /* CHECKME: may want to get rid of this,
                                     * especially for xshogi. */
                                    printf("%ld rec %d openings "
                                           "processed\n",
                                           B.bookcount, games);
                                }

                                /* initialize a record */
                                DATA.hashbd = bhashbd;
                                DATA.hashkey = HashValue(bhashkey);
                                DATA.bmove = mv;
                                DATA.flags = flags | LASTMOVE;
                                DATA.count = 1;
                                DATA.hint = 0;
                                mustwrite = true;
                            }
                        }
                    }

                    computer = opponent;
                    opponent = computer ^ 1;

                    side = side ^ 1;
                }
                else if (i > 0)
                {
                    /* reset for next opening */
                    games++;
                    WriteData(currentoffset, &mustwrite);
                    RESET();
                    i = 0;
                    side = black;
                }
            }

            WriteData(currentoffset, &mustwrite);
            fclose(fd);
            /* write admin rec with counts */
            ADMIN.bookcount = B.bookcount;
            WriteAdmin();

            close(gfd);
        }
    }

    if (binbookfile != NULL)
    {
        /* open book as reader */
        gfd = open(binbookfile, O_RDONLY | O_BINARY);

        if (gfd >= 0)
        {
            if (ReadAdmin() && (!ADMIN.booksize
                                || (ADMIN.maxoffset == MAXOFFSET(ADMIN))))
            {
                B.bookcount = ADMIN.bookcount;
                B.booksize  = ADMIN.booksize;
                B.maxoffset = ADMIN.maxoffset;
            }
            else
            {
                printf("bad format %s\n", binbookfile);
                exit(1);
            }

        }
        else
        {
            B.bookcount = 0;
            B.booksize = booksize;

        }

        sprintf(msg, "Book used %lu(%lu).", B.bookcount, B.booksize);
        dsp->ShowMessage(msg);
    }

    /* Set everything back to start the game. */
    Book = BOOKFAIL;
    RESET();

    /* Now get ready to play .*/
    if (!B.bookcount)
    {
        dsp->ShowMessage("Can't find book.");
        Book = 0;
    }
}


/*
 * OpeningBook(hint)
 *
 * Go through each of the opening lines of play and check for a match with
 * the current game listing. If a match occurs, generate a random
 * number. If this number is the largest generated so far then the next
 * move in this line becomes the current "candidate".  After all lines are
 * checked, the candidate move is put at the top of the Tree[] array and
 * will be played by the program.  Note that the program does not handle
 * book transpositions.
 */

int
OpeningBook(unsigned short *hint)
{
    ULONG currentoffset;
    unsigned short r, m;
    int possibles = TrPnt[2] - TrPnt[1];

    gsrand((unsigned int) time((long *) 0));
    m = 0;

    /*
     * Find all the moves for this position  - count them and get their
     * total count.
     */

    {
        USHORT i, x;
        USHORT rec = 0;
        USHORT summ = 0;
        USHORT h = 0, b = 0;
        struct gdxdata OBB[128];

        if (B.bookcount == 0)
        {
            Book--;
            return false;
        }

        x = 0;
        currentoffset = HashOffset(hashkey, &B);
#ifdef BOOKTEST
        printf("looking for book move, bhashbd = 0x%lx bhashkey = 0x%x\n",
               (ULONG)hashbd, HashValue(hashkey));
#endif
        while (true)
        {
            if (!ReadData(currentoffset, &OBB[x]))
                break;

            if (OBB[x].bmove == 0)
                break;

#ifdef BOOKTEST
            printf("compare with bhashbd = 0x%lx bhashkey = 0x%x\n",
                   OBB[x].hashbd, OBB[x].hashkey);
#endif
            if ((OBB[x].hashkey == HashValue(hashkey))
                && (OBB[x].hashbd == (ULONG)hashbd))
            {
                x++;

                if (OBB[x-1].flags & LASTMOVE)
                    break;
            }

            currentoffset = NextOffset(&B, currentoffset);
        }

#ifdef BOOKTEST
        printf("%d book move(s) found.\n", x);
#endif

        if (x == 0)
        {
            Book--;
            return false;
        }

        for (i = 0; i < x; i++)
        {
            if (OBB[i].flags & BADMOVE)
            {
                m = OBB[i].bmove;

                /* Is the move in the MoveList? */
                for (b = TrPnt[1]; b < (unsigned) TrPnt[2]; b++)
                {
                    if (((Tree[b].f << 8) | Tree[b].t) == m)
                    {
                        if (--possibles)
                            Tree[b].score = DONTUSE;
                        break;
                    }
                }
            }
            else
            {
#if defined BOOKTEST
                char s[20];
                movealgbr(m = OBB[i].bmove, s);
                printf("finding book move: %s\n", s);
#endif
                summ += OBB[i].count;
            }
        }

        if (summ == 0)
        {
            Book--;
            return false;
        }

        r = (urand() % summ);

        for (i = 0; i < x; i++)
        {
            if (!(OBB[i].flags & BADMOVE))
            {
                if (r < OBB[i].count)
                {
                    rec = i;
                    break;
                }
                else
                {
                    r -= OBB[i].count;
                }
            }
        }

        h = OBB[rec].hint;
        m = OBB[rec].bmove;

        /* Make sure the move is in the MoveList. */
        for (b = TrPnt[1]; b < (unsigned) TrPnt[2]; b++)
        {
            if (((Tree[b].f << 8) | Tree[b].t) == m)
            {
                Tree[b].flags |= book;
                Tree[b].score = 0;
                break;
            }
        }

        /* Make sure it's the best. */

        pick(TrPnt[1], TrPnt[2] - 1);

        if (Tree[TrPnt[1]].score)
        {
            /* no! */
            Book--;
            return false;
        }

        /* Ok, pick up the hint and go. */
        *hint = h;
        return true;
    }

    Book--;
    return false;
}
