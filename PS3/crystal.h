#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <stdbool.h>

#include "player.h"

typedef struct Crystal {
    int x;
} Crystal;

static Crystal crystal;

static void MoveCrystal(void) {
    crystal.x -= 10;
}

static bool IfCrystalCollision(void) {
    const int spriteSize = 50;
    const int groundY = 380;

    return (Player.x < crystal.x + spriteSize) &&
           (Player.x + spriteSize > crystal.x) &&
           (Player.y < groundY + spriteSize) &&
           (Player.y + spriteSize >= groundY);
}

static void ResetCrystalState(void) {
    crystal.x = 500;
    Player.score++;
}

#endif
