#ifndef DUNGEONENTRANCE_H
#define DUNGEONENTRANCE_H

#include "math3d.h"

class DungeonEntrance
{
public:
    DungeonEntrance();

    vec3 position;
    int seed;

    // when a player leaves a dungeon they will be given a certain amount of time before
    // they can enter again - to save it from loading straight away once they leave
    float waitTime;
};

#endif // DUNGEONENTRANCE_H
