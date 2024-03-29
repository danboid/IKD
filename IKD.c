/** \file   IKD.c
 *  \brief  A Uzebox remake of the tank game in Atari's Combat.
 *  \author Dan MacDonald
 *          BIG thanks to D3thAdd3r for his fantastic C64 Commando theme rendition!
 *          Score drawing code borrowed from Bradley Boccuzzi's Uzebox port of Pong.
 *  \date   2024
 */


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uzebox.h>

#include "data/patches.inc"
#include "data/tileset.inc"
#include "data/music-compressed.inc"

int tank1Prev = 0;     // Previous button
int tank1Held = 0;     // buttons that are held right now
int tank1Pressed = 0;  // buttons that were pressed this frame
int tank1Released = 0; // buttons that were released this frame

int tank2Prev = 0;
int tank2Held = 0;
int tank2Pressed = 0;
int tank2Released = 0;

int seed, maze, nextX, nextY = 0;

float angles[] = {0,   23,  45,  68,  90,  113, 135, 158,
                  180, 203, 225, 248, 270, 293, 315, 338};

#define WALL_TILE       0x64

const char *tank1_sprites[16] = {tank1_000, tank1_023, tank1_045, tank1_068,
                                 tank1_090, tank1_113, tank1_135, tank1_158,
                                 tank1_180, tank1_203, tank1_225, tank1_248,
                                 tank1_270, tank1_293, tank1_315, tank1_338};

const char *tank2_sprites[16] = {tank2_000, tank2_023, tank2_045, tank2_068,
                                 tank2_090, tank2_113, tank2_135, tank2_158,
                                 tank2_180, tank2_203, tank2_225, tank2_248,
                                 tank2_270, tank2_293, tank2_315, tank2_338};

const char *tank1_current_sprite, *tank2_current_sprite;

unsigned char Score[2] = {0, 0};

unsigned char Tens[2] = {0, 0};

const unsigned char menu_opts[5] PROGMEM = {22, 23, 24, 25, 26};

const char *mazes[4] = {maze0, maze1, maze2, maze3};

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
  int top;     // Top edge of current bullet's current grid
  int bottom;
  int left;
  int right;
  int tside;
  int rside;
  int bside;
  int lside;
  int pitch;
};

struct bulletStruct p1_bullet, p2_bullet;

bool bounce = true;

struct tankStruct {
  float top;     // Top corner of the sprite in pixels
  float bottom;
  float left;
  float right;
  float vX;
  float vY;
  int angle;     // Which direction is the tank facing? 0 = up/north, 4 = right/east
  int x;         // Current 8x8 pixel tile horizontal grid position, starting from the left
  int y;         // Vertical tile position, starting from the top of the screen
  bool advance;  // Can the tank move forward in its current location and angle?
};

struct tankStruct p1_tank, p2_tank;

void hyperTanks(void);
void initIKD(void);
void initMaze(void);
void processTrig(void);
void processBullets(void);
void processTank1(void);
void processTank1Forward(void);
void processTank2(void);
void processTank2Forward(void);
void processScore(void);
void cuzeboxCOut(char str[]);
void cuzeboxHOut(int num);
void wallTankCollision(int tankN, int tankX, int tankY, int tankAngle);
void drawMainMenu(void);
void processMainMenu(void);
int wallCheck(int gridX, int gridY, int side);

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

typedef enum {MAIN_MENU, GAME} state;
state game_state = MAIN_MENU; ///< Tracks current state of the game.

