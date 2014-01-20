/*
 * FILE: tcontrl.c
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
#include <math.h>

#define ALTERNATIVE_TC


/*
 * In a networked enviroment gnushogi might be compiled on different hosts
 * with different random number generators; that is not acceptable if they
 * are going to share the same transposition table.
 */

static unsigned long next = 1;

unsigned int
urand(void)
{
    next *= 1103515245;
    next += 12345;
    return ((unsigned int) (next >> 16) & 0xFFFF);
}



void
gsrand(unsigned int seed)
{
    next = seed;
}



void
TimeCalc()
{
    /* adjust number of moves remaining in gamein games */
    int increment = 0;
    int topsum = 0;
    int tcompsum = 0;
    int me, him;
    int i;

    /* Don't do anything until you have enough numbers. */
    if (GameCnt < (MINGAMEIN * 2))
        return;

    /* Calculate average time in sec for last MINGAMEIN moves. */
    for (i = 0; i < MINGAMEIN; i++)
    {
        tcompsum += timecomp[i];
        topsum += timeopp[i];
    }

    topsum   /= (100 * MINGAMEIN);
    tcompsum /= (100 * MINGAMEIN);

    /* If I have less time than opponent add another move. */
    me  = TimeControl.clock[computer] / 100;
    him = TimeControl.clock[opponent] / 100;

    if (me < him)
        increment += 2;

    if (((him - me) > 60) || ((me < him) && (me < 120)))
        increment++;

    /* If I am losing more time with each move add another. */
    /* If (!((me - him) > 60) && tcompsum > topsum) increment++; */

    if (tcompsum > topsum)
    {
        increment += 2;
    }
    else if ((TimeControl.moves[computer] < MINMOVES) && !increment)
    {
        /* ... but don't let moves go below MINMOVES. */
        increment++;
    }
    else if ((me > him) && (tcompsum < topsum))
    {
        /* If I am doing really well use more time per move. */
        increment = -1;
    }

    /* If not fischer clock be careful about time. */
    /* CHECKME: what's a fischer clock? */

    if ((TCadd == 0) && (increment > 0))
        increment += 2;

    if ((me == 0) && (increment > 0))
        increment += 2;

    TimeControl.moves[computer] += increment;
}



/*
 * Set ResponseTime, TCcount, and TCleft.
 */

void SetResponseTime(short side)
{
#ifdef ALTERNATIVE_TC
    int DetermineTCcount = true;

    if (TCflag)
    {
        TCcount = 0;

        if (TimeControl.moves[side] < 1)
            TimeControl.moves[side] = 1;

        /* special case time per move specified */
        if (flag.onemove)
        {
            ResponseTime = TimeControl.clock[side] - 100;
            TCleft = 0;
        }
        else
        {
            /* calculate avg time per move remaining */
            if (TimeControl.clock[side] <= 0)
            {
                ResponseTime = 0;
                TCleft = (long)MINRESPONSETIME / MAXTCCOUNTX;
            }
            else
            {
                short rtf = in_opening_stage ? 8 : 2;
                short tcq = in_opening_stage ? 2 : 4;

                TimeControl.clock[side] += TCadd;
                ResponseTime = (TimeControl.clock[side])
                    / (((TimeControl.moves[side]) * rtf) + 1);
                TCleft = (long)ResponseTime / tcq;
                ResponseTime += TCadd / 2;
            }

            if (TimeControl.moves[side] < 5)
            {
                TCcount = MAXTCCOUNTX - 10;

                if (TCcount < 0)
                    TCcount = 0;

                DetermineTCcount = false;
            }
        }

        if (ResponseTime < MINRESPONSETIME)
        {
            ResponseTime = MINRESPONSETIME;
            TCcount = MAXTCCOUNTX - 10;

            if (TCcount < 0)
                TCcount = 0;

            DetermineTCcount = false;
        }

        if (!hard_time_limit && (ResponseTime < 2 * MINRESPONSETIME))
        {
            TCcount = MAXTCCOUNTX - 10;

            if (TCcount < 0)
                TCcount = 0;

            DetermineTCcount = false;
        }
    }
    else
    {
        TCleft = 0;
        ResponseTime = MaxResponseTime;
        ElapsedTime(COMPUTE_AND_INIT_MODE);
    }

    if (DetermineTCcount)
    {
        if (TCleft )
        {
            int AllowedCounts
                = ((int)((TimeControl.clock[side] - ResponseTime)) / 2)
                / TCleft;

            if (AllowedCounts <= 0)
                TCcount = MAXTCCOUNTX;
            else if (AllowedCounts > MAXTCCOUNTX)
                TCcount = 0;
            else
                TCcount = MAXTCCOUNTX - AllowedCounts;
        }
        else
        {
            TCcount = MAXTCCOUNTX;
        }
    }

    if (ResponseTime < MINRESPONSETIME)
        ResponseTime = MINRESPONSETIME;

#else

    if (TCflag)
    {
        TCcount = 0;

        if (TimeControl.moves[side] < 1)
            TimeControl.moves[side] = 1;

        /* special case time per move specified */
        if (flag.onemove)
        {
            ResponseTime = TimeControl.clock[side] - 100;
            TCleft = 0;
        }
        else
        {
            /* calculate avg time per move remaining */
            TimeControl.clock[side] += TCadd;

            ResponseTime = (TimeControl.clock[side])
                / (((TimeControl.moves[side]) * 2) + 1);
            TCleft = (int) ResponseTime / 3;
            ResponseTime += TCadd / 2;

            if (TimeControl.moves[side] < 5)
                TCcount = MAXTCCOUNTX - 10;
        }

        if (ResponseTime < 101)
        {
            ResponseTime = 100;
            TCcount = MAXTCCOUNTX - 10;
        }
        else if (ResponseTime < 200)
        {
            TCcount = MAXTCCOUNTX - 10;
        }
    }
    else
    {
        ResponseTime = MaxResponseTime;
        TCleft = 0;
        ElapsedTime(COMPUTE_AND_INIT_MODE);
    }

    if (TCleft)
    {
        TCcount = ((int)((TimeControl.clock[side] - ResponseTime)) / 2)
            / TCleft;

        if (TCcount > MAXTCCOUNTX)
            TCcount = 0;
        else
            TCcount = MAXTCCOUNTX - TCcount;
    }
    else
    {
        TCcount = MAXTCCOUNTX;
    }
#endif

    assert(TCcount <= MAXTCCOUNTX);
}



