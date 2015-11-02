#ifndef RIOT_UNITS
#define RIOT_UNITS

#include "riotCommon.h"


/* Data Types */

struct Inmate {/*

 Inmate stores the information related to inmate units.*/

    char type;
    short speed;
    short healthMax;
    short healthCur;
    char repCost;
    short position;
    struct Inmate *next;
}Inmate;


struct Guard {/*

 Inmate stores the information related to guard units.*/

    char type;
    enum AI ai;
    short damage;
    short range[10];
    struct Guard *next;
}Guard;


/* Function Prototypes */

_Bool unitAdd(char InmateType);/*

DESCRIPTION: unitAdd() is used to add an inmate to the game.

ARGUMENTS: The character pressed by the user which corresponds to an
 associated unit type.

PRECONDITIONS: The character provided corresponds with an inmate type
 specified within the Inmate enum.

POSTCONDITIONS: An Inmate is added to the Inmate list.*/


void inmateMove(struct Inmate *inmate);/*

 DESCRIPTION: Move inmate every turn by its speed.*/


void guardAttack(struct Guard *guard, struct Inmate *inmate);/*

 DESCRIPTION: Compares the positions of every inmate, and the positions of
 attack of every guard if the units position matches the area of attack of
 the guard than subtract its health by the guards damage*/


#endif //RIOT_UNITS