int main() {
  while(1)
  {
    if(game_state == GAME)
    {
      // Basic prep work
      initIKD();

      // Reset scores
      Score[0] = 0;
      Score[1] = 0;

      // Load maze
      initMaze();
      // Main loop
      while (1) {
        // wait until the next frame
        WaitVsync(1);
        seed++;
        processTank1();
        processTank2();
        processBullets();
        processScore();
        if(!IsRunningInEmulator() && IsPowerSwitchPressed())
        {
          hyperTanks();
        }
      }
    }
    if(game_state == MAIN_MENU)
    {
      // Menu code here
      initIKD();
      if(!IsSongPlaying()){
        StartSong(commando);/*TriggerFx(SFX_FIRE,255,1);WaitVsync(60);TriggerFx(SFX_EXPLODE,255,1);WaitVsync(60);TriggerFx(SFX_ENGINE,255,1);WaitVsync(60);TriggerFx(SFX_BOING,255,1);WaitVsync(60);*/}
        drawMainMenu();
    }
    while(game_state == MAIN_MENU)
    {
      WaitVsync(1);
      processMainMenu();
    }
    StopSong();
    FadeOut(1,1);
    FadeIn(3,0);
    while(GetMasterVolume() > 16){
      SetMasterVolume(GetMasterVolume()-16);
      WaitVsync(1);
    }
    StopSong();
    SetMasterVolume(IKD_MASTER_VOL);
  }
}

