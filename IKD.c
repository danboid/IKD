/** \file   IKD.c
 *  \brief  A Uzebox remake of the tank game in Atari's Combat.
 *  \author Dan MacDonald
 *          Score drawing code borrowed from Bradley Boccuzzi's
 *          Uzebox port of Pong.
 *  \date   2022
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

int seed = 0;

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

int Score[2] = {0, 0};

const char *numbers[10] = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9};

const char *numbers2[10] = {sc0, sc1, sc2, sc3, sc4, sc5, sc6, sc7, sc8, sc9};

struct bulletStruct {
  float x;
  float y;
  float vX;
  float vY;
  bool active;
  int age;
  int gridX;
  int gridY;
};

struct bulletStruct p1_bullet, p2_bullet;

struct tankStruct {
  float top;     // Top corner of the sprite in pixels
  float bottom;
  float left;
  float right;
  int angle;     // Which direction is the tank facing? 0 = up/north, 4 = right/east
  int x;         // Current 8x8 pixel tile horizontal grid position, starting from the left
  int y;         // Vertical tile position, starting from the top of the screen
  bool advance;  // Can the tank move forward in its current location and angle?
};

struct tankStruct p1_tank, p2_tank;

void initIKD(void);
void initMaze2(void);
void processTrig(void);
void processBullets(void);
void processTank1(void);
void processTank2(void);
void processScore(void);
void cuzeboxCOut(char str[]);
void cuzeboxHOut(int num);
void wallTankCollision(int tankN, int tankX, int tankY, int tankAngle);

// cuzeboxCOut() is used to print debug strings to the cuzebox console.
void cuzeboxCOut(char str[]) {
  for (int i = 0; str[i] != '\0'; i++) {
        _SFR_IO8(0X1a)=str[i];
  }
  _SFR_IO8(0X1a)='\n'; // Add newline after last character
}

// cuzeboxHOut() is used to print a hex value to the cuzebox console.
// eg cuzeboxHOut(GetTile(2,8));
void cuzeboxHOut(int num) {
  _SFR_IO8(25)=num;
  _SFR_IO8(0X1a)='\n'; // Add newline
}

int main() {
  // Basic prep work
  initIKD();
  // Load maze 2
  initMaze2();
  // Main loop
  while (1) {
    // wait until the next frame
    WaitVsync(1);
    seed++;
    processTank1();
    processTank2();
    processBullets();
    processScore();
  }
}

void initIKD(void) {
  InitMusicPlayer(patches);
  SetSpritesTileTable(tileset);
  SetTileTable(tileset); // Tile set to use for ClearVram()
  ClearVram();           // fill entire screen with first tile in the tileset

  Score[0] = 0;
  Score[1] = 0;
}

void processTank1(void) {
  MoveSprite(0, p1_tank.left, p1_tank.top, 1, 1); // position tank 1 sprite
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
    wallTankCollision(0,p1_tank.x,p1_tank.y,p1_tank.angle);
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
    wallTankCollision(0,p1_tank.x,p1_tank.y,p1_tank.angle);
  }
  if (tank1Pressed & BTN_A) {
    srand((unsigned)seed);
    if (p1_bullet.active == false) {
      p1_bullet.age = 0;
      p1_bullet.x = p1_tank.left;
      p1_bullet.y = p1_tank.top;
      p1_bullet.active = true;
      MapSprite2(1, bullet, 0); // map bullet
      MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
      TriggerFx(0, 0xFF, true);
    }
  }
  if (tank1Held & BTN_UP) {
    if (p1_tank.advance == true) {
    p1_tank.left += p1_bullet.vX / 2;
    p1_tank.top += p1_bullet.vY / 2;
    p1_tank.right = p1_tank.left + 8;
    p1_tank.bottom = p1_tank.top + 8;
    MoveSprite(0, p1_tank.left, p1_tank.top, 1, 1);
    // Update tanks position on the tile grid
    p1_tank.x = p1_tank.left / 8;
    p1_tank.y = p1_tank.top / 8;
    wallTankCollision(0,p1_tank.x,p1_tank.y,p1_tank.angle);
    }
  }
  tank1Prev = tank1Held;
}

void processTank2(void) {
  MoveSprite(2, p2_tank.left, p2_tank.top, 1, 1); // position tank 2 sprite
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
    wallTankCollision(1,p2_tank.x,p2_tank.y,p2_tank.angle);
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
    wallTankCollision(1,p2_tank.x,p2_tank.y,p2_tank.angle);
  }
  if (tank2Pressed & BTN_A) {
    srand((unsigned)seed);
    if (p2_bullet.active == false) {
      p2_bullet.age = 0;
      p2_bullet.x = p2_tank.left;
      p2_bullet.y = p2_tank.top;
      p2_bullet.active = true;
      MapSprite2(3, bullet, 0); // map bullet
      MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
      TriggerFx(0, 0xFF, true);
    }
  }
  if (tank2Held & BTN_UP) {
    if (p2_tank.advance == true) {
    p2_tank.left += p2_bullet.vX / 2;
    p2_tank.top += p2_bullet.vY / 2;
    p2_tank.right = p2_tank.left + 8;
    p2_tank.bottom = p2_tank.top + 8;
    MoveSprite(2, p2_tank.left, p2_tank.top, 1, 1);
    // Update tank 2's position on the tile grid
    p2_tank.x = p2_tank.left / 8;
    p2_tank.y = p2_tank.top / 8;
    wallTankCollision(1,p2_tank.x,p2_tank.y,p2_tank.angle);
  }
  }
  tank2Prev = tank2Held;
}

void processBullets(void) {
  if (p1_bullet.active == true && p1_bullet.age < 60) {
    p1_bullet.age++;
    p1_bullet.x += p1_bullet.vX * 3;
    p1_bullet.y += p1_bullet.vY * 3;
    p1_bullet.gridX = p1_bullet.x / 8;
    p1_bullet.gridY = p1_bullet.y / 8;
    MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
    if (p1_bullet.x >= p2_tank.left && p1_bullet.x <= p2_tank.right &&
        p1_bullet.y >= p2_tank.top && p1_bullet.y <= p2_tank.bottom) {
      p1_bullet.active = false;
      MapSprite2(1, blank, 0);
      TriggerFx(1, 0xFF, true);
      Score[0]++;
      if (Score[0] > 9) {
          Score[0] = 0;
      }

      p2_tank.left = rand() % 210;
      p2_tank.top = rand() % 210;
      p2_tank.right = p2_tank.left + 8;
      p2_tank.bottom = p2_tank.top + 8;
      p2_tank.angle = rand() % 15;
      tank2_current_sprite = tank2_sprites[p2_tank.angle];
      processTrig();
      MapSprite2(2, tank2_current_sprite, 0);
      p2_bullet.active = false;
      p2_bullet.age = 0;
    }
    else if (GetTile(p1_bullet.gridX, p1_bullet.gridY) == 0x25) {
      p1_bullet.active = false;
      MapSprite2(1, blank, 0);
    }
  } else {
    p1_bullet.active = false;
    MapSprite2(1, blank, 0);
  }

  if (p2_bullet.active == true && p2_bullet.age < 60) {
    p2_bullet.age++;
    p2_bullet.x += p2_bullet.vX * 3;
    p2_bullet.y += p2_bullet.vY * 3;
    p2_bullet.gridX = p2_bullet.x / 8;
    p2_bullet.gridY = p2_bullet.y / 8;
    MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
    if (p2_bullet.x >= p1_tank.left && p2_bullet.x <= p1_tank.right &&
        p2_bullet.y >= p1_tank.top && p2_bullet.y <= p1_tank.bottom) {
      p2_bullet.active = false;
      MapSprite2(3, blank, 0);
      TriggerFx(1, 0xFF, true);
      Score[1]++;
      if (Score[1] > 9) {
          Score[1] = 0;
      }

      p1_tank.left = rand() % 210;
      p1_tank.top = rand() % 210;
      p1_tank.right = p1_tank.left + 8;
      p1_tank.bottom = p1_tank.top + 8;
      p1_tank.angle = rand() % 15;
      tank1_current_sprite = tank1_sprites[p1_tank.angle];
      processTrig();
      MapSprite2(0, tank1_current_sprite, 0);
      p1_bullet.active = false;
      p1_bullet.age = 0;
    }
    else if (GetTile(p2_bullet.gridX, p2_bullet.gridY) == 0x25) {
      p2_bullet.active = false;
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

void processScore(void) {
  DrawMap2(6, 22, (numbers[Score[0]]));
  DrawMap2(18, 22, (numbers2[Score[1]]));
}

void initMaze2(void) {
  DrawMap2(0, 0, maze2);

  MapSprite2(0, tank1_090, 0); // setup tank 1 for drawing
  p1_tank.left = 8;              // set tank to the left
  p1_tank.top = 80;             // center tank vertically
  p1_tank.right = 13;
  p1_tank.bottom = 93;
  p1_tank.angle = 4;           // face right
  p1_tank.x = 1;
  p1_tank.y = 10;
  p1_tank.advance = true;
  p1_bullet.vX = 1;
  p1_bullet.vY = 0;
  p1_bullet.active = false;
  p1_bullet.age = 0;
  p1_bullet.gridX = 1;
  p1_bullet.gridY = 10;

  MapSprite2(2, tank2_270, 0); // setup tank 2 for drawing
  p2_tank.left = 202;             // set tank to the right
  p2_tank.top = 85;             // center tank vertically
  p2_tank.right = 210;
  p2_tank.bottom = 93;
  p2_tank.angle = 12;          // face left
  p2_tank.x = 26;
  p2_tank.y = 10;
  p2_tank.advance = true;
  p2_bullet.vX = -1;
  p2_bullet.vY = 0;
  p2_bullet.active = false;
  p2_bullet.age = 0;
  p2_bullet.gridX = 25;
  p2_bullet.gridY = 10;
}

void wallTankCollision(int tankN, int tankX, int tankY, int tankAngle) {
  if (tankAngle == 0) {
    if (GetTile(tankX, tankY) == 0x25) { // Is tank above a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 1) {
    if (GetTile(tankX, tankY) == 0x25) { // Is tank above a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 2) {
    if (GetTile(tankX, tankY) == 0x25) { // Is tank above a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 3) {
    if (GetTile(tankX, tankY) == 0x25) { // Is tank above a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 4) {
    if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 5) {
    if (GetTile(tankX, (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 6) {
    if (GetTile(tankX, (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 7) {
    if (GetTile(tankX, (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX + 1), tankY) == 0x25) { // Is tank to the right a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 8) {
    if (GetTile(tankX, (tankY + 1)) == 0x25) { // Is tank below a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 9) {
    if (tankY == 21) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (tankX == 0) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile(tankX, (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), tankY) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 10) {
    if (tankX == 0) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile(tankX, (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), tankY) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 11) {
    if (GetTile(tankX, (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), (tankY + 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), tankY) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 12) {
    if (GetTile((tankX - 1), tankY) == 0x25) { // Is tank to the left a wall?
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 13) {
    if (tankX == 0) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile(tankX, (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), tankY) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 14) {
    if (tankX == 0) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile(tankX, (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), tankY) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
  if (tankAngle == 15) {
    if (tankX == 0) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile(tankX, (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), (tankY - 1)) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else if (GetTile((tankX - 1), tankY) == 0x25) {
      if (tankN == 0) {
        p1_tank.advance = false;
      }
      else if (tankN == 1) {
        p2_tank.advance = false;
      }
    }
    else {
      if (tankN == 0) {
        p1_tank.advance = true;
      }
      else if (tankN == 1) {
        p2_tank.advance = true;
      }
    }
  }
}
