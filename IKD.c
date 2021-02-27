/** \file   IKD.c
 *  \brief  A Uzebox remake of the tank games in Atari's Combat 
 *  \author Dan MacDonald 
 *  \date   2020-2021
 */


#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uzebox.h>
#include "data/sfx.inc"
#include "data/tileset.inc"

int tank1Prev = 0;     // Previous button
int tank1Held = 0;     // buttons that are held right now
int tank1Pressed = 0;  // buttons that were pressed this frame
int tank1Released = 0; // buttons that were released this frame

int tank2Prev = 0;
int tank2Held = 0;
int tank2Pressed = 0;
int tank2Released = 0;

float angles[] = {0,   23,  45,  68,  90,  113, 135, 158,
                  180, 203, 225, 248, 270, 293, 315, 338};

const char *tank1_sprites[16] = {tank1_000, tank1_023, tank1_045, tank1_068,
                                 tank1_090, tank1_113, tank1_135, tank1_158,
                                 tank1_180, tank1_203, tank1_225, tank1_248,
                                 tank1_270, tank1_293, tank1_315, tank1_338};

const char *tank2_sprites[16] = {tank2_000, tank2_023, tank2_045, tank2_068,
                                 tank2_090, tank2_113, tank2_135, tank2_158,
                                 tank2_180, tank2_203, tank2_225, tank2_248,
                                 tank2_270, tank2_293, tank2_315, tank2_338};

const char *tank1_current_sprite, *tank2_current_sprite;

struct bulletStruct {
  float x;
  float y;
  float vX;
  float vY;
  bool active;
  int age;
};

struct bulletStruct p1_bullet, p2_bullet;

struct tankStruct {
  float x;
  float y;
  int angle;
};

struct tankStruct p1_tank, p2_tank;

void initIKD(void);
void processTrig(void);
void processBullets(void);
void processTank1(void);
void processTank2(void);

int main() {
  // some basic prep work
  initIKD();
  // Main loop
  while (1) {
    // wait until the next frame
    WaitVsync(1);
    ClearVram(); // wipe screen each frame
    processTank1();
    processTank2();
    processBullets();
  }
}

void initIKD(void) {
  InitMusicPlayer(patches);
  SetSpritesTileTable(
      tileset);          // sets the tiles to be used for our various sprites
  SetTileTable(tileset); // Tile set to use for ClearVram()
  ClearVram();           // fill entire screen with first tile in the tileset

  MapSprite2(0, tank1_090, 0); // setup tank 1 for drawing
  p1_tank.x = 10;              // set tank to the left
  p1_tank.y = 112;             // center tank vertically
  p1_tank.angle = 4;           // face right
  p1_bullet.vX = 1;
  p1_bullet.vY = 0;
  p1_bullet.active = false;
  p1_bullet.age = 0;

  MapSprite2(2, tank2_270, 0); // setup tank 2 for drawing
  p2_tank.x = 210;             // set tank to the right
  p2_tank.y = 112;             // center tank vertically
  p2_tank.angle = 12;          // face left
  p2_bullet.vX = -1;
  p2_bullet.vY = 0;
  p2_bullet.active = false;
  p2_bullet.age = 0;
}

void processTank1(void) {
  MoveSprite(0, p1_tank.x, p1_tank.y, 1, 1); // position tank 1 sprite
  tank1Held = ReadJoypad(0); // read in our player one joypad input
  tank1Pressed = tank1Held & (tank1Held ^ tank1Prev);

  if (tank1Pressed & BTN_RIGHT) {
    p1_tank.angle++; // move forward to next animation frame
    if (p1_tank.angle == 16) {
      p1_tank.angle = 0;
    }
    tank1_current_sprite = tank1_sprites[p1_tank.angle];
    processTrig();
    MapSprite2(0, tank1_current_sprite, 0);
  }
  if (tank1Pressed & BTN_LEFT) {
    if (p1_tank.angle == 0) {
      p1_tank.angle = 15;
    } else {
      p1_tank.angle--; // move back to previous animation frame
    }
    tank1_current_sprite = tank1_sprites[p1_tank.angle];
    processTrig();
    MapSprite2(0, tank1_current_sprite, 0);
  }
  if (tank1Pressed & BTN_A) {
    if (p1_bullet.active == false) {
      p1_bullet.age = 0;
      p1_bullet.x = p1_tank.x;
      p1_bullet.y = p1_tank.y;
      p1_bullet.active = true;
      MapSprite2(1, bullet, 0); // map bullet
      MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
      TriggerFx(0, 0xFF, true);
    }
  }
  if (tank1Held & BTN_UP) {
    p1_tank.x += p1_bullet.vX / 2;
    if (p1_tank.x < 0) {
      p1_tank.x += 1;
    }
    if (p1_tank.x > 220) {
      p1_tank.x -= 1;
    }
    p1_tank.y += p1_bullet.vY / 2;
    if (p1_tank.y < 0) {
      p1_tank.y += 1;
    }
    if (p1_tank.y > 210) {
      p1_tank.y -= 1;
    }
    MoveSprite(0, p1_tank.x, p1_tank.y, 1, 1);
  }
  tank1Prev = tank1Held;
}