void initIKD(void) {
  InitMusicPlayer(patches);
  SetMasterVolume(IKD_MASTER_VOL);
  SetSpritesTileTable(tileset);
  SetTileTable(tileset); // Tile set to use for ClearVram()
  SetFontTilesIndex(0);
  ClearVram();           // fill entire screen with first tile in the tileset
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
      p1_bullet.vX = p1_tank.vX;
      p1_bullet.vY = p1_tank.vY;
      p1_bullet.active = true;
      p1_bullet.pitch = 75;
      MapSprite2(1, bullet, 0); // map bullet
      MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
      TriggerFx(SFX_FIRE, 0xFF, true);
    }
  }
  if (tank1Held & BTN_UP) {
    if (p1_tank.advance == true) {
      processTank1Forward();
    }
  }
  if (tank1Held & BTN_B) {
    if (p1_tank.advance == true) {
      processTank1Forward();
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
      p2_bullet.vX = p2_tank.vX;
      p2_bullet.vY = p2_tank.vY;
      p2_bullet.active = true;
      p2_bullet.pitch = 75;
      MapSprite2(3, bullet, 0); // map bullet
      MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
      TriggerFx(SFX_FIRE, 0xFF, true);
    }
  }
  if (tank2Held & BTN_UP) {
    if (p2_tank.advance == true) {
      processTank2Forward();
    }
  }
  if (tank2Held & BTN_B) {
    if (p2_tank.advance == true) {
      processTank2Forward();
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
    p1_bullet.top = p1_bullet.gridY * 8;
    p1_bullet.bottom = p1_bullet.top + 8;
    p1_bullet.left = p1_bullet.gridX * 8;
    p1_bullet.right = p1_bullet.left + 8;
    p1_bullet.tside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,0);
    p1_bullet.rside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,1);
    p1_bullet.bside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,2);
    p1_bullet.lside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,3);
    MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
    if (p1_bullet.x >= p2_tank.left && p1_bullet.x <= p2_tank.right &&
        p1_bullet.y >= p2_tank.top && p1_bullet.y <= p2_tank.bottom) {
      p1_bullet.active = false;
      MapSprite2(1, blank, 0);
      TriggerFx(SFX_EXPLODE, 0xFF, true);
      Score[0]++;
      if (Score[0] > 9) {
          Tens[0]++;
          Score[0] = 0;
      }

      p2_tank.x = rand() % 27;
      p2_tank.y = rand() % 21;
      // This loop stops the tank respawning on a wall
      while (GetTile(p2_tank.x, p2_tank.y) == WALL_TILE) {
        p2_tank.x = rand() % 27;
        p2_tank.y = rand() % 21;
      }
      p2_tank.left = p2_tank.x * 8;
      p2_tank.top = p2_tank.y * 8;
      p2_tank.right = p2_tank.left + 8;
      p2_tank.bottom = p2_tank.top + 8;
      p2_tank.angle = rand() % 15;
      tank2_current_sprite = tank2_sprites[p2_tank.angle];
      processTrig();
      MapSprite2(2, tank2_current_sprite, 0);
      p2_bullet.active = false;
      p2_bullet.age = 0;
    }
    else if (p1_bullet.y <= 8 || p1_bullet.y >= 168) {
        if (bounce == true) {
          p1_bullet.vY = p1_bullet.vY * -1;
          TriggerNote(2, 38, p1_bullet.pitch, 127);
          p1_bullet.pitch++;
        }
        else {
          p1_bullet.active = false;
          MapSprite2(1, blank, 0);
        }
    }
    else if (p1_bullet.x >= (p1_bullet.right - 2)) {
      if (p1_bullet.rside == 1) {
        if (bounce == true) {
          p1_bullet.vX = p1_bullet.vX * -1;
          TriggerNote(2, 38, p1_bullet.pitch, 127);
          p1_bullet.pitch++;
        }
        else {
          p1_bullet.active = false;
          MapSprite2(1, blank, 0);
        }
      }
    }
    else if (p1_bullet.x <= (p1_bullet.left + 2)) {
      if (p1_bullet.lside == 1) {
        if (bounce == true) {
          p1_bullet.vX = p1_bullet.vX * -1;
          TriggerNote(2, 38, p1_bullet.pitch, 127);
          p1_bullet.pitch++;
        }
        else {
          p1_bullet.active = false;
          MapSprite2(1, blank, 0);
        }
      }
    }
    else if (p1_bullet.y <= (p1_bullet.top + 2)) {
      if (p1_bullet.tside == 1) {
        if (bounce == true) {
          p1_bullet.vY = p1_bullet.vY * -1;
          TriggerNote(2, 38, p1_bullet.pitch, 127);
          p1_bullet.pitch++;
        }
        else {
          p1_bullet.active = false;
          MapSprite2(1, blank, 0);
        }
      }
    }
    else if (p1_bullet.y >= (p1_bullet.bottom - 2)) {
      if (p1_bullet.bside == 1) {
        if (bounce == true) {
          p1_bullet.vY = p1_bullet.vY * -1;
          TriggerNote(2, 38, p1_bullet.pitch, 127);
          p1_bullet.pitch++;
        }
        else {
          p1_bullet.active = false;
          MapSprite2(1, blank, 0);
        }
      }
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
    p2_bullet.top = p2_bullet.gridY * 8;
    p2_bullet.bottom = p2_bullet.top + 8;
    p2_bullet.left = p2_bullet.gridX * 8;
    p2_bullet.right = p2_bullet.left + 8;
    p2_bullet.tside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,0);
    p2_bullet.rside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,1);
    p2_bullet.bside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,2);
    p2_bullet.lside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,3);
    MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
    if (p2_bullet.x >= p1_tank.left && p2_bullet.x <= p1_tank.right &&
        p2_bullet.y >= p1_tank.top && p2_bullet.y <= p1_tank.bottom) {
      p2_bullet.active = false;
      MapSprite2(3, blank, 0);
      TriggerFx(SFX_EXPLODE, 0xFF, true);
      Score[1]++;
      if (Score[1] > 9) {
          Tens[1]++;
          Score[1] = 0;
      }

      p1_tank.x = rand() % 27;
      p1_tank.y = rand() % 21;
      while (GetTile(p1_tank.x, p1_tank.y) == WALL_TILE) {
        p1_tank.x = rand() % 27;
        p1_tank.y = rand() % 21;
      }
      p1_tank.left = p1_tank.x * 8;
      p1_tank.top = p1_tank.y * 8;
      p1_tank.right = p1_tank.left + 8;
      p1_tank.bottom = p1_tank.top + 8;
      p1_tank.angle = rand() % 15;
      tank1_current_sprite = tank1_sprites[p1_tank.angle];
      processTrig();
      MapSprite2(0, tank1_current_sprite, 0);
      p1_bullet.active = false;
      p1_bullet.age = 0;
    }
    else if (p2_bullet.y <= 8 || p2_bullet.y >= 168) {
        if (bounce == true) {
          p2_bullet.vY = p2_bullet.vY * -1;
          TriggerNote(2, 38, p2_bullet.pitch, 127);
          p2_bullet.pitch++;
        }
        else {
          p2_bullet.active = false;
          MapSprite2(3, blank, 0);
        }
    }
    else if (p2_bullet.x >= (p2_bullet.right - 2)) {
      if (p2_bullet.rside == 1) {
        if (bounce == true) {
          p2_bullet.vX = p2_bullet.vX * -1;
          TriggerNote(2, 38, p2_bullet.pitch, 127);
          p2_bullet.pitch++;
        }
        else {
          p2_bullet.active = false;
          MapSprite2(3, blank, 0);
        }
      }
    }
    else if (p2_bullet.x <= (p2_bullet.left + 2)) {
      if (p2_bullet.lside == 1) {
        if (bounce == true) {
          p2_bullet.vX = p2_bullet.vX * -1;
          TriggerNote(2, 38, p2_bullet.pitch, 127);
          p2_bullet.pitch++;
        }
        else {
          p2_bullet.active = false;
          MapSprite2(3, blank, 0);
        }
      }
    }
    else if (p2_bullet.y <= (p2_bullet.top + 2)) {
      if (p2_bullet.tside == 1) {
        if (bounce == true) {
          p2_bullet.vY = p2_bullet.vY * -1;
          TriggerNote(2, 38, p2_bullet.pitch, 127);
          p2_bullet.pitch++;
        }
        else {
          p2_bullet.active = false;
          MapSprite2(3, blank, 0);
        }
      }
    }
    else if (p2_bullet.y >= (p2_bullet.bottom - 2)) {
      if (p2_bullet.bside == 1) {
        if (bounce == true) {
          p2_bullet.vY = p2_bullet.vY * -1;
          TriggerNote(2, 38, p2_bullet.pitch, 127);
          p2_bullet.pitch++;
        }
        else {
          p2_bullet.active = false;
          MapSprite2(3, blank, 0);
        }
      }
    }
  } else {
    p2_bullet.active = false;
    MapSprite2(3, blank, 0);
  }
}

