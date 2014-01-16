/*
 * FILE: pattern-common.c
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

short
ValueOfOpeningName (char *name)
{
    short i;
    i = (name[0] == 'C') ? 0 : 100;

    switch (name[7])
    {
    case 'S':
        i += 10;
        break;

    case 'R':
        i += 20;
        break;

    case 'U':
        i += 30;
        break;

    default:
        i += 40;
        break;
    }

    switch (name[9])
    {
    case 'S':
        i += 1;
        break;

    case 'R':
        i += 2;
        break;

    case 'U':
        i += 3;
        break;

    default:
        i += 4;
        break;
    }

    return i;
}