void processTank2(void) {
  MoveSprite(2, p2_tank.x, p2_tank.y, 1, 1); // position tank 2 sprite
  tank2Held = ReadJoypad(1);                 // read player 2 input
  tank2Pressed = tank2Held & (tank2Held ^ tank2Prev);

  if (tank2Pressed & BTN_RIGHT) {
    p2_tank.angle++; // move forward to next animation frame
    if (p2_tank.angle == 16) {
      p2_tank.angle = 0;
    }
    tank2_current_sprite = tank2_sprites[p2_tank.angle];
    processTrig();
    MapSprite2(2, tank2_current_sprite, 0);
  }
  if (tank2Pressed & BTN_LEFT) {
    if (p2_tank.angle == 0) {
      p2_tank.angle = 15;
    } else {
      p2_tank.angle--; // move back to previous animation frame
    }
    tank2_current_sprite = tank2_sprites[p2_tank.angle];
    processTrig();
    MapSprite2(2, tank2_current_sprite, 0);
  }
  if (tank2Pressed & BTN_A) {
    if (p2_bullet.active == false) {
      p2_bullet.age = 0;
      p2_bullet.x = p2_tank.x;
      p2_bullet.y = p2_tank.y;
      p2_bullet.active = true;
      MapSprite2(3, bullet, 0); // map bullet
      MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
      TriggerFx(0, 0xFF, true);
    }
  }
  if (tank2Held & BTN_UP) {
    p2_tank.x += p2_bullet.vX / 2;
    if (p2_tank.x < 0) {
      p2_tank.x += 1;
    }
    if (p2_tank.x > 220) {
      p2_tank.x -= 1;
    }
    p2_tank.y += p2_bullet.vY / 2;
    if (p2_tank.y < 0) {
      p2_tank.y += 1;
    }
    if (p2_tank.y > 210) {
      p2_tank.y -= 1;
    }
    MoveSprite(2, p2_tank.x, p2_tank.y, 1, 1);
  }
  tank2Prev = tank2Held;
}

void processBullets(void) {
  if (p1_bullet.active == true && p1_bullet.age < 60) {
    p1_bullet.age++;
    p1_bullet.x += p1_bullet.vX * 3;
    p1_bullet.y += p1_bullet.vY * 3;
    MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
    if (p1_bullet.x >= p2_tank.x && p1_bullet.x <= p2_tank.x + 8 &&
        p1_bullet.y >= p2_tank.y && p1_bullet.y <= p2_tank.y + 8) {
      p1_bullet.active = false;
      MapSprite2(1, blank, 0);
      MapSprite2(2, blank, 0);
    }
  } else {
    p1_bullet.active = false;
    MapSprite2(1, blank, 0);
  }

  if (p2_bullet.active == true && p2_bullet.age < 60) {
    p2_bullet.age++;
    p2_bullet.x += p2_bullet.vX * 3;
    p2_bullet.y += p2_bullet.vY * 3;
    MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
    if (p2_bullet.x >= p1_tank.x && p2_bullet.x <= p1_tank.x + 8 &&
        p2_bullet.y >= p1_tank.y && p2_bullet.y <= p1_tank.y + 8) {
      p2_bullet.active = false;
      MapSprite2(0, blank, 0);
      MapSprite2(3, blank, 0);
    }
  } else {
    p2_bullet.active = false;
    MapSprite2(3, blank, 0);
  }
}

void processTrig(void) {
  p1_bullet.vX = sin(2 * M_PI * (angles[p1_tank.angle] / 360));
  p1_bullet.vY = -cos(2 * M_PI * (angles[p1_tank.angle] / 360));
  p2_bullet.vX = sin(2 * M_PI * (angles[p2_tank.angle] / 360));
  p2_bullet.vY = -cos(2 * M_PI * (angles[p2_tank.angle] / 360));
}
