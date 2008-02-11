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