void
CheckForTimeout(int score, int globalscore, int Jscore, int zwndw)
{
    if (flag.musttimeout || (Sdepth >= MaxSearchDepth))
        flag.timeout = true;

    else if (TCflag && (Sdepth > (MINDEPTH - 1)) && (TCcount < MAXTCCOUNTR))
    {
        if (killr0[1] != PrVar[1] /* || Killr0[2] != PrVar[2] */)
        {
            TCcount++;
            ExtraTime += TCleft;
        }

        if ((TCcount < MAXTCCOUNTR)
            && (abs(score - globalscore) / Sdepth) > ZDELTA)
        {
            TCcount++;
            ExtraTime += TCleft;
        }
    }

    if ((score > (Jscore - zwndw)) && (score > (Tree[1].score + 250)))
        ExtraTime = 0;

    ElapsedTime(COMPUTE_MODE);

    if (root->flags & exact)
        flag.timeout = true;
    /*else if (Tree[1].score < -SCORE_LIMIT) flag.timeout = true;
     */
#if defined OLDTIME || !defined HAVE_GETTIMEOFDAY
    else if (!(Sdepth < MINDEPTH)
             && TCflag
             && ((4 * et) > (2*ResponseTime + ExtraTime)))
        flag.timeout = true;
#else
    else if (!(Sdepth < MINDEPTH)
             && TCflag
             && ((int)(1.93913099l * (pow((double)et, 1.12446928l)))
                 > (ResponseTime + ExtraTime)))
        flag.timeout = true;
#endif

    if (flag.timeout)
        dsp->ShowMessage("timeout");
}


/*
 * Determine the time that has passed since the search was started. If the
 * elapsed time exceeds the target(ResponseTime + ExtraTime) then set timeout
 * to true which will terminate the search.
 * iop = COMPUTE_MODE calculate et, bump ETnodes
 * iop = COMPUTE_AND_INIT_MODE calculate et, set timeout if time exceeded,
 *     set reference time
 */
void
ElapsedTime(ElapsedTime_mode iop)
{
    long current_time;
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
#endif

    dsp->PollForInput();

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    current_time = tv.tv_sec*100 + (tv.tv_usec/10000);
#else
    et = ((current_time = time((long *) 0)) - time0) * 100;
#endif

#ifdef INTERRUPT_TEST
    if (iop == INIT_INTERRUPT_MODE)
    {
        itime0 = current_time;
    }
    else if (iop == COMPUTE_INTERRUPT_MODE)
    {
        it = current_time - itime0;
    }
    else
#endif
    {
#ifdef HAVE_GETTIMEOFDAY
        et = current_time - time0;
#endif
        ETnodes = NodeCnt + znodes;

        if (et < 0)
        {
#ifdef INTERRUPT_TEST
            printf("elapsed time %ld not positive\n", et);
#endif
            et = 0;
        }

        if (iop == COMPUTE_AND_INIT_MODE)
        {
            if ((et > (ResponseTime + ExtraTime)) && (Sdepth > MINDEPTH))
                flag.timeout = true;

            time0 = current_time;
        }

#ifdef QUIETBACKGROUND
        if (!background)
#endif
            dsp->UpdateClocks();
    }
}


void
SetTimeControl(void)
{
    if (TCflag)
    {
        TimeControl.moves[black] = TimeControl.moves[white] = TCmoves;
        TimeControl.clock[black] += 6000L * TCminutes + TCseconds * 100;
        TimeControl.clock[white] += 6000L * TCminutes + TCseconds * 100;
    }
    else
    {
        TimeControl.moves[black] = TimeControl.moves[white] = 0;
        TimeControl.clock[black] = TimeControl.clock[white] = 0;
    }

    flag.onemove = (TCmoves == 1);
    et = 0;
    ElapsedTime(COMPUTE_AND_INIT_MODE);
}
