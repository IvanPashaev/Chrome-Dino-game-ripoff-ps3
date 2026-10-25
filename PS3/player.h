#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef struct player {
    int x;
    int y;
    int jumpcount;
    bool IsJumping;
    int score;
} player;

static player Player;
static player HomePlayer;

static void TriggerJumpAt(int XValue) {
    if (!Player.IsJumping && HomePlayer.x >= XValue && HomePlayer.x < XValue + 2) {
        Player.IsJumping = true;
    }
}

static void MoveHomePlayer(void) {
    HomePlayer.x += 2;
}

static void ResetHomePlayerState(void) {
    HomePlayer.x = 100;
    HomePlayer.y = 350;
    Player.jumpcount = 10;
    Player.IsJumping = false;
}

#endif
