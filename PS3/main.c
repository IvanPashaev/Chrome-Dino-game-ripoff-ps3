#include <malloc.h>
#include <math.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sysmodule/sysmodule.h>
#include <time.h>

#include "game.h"

#include <soundlib/audioplayer.h>
#include <soundlib/spu_soundlib.h>

#include "jump_mp3.h"
#include "spu_soundmodule.bin.h"

#include "Home_Screen_png.h"
#include "cave_Background_png.h"
#include "dino_sprite_png.h"
#include "object_png.h"

#define JUMP_VOICE 2
#define JUMP_MAX_DECODED_BYTES (48000 * 2 * 2 * 2)

static short *jump_samples = NULL;
static int jump_samples_size = 0;
static int jump_freq = 0;
static int jump_is_stereo = 0;

void InitializePS3Audio(void) {
  u32 spu;
  sysSpuImage spu_image;
  sysSpuInitialize(6, 5);
  sysSpuRawCreate(&spu, NULL);
  sysSpuImageImport(&spu_image, spu_soundmodule_bin, 0);
  sysSpuRawImageLoad(spu, &spu_image);
  SND_Init(spu);
}

static Texture2D LoadEmbeddedTexture(const char *fileType,
                                     const unsigned char *fileData,
                                     int dataSize) {
  Texture2D texture = {0};
  Image image = LoadImageFromMemory(fileType, fileData, dataSize);

  if (image.data == NULL) {
    TRACELOG(LOG_WARNING, "TEXTURE: Failed to decode embedded image");
    return texture;
  }

  texture = LoadTextureFromImage(image);
  UnloadImage(image);

  return texture;
}

static void LoadJumpSound(void) {
  jump_samples = memalign(32, JUMP_MAX_DECODED_BYTES);
  if (jump_samples == NULL) {
    TRACELOG(LOG_WARNING, "AUDIO: Failed to allocate jump sound buffer");
    return;
  }

  memset(jump_samples, 0, JUMP_MAX_DECODED_BYTES);
  jump_samples_size = JUMP_MAX_DECODED_BYTES;

  if (DecodeAudio((void *)jump_mp3, (int)jump_mp3_size, jump_samples,
                  &jump_samples_size, &jump_freq, &jump_is_stereo) != 0) {
    TRACELOG(LOG_WARNING, "AUDIO: Failed to decode jump.mp3");
    free(jump_samples);
    jump_samples = NULL;
    jump_samples_size = 0;
  }
}

static void PlayJumpSound(void) {
  if (jump_samples == NULL)
    return;

  SND_Pause(0);
  SND_SetVoice(JUMP_VOICE,
               jump_is_stereo ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT,
               jump_freq, 0, jump_samples, jump_samples_size, MAX_VOLUME,
               MAX_VOLUME, NULL);
}

static void StopJumpSound(void) { SND_StopVoice(JUMP_VOICE); }

static bool IsJumpPressed(void) {
  return IsKeyPressed(KEY_SPACE) ||
         IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}

static bool IsStartPressed(void) {
  return IsJumpPressed() ||
         IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT);
}

int main(void) {
  GameInit();
  InitWindow(500, 500, "CDG-Ripoff");
  SetTargetFPS(60);
  InitializePS3Audio();
  LoadJumpSound();
  Texture2D home_screen =
      LoadEmbeddedTexture(".png", Home_Screen_png, (int)Home_Screen_png_size);
  Texture2D play_screen = LoadEmbeddedTexture(".png", cave_Background_png,
                                              (int)cave_Background_png_size);
  Texture2D player_main =
      LoadEmbeddedTexture(".png", dino_sprite_png, (int)dino_sprite_png_size);
  Texture2D crystal_sprite =
      LoadEmbeddedTexture(".png", object_png, (int)object_png_size);

  const float gameWidth = 500.0f;
  const float gameHeight = 500.0f;
  Camera2D camera = {0};
  camera.target = (Vector2){0.0f, 0.0f};
  camera.zoom = fminf((float)GetScreenWidth() / gameWidth,
                      (float)GetScreenHeight() / gameHeight);
  camera.offset.x = ((float)GetScreenWidth() - gameWidth * camera.zoom) * 0.5f;
  camera.offset.y =
      ((float)GetScreenHeight() - gameHeight * camera.zoom) * 0.5f;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);

    switch (map.id) {
    case 1:
      DrawTexture(home_screen, 0, 0, WHITE);
      DrawTexture(player_main, HomePlayer.x, HomePlayer.y, WHITE);
      TriggerJumpAt(190);
      TriggerJumpAt(410);
      MoveHomePlayer();

      if (Player.IsJumping == true) {
        if (Player.jumpcount >= -10) {
          HomePlayer.y -= (Player.jumpcount * abs(Player.jumpcount)) * .2;
          HomePlayer.x += 2;
          Player.jumpcount -= 1;
        } else {
          Player.jumpcount = 10;
          HomePlayer.y = 350;
          Player.IsJumping = false;
        }
      }

      if (HomePlayer.x >= 500) {
        ResetHomePlayerState();
      }

      if (IsStartPressed()) {
        Player.x = 100;
        Player.y = 380;
        Player.jumpcount = 10;
        Player.IsJumping = false;
        map.id = 2;
        PlayJumpSound();
      } // Transition To The Main Game Screen

      break;
    case 2:
      DrawTexture(play_screen, 0, 0, LIGHTGRAY);
      DrawTexture(crystal_sprite, crystal.x, 380, WHITE);
      DrawTexture(player_main, Player.x, Player.y, WHITE);

      if (IsJumpPressed() && !Player.IsJumping) {
        PlayJumpSound();
        Player.IsJumping = true;
      }

      if (Player.IsJumping == true) {
        if (Player.jumpcount >= -10) {
          Player.y -= (Player.jumpcount * abs(Player.jumpcount)) * .4;
          Player.jumpcount -= 1;
        } else {
          Player.jumpcount = 10;
          Player.y = 380;
          StopJumpSound();
          Player.IsJumping = false;
        }
      }

      MoveCrystal();

      if (IfCrystalCollision()) {
        StopJumpSound();
        ResetGameState();
        break;
      }

      if (crystal.x <= 0) {
        ResetCrystalState();
      }

      DrawText(TextFormat("Score: %02i", Player.score), 375, 200, 20, BLACK);
      break;
    }

    EndMode2D();
    EndDrawing();
  }
  UnloadTexture(home_screen);
  UnloadTexture(play_screen);
  UnloadTexture(player_main);
  UnloadTexture(crystal_sprite);
  StopJumpSound();
  SND_End();
  free(jump_samples);
  jump_samples = NULL;
  CloseWindow();

  return 0;
}