void processTrig(void) {
  p1_tank.vX = sin(2 * M_PI * (angles[p1_tank.angle] / 360));
  p1_tank.vY = -cos(2 * M_PI * (angles[p1_tank.angle] / 360));
  p2_tank.vX = sin(2 * M_PI * (angles[p2_tank.angle] / 360));
  p2_tank.vY = -cos(2 * M_PI * (angles[p2_tank.angle] / 360));
}

void processScore(void) {
  DrawMap2(6, 22, (numbers[Score[0]]));
  DrawMap2(18, 22, (numbers2[Score[1]]));
  PrintHexInt(1,24,(Tens[0]));
  PrintHexInt(23,24,(Tens[1]));
}

void processTank1Forward(void) {
  p1_tank.left += p1_tank.vX / 2;
  p1_tank.top += p1_tank.vY / 2;
  p1_tank.right = p1_tank.left + 8;
  p1_tank.bottom = p1_tank.top + 8;
  MoveSprite(0, p1_tank.left, p1_tank.top, 1, 1);
  // Update tanks position on the tile grid
  p1_tank.x = p1_tank.left / 8;
  p1_tank.y = p1_tank.top / 8;
  wallTankCollision(0,p1_tank.x,p1_tank.y,p1_tank.angle);
  TriggerNote(2, 2, 20, 127);
}

void processTank2Forward(void) {
  p2_tank.left += p2_tank.vX / 2;
  p2_tank.top += p2_tank.vY / 2;
  p2_tank.right = p2_tank.left + 8;
  p2_tank.bottom = p2_tank.top + 8;
  MoveSprite(2, p2_tank.left, p2_tank.top, 1, 1);
  // Update tank 2's position on the tile grid
  p2_tank.x = p2_tank.left / 8;
  p2_tank.y = p2_tank.top / 8;
  wallTankCollision(1,p2_tank.x,p2_tank.y,p2_tank.angle);
  TriggerNote(1, 2, 20, 127);
}

void initMaze(void) {
  DrawMap2(0, 0, mazes[maze]);

  MapSprite2(0, tank1_090, 0); // setup tank 1 for drawing
  p1_tank.left = 8;            // set tank to the left
  p1_tank.top = 80;            // center tank vertically
  p1_tank.right = 13;
  p1_tank.bottom = 93;
  p1_tank.angle = 4;           // face right
  p1_tank.x = 1;
  p1_tank.y = 10;
  p1_tank.vX = 1;
  p1_tank.vY = 0;
  p1_tank.advance = true;
  p1_bullet.vX = 1;
  p1_bullet.vY = 0;
  p1_bullet.active = false;
  p1_bullet.age = 0;
  p1_bullet.gridX = 1;
  p1_bullet.gridY = 10;
  p1_bullet.top = 8;
  p1_bullet.bottom = 16;
  p1_bullet.left = 8;
  p1_bullet.right = 16;
  p1_bullet.tside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,0);
  p1_bullet.bside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,2);
  p1_bullet.lside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,3);
  p1_bullet.rside = wallCheck(p1_bullet.gridX, p1_bullet.gridY,1);

  MapSprite2(2, tank2_270, 0); // setup tank 2 for drawing
  p2_tank.left = 208;          // set tank to the right
  p2_tank.top = 80;            // center tank vertically
  p2_tank.right = 210;
  p2_tank.bottom = 93;
  p2_tank.angle = 12;          // face left
  p2_tank.x = 26;
  p2_tank.y = 10;
  p2_tank.vX = -1;
  p2_tank.vY = 0;
  p2_tank.advance = true;
  p2_bullet.vX = -1;
  p2_bullet.vY = 0;
  p2_bullet.active = false;
  p2_bullet.age = 0;
  p2_bullet.gridX = 25;
  p2_bullet.gridY = 10;
  p2_bullet.top = p2_bullet.gridY * 8;
  p2_bullet.bottom = p2_bullet.top + 8;
  p2_bullet.left = p2_bullet.gridX * 8;
  p2_bullet.right = p2_bullet.left + 8;
  p2_bullet.tside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,0);
  p2_bullet.rside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,1);
  p2_bullet.bside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,2);
  p2_bullet.lside = wallCheck(p2_bullet.gridX, p2_bullet.gridY,3);
}

void drawMainMenu()
{
  ClearVram();
  Print(2,0,PSTR("A TRIBUTE TO ATARI'S COMBAT"));
  Print(13,2,PSTR("V1.1"));
  DrawMap2(8,4,title_map);
  Print(7,18,PSTR("BY DAN MACDONALD"));
  Print(10,22,PSTR("NO MAZE"));
  Print(10,23,PSTR("MAZE #1"));
  Print(10,24,PSTR("MAZE #2"));
  Print(10,25,PSTR("MAZE #3"));

  if (bounce == true) {
  Print(10,26,PSTR("BOUNCE ON"));
  }
  else {
    Print(10,26,PSTR("BOUNCE OFF"));
  }
  SetTile(7,pgm_read_byte(&menu_opts[maze]),101);
}

void processMainMenu()
{
  tank1Held = ReadJoypad(0); // read in our player one joypad input
  //pressing something and it isn't the same buttons as last frame so it's a new button press, not a hold
  if (tank1Held!=tank1Prev) {
    if (tank1Held & BTN_DOWN) {
      maze++;
      TriggerNote(1, 38, 75, 127);
      if (maze > 4) {
        maze = 0;
      }
      drawMainMenu();
    }
    if (tank1Held & BTN_UP) {
      maze--;
      TriggerNote(1, 38, 75, 127);
      if (maze < 0) {
        maze = 4;
      }
      drawMainMenu();
    }
    if (tank1Held & BTN_LEFT) {
      if (maze == 4) {
        bounce = !bounce;
        TriggerNote(1, 38, 75, 127);
      }
      drawMainMenu();
    }
    if (tank1Held & BTN_RIGHT) {
      if (maze == 4) {
        bounce = !bounce;
        TriggerNote(1, 38, 75, 127);
      }
      drawMainMenu();
    }
    if (tank1Held & BTN_START) {
      if (maze != 4) {
        game_state = GAME;
      }
    }
    tank1Prev = tank1Held;
  }
}

// To implement bouncy bullets, we need a quick and easy way to check if a
// tile next to the current one contains a wall tile, hence wallCheck():

int wallCheck(int gridX, int gridY, int side) {
  if (side == 0) {    // Check the grid location above for a wall tile
    if (gridY <= 0) {
      return 0;
    }
    else if (GetTile(gridX, (gridY - 1)) == WALL_TILE) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else if (side == 1) {   // Check grid to the right for a wall tile
    if (gridX >= 28) {
      return 0;
    }
    else if (GetTile((gridX + 1), gridY) == WALL_TILE) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else if (side == 2) {   // Check grid location below for a wall tile
    if (gridY >= 22) {
      return 0;
    }
    else if (GetTile(gridX, (gridY + 1)) == WALL_TILE) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else if (side == 3) {   // Check grid to the left for a wall tile
    if (gridX <= 0) {
      return 0;
    }
    else if (GetTile((gridX - 1), gridY) == WALL_TILE) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }
}

void wallTankCollision(int tankN, int tankX, int tankY, int tankAngle) {

  // Calculate the next position based on the tank's angle
  if (tankAngle == 0) {
    nextX = tankX;
    nextY = tankY;
  } else if (tankAngle == 1 || tankAngle == 2 || tankAngle == 3) {
    nextX = tankX + 1;
    nextY = tankY - 1;
  } else if (tankAngle == 4) {
    nextX = tankX + 1;
    nextY = tankY;
  } else if (tankAngle == 5 || tankAngle == 6 || tankAngle == 7) {
    nextX = tankX + 1;
    nextY = tankY + 1;
  } else if (tankAngle == 8) {
    nextX = tankX;
    nextY = tankY + 1;
  } else if (tankAngle == 9 || tankAngle == 10 || tankAngle == 11) {
    nextX = tankX - 1;
    nextY = tankY + 1;
  } else if (tankAngle == 12) {
    nextX = tankX;
    nextY = tankY;
  } else if (tankAngle == 13 || tankAngle == 14 || tankAngle == 15) {
    nextX = tankX - 1;
    nextY = tankY - 1;
  }

  // Check if the next tank position is occupied by a wall
  if (GetTile(nextX, nextY) == WALL_TILE) {
    if (tankN == 0) {
      p1_tank.advance = false;
    }
    else if (tankN == 1) {
      p2_tank.advance = false;
    }
  }
  if (GetTile(nextX, nextY) == 0x0) {
    if (tankN == 0) {
      p1_tank.advance = true;
    }
    else if (tankN == 1) {
      p2_tank.advance = true;
    }
  }
}

void hyperTanks(void) {
  //Hyper tank 2
  p2_tank.x = rand() % 27;
  p2_tank.y = rand() % 21;
  while (GetTile(p2_tank.x, p2_tank.y) == WALL_TILE) {
    p2_tank.x = rand() % 27;
    p2_tank.y = rand() % 21;
  }
  p2_tank.left = p2_tank.x * 8;
  p2_tank.top = p2_tank.y * 8;
  p2_tank.right = p2_tank.left + 8;
  p2_tank.bottom = p2_tank.top + 8;
  p2_tank.angle = rand() % 15;
  tank2_current_sprite = tank2_sprites[p2_tank.angle];
  processTrig();
  MapSprite2(2, tank2_current_sprite, 0);
  p2_bullet.active = false;
  p2_bullet.age = 0;

  //Hyper tank 1
  p1_tank.x = rand() % 27;
  p1_tank.y = rand() % 21;
  while (GetTile(p1_tank.x, p1_tank.y) == WALL_TILE) {
    p1_tank.x = rand() % 27;
    p1_tank.y = rand() % 21;
  }
  p1_tank.left = p1_tank.x * 8;
  p1_tank.top = p1_tank.y * 8;
  p1_tank.right = p1_tank.left + 8;
  p1_tank.bottom = p1_tank.top + 8;
  p1_tank.angle = rand() % 15;
  tank1_current_sprite = tank1_sprites[p1_tank.angle];
  processTrig();
  MapSprite2(0, tank1_current_sprite, 0);
  p1_bullet.active = false;
  p1_bullet.age = 0;
}
