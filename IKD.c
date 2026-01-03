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

// --- GLOBALS & STATE ---
typedef enum {MAIN_MENU, GAME} state;
state game_state = MAIN_MENU;
bool isSinglePlayer = false;
bool bounce = true;
int seed, maze, nextX, nextY = 0;

int aiStuckTimer = 0;
float aiLastX = 0;
float aiLastY = 0;
bool aiRapidTurn = false;
int aiDecisionTimer = 0; // Tracks frames to wait after a collision

int tank1Prev, tank1Held, tank1Pressed;
int tank2Prev, tank2Held, tank2Pressed;

unsigned char Score[2] = {0, 0};
unsigned char Tens[2] = {0, 0};

#define WALL_TILE 0x64
// NOTE: IKD_MASTER_VOL is already defined in patches.inc

// --- SPRITE & MAP DATA (Now Global so all functions see them) ---
const char *tank1_sprites[16] = {tank1_000, tank1_023, tank1_045, tank1_068, tank1_090, tank1_113, tank1_135, tank1_158, tank1_180, tank1_203, tank1_225, tank1_248, tank1_270, tank1_293, tank1_315, tank1_338};
const char *tank2_sprites[16] = {tank2_000, tank2_023, tank2_045, tank2_068, tank2_090, tank2_113, tank2_135, tank2_158, tank2_180, tank2_203, tank2_225, tank2_248, tank2_270, tank2_293, tank2_315, tank2_338};
const char *numbers[10] = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9};
const char *numbers2[10] = {sc0, sc1, sc2, sc3, sc4, sc5, sc6, sc7, sc8, sc9};
const char *mazes[4] = {maze0, maze1, maze2, maze3};

// Current sprite trackers
const char *tank1_current_sprite;
const char *tank2_current_sprite;

// --- LOOKUP TABLES ---
float angles[] = {0, 23, 45, 68, 90, 113, 135, 158, 180, 203, 225, 248, 270, 293, 315, 338};

const int8_t angle_lut[9][9] PROGMEM = {
    {14, 14, 15, 15,  0,  1,  1,  2,  2}, // dy = -4 (P1 is far above)
    {14, 14, 14, 15,  0,  1,  2,  2,  2},
    {13, 13, 14, 15,  0,  1,  2,  3,  3},
    {13, 13, 13, 14,  0,  2,  3,  3,  3},
    {12, 12, 12, 12,  0,  4,  4,  4,  4}, // dy = 0  (P1 is on same Y)
    {11, 11, 11, 10,  8,  6,  5,  5,  5},
    {11, 11, 10,  9,  8,  7,  6,  5,  5},
    {10, 10, 10,  9,  8,  7,  6,  6,  6},
    {10, 10,  9,  9,  8,  7,  7,  6,  6}  // dy = 4  (P1 is far below)
};

const unsigned char menu_y_pos[] PROGMEM = {20, 20, 21, 21, 22, 22, 23, 23, 25};

// ... (Rest of your Structs and Prototypes here)

// --- STRUCTURES ---
struct bulletStruct {
    float x, y, vX, vY;
    bool active;
    int age, gridX, gridY, pitch;
    int top, bottom, left, right;
    int tside, rside, bside, lside;
};

struct tankStruct {
    float top, bottom, left, right, vX, vY;
    int angle, x, y;
    bool advance;
};

struct bulletStruct p1_bullet, p2_bullet;
struct tankStruct p1_tank, p2_tank;

// --- PROTOTYPES ---
void initIKD(void);
void initMaze(void);
void processTrig(void);
void processBullets(void);
void processTank1(void);
void processTank2(void);
void processAI(void);
void processScore(void);
void drawMainMenu(void);
void processMainMenu(void);
bool hasLineOfSight(void);
int wallCheck(int gridX, int gridY, int side);
void wallTankCollision(int tankN);

// --- MAIN LOOP ---
int main() {
    while(1) {
        if(game_state == MAIN_MENU) {
            initIKD();
            if(!IsSongPlaying()) StartSong(commando);
            drawMainMenu();
            WaitVsync(1);   // Ensure the first frame renders properly
            while(game_state == MAIN_MENU) {
                WaitVsync(1);
                processMainMenu();
            }
            StopSong();
        }

        if(game_state == GAME) {
            initIKD();
            Score[0] = 0; Score[1] = 0;
            Tens[0] = 0;  Tens[1] = 0;
            initMaze(); // Loads the map and calls hyperTanks()

            while (game_state == GAME) {
                WaitVsync(1);
                seed++;

                processTank1();

                if (isSinglePlayer) {
                    processAI();
                } else {
                    processTank2();
                }

                processBullets();
                processScore();
            }
        }
    }
}

// --- CORE FUNCTIONS ---

void initIKD(void) {
    InitMusicPlayer(patches);
    SetMasterVolume(IKD_MASTER_VOL);
    SetSpritesTileTable(tileset);
    SetTileTable(tileset);
    ClearVram();
}

void processAI(void) {
    // 1. POSITION TRACKING (Stuck Detection)
    if (abs(p2_tank.left - aiLastX) < 0.2 && abs(p2_tank.top - aiLastY) < 0.2) {
        aiStuckTimer++;
    } else {
        aiStuckTimer = 0;
    }
    aiLastX = p2_tank.left;
    aiLastY = p2_tank.top;

    // 2. DIRECTIONAL LOGIC (Targeting P1)
    int dx = p1_tank.x - p2_tank.x;
    int dy = p1_tank.y - p2_tank.y;

    // Clamp to -4 to 4 range, then shift to 0 to 8 for the table index
    int lutX = (dx < -4 ? -4 : (dx > 4 ? 4 : dx)) + 4;
    int lutY = (dy < -4 ? -4 : (dy > 4 ? 4 : dy)) + 4;

    // Read the target angle from Flash memory (PROGMEM)
    int targetAngle = pgm_read_byte(&(angle_lut[lutY][lutX]));

    // Smooth rotation toward target
    if (p2_tank.angle != targetAngle && (seed % 4 == 0)) {
        int diff = (targetAngle - p2_tank.angle + 16) % 16;
        if (diff < 8) p2_tank.angle = (p2_tank.angle + 1) % 16;
        else p2_tank.angle = (p2_tank.angle + 15) % 16;
        processTrig(); // Recalculate vX and vY
    }

    // 3. FORWARD MOVEMENT
    if (aiStuckTimer < 40) {
        p2_tank.left += p2_tank.vX / 2;
        p2_tank.top  += p2_tank.vY / 2;
    } else {
        // PANIC MODE: Break the corner lock
        if (seed % 8 == 0) {
            p2_tank.angle = (p2_tank.angle + 1) % 16;
            processTrig();
        }
        p2_tank.left += p2_tank.vX / 4;
        p2_tank.top  += p2_tank.vY / 4;
    }

    // 4. COLLISION RESOLUTION
    wallTankCollision(1);

    // 5. SHOOTING & GRAPHICS
    if (!p2_bullet.active && hasLineOfSight()) {
        if (rand() % 30 == 1) {
            p2_bullet.active = true;
            p2_bullet.x = p2_tank.left;
            p2_bullet.y = p2_tank.top;
            p2_bullet.vX = p2_tank.vX;
            p2_bullet.vY = p2_tank.vY;
            p2_bullet.pitch = 60;
            MapSprite2(3, bullet, 0);
            TriggerFx(SFX_FIRE, 0xFF, true);
        }
    }

    MapSprite2(2, tank2_sprites[p2_tank.angle], 0);
    MoveSprite(2, p2_tank.left, p2_tank.top, 1, 1);
}

bool hasLineOfSight(void) {
    int x0 = p2_tank.x; int y0 = p2_tank.y;
    int x1 = p1_tank.x; int y1 = p1_tank.y;
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;
    while(1){
        if(GetTile(x0, y0) == WALL_TILE) return false;
        if(x0==x1 && y0==y1) return true;
        e2 = 2*err;
        if(e2 >= dy){ err += dy; x0 += sx; }
        if(e2 <= dx){ err += dx; y0 += sy; }
    }
}

void processScore(void) {
    DrawMap2(6, 22, (numbers[Score[0]]));
    DrawMap2(18, 22, (numbers2[Score[1]]));
    PrintHexInt(1, 24, Tens[0]);
    PrintHexInt(23, 24, Tens[1]);

    if (Tens[0] > 0 || Tens[1] > 0) {
        for(int i=0; i<4; i++){
            TriggerNote(2, 38, 50-(i*12), 127);
            WaitVsync(12);
        }
        ClearVram();
        if (Tens[0] > 0) Print(9, 12, PSTR("PLAYER 1 WINS"));
        else Print(9, 12, PSTR("PLAYER 2 WINS"));
        WaitVsync(180);
        game_state = MAIN_MENU;
    }
}

void drawMainMenu() {
    ClearVram();
    Print(2,0,PSTR("A TRIBUTE TO ATARI'S COMBAT"));
    Print(13,2,PSTR("V1.4"));
    DrawMap2(8,4,title_map);

    // Draw Maze Labels
    Print(10,20,PSTR("MAZE #0"));
    Print(10,21,PSTR("MAZE #1"));
    Print(10,22,PSTR("MAZE #2"));
    Print(10,23,PSTR("MAZE #3"));

    // Draw the 1P/2P indicator for the CURRENTLY selected maze
    // We only show "1P/2P" next to the maze that is being pointed at
    if (maze < 8) {
        if (maze % 2 == 0) Print(18, 20 + (maze/2), PSTR("2P"));
        else Print(18, 20 + (maze/2), PSTR("1P"));
    }

    // Bounce Option
    if (bounce) Print(10, 25, PSTR("BOUNCE ON"));
    else Print(10, 25, PSTR("BOUNCE OFF"));

    // Place the cursor (Tile 101) based on the menu_y_pos lookup table
    SetTile(8, pgm_read_byte(&menu_y_pos[maze]), 101);
}

void processMainMenu() {
    tank1Held = ReadJoypad(0);
    if (tank1Held != tank1Prev) {
        if (tank1Held & BTN_DOWN) { maze = (maze + 2) % 10; if(maze==9) maze=0; drawMainMenu(); }
        if (tank1Held & BTN_UP) { if(maze==0) maze=8; else maze-=2; drawMainMenu(); }
        if ((tank1Held & BTN_LEFT) || (tank1Held & BTN_RIGHT)) {
            if (maze < 8) { if (maze % 2 == 0) maze++; else maze--; }
            else bounce = !bounce;
            drawMainMenu();
        }
        if (tank1Held & BTN_START) {
            if (maze != 8) {
                isSinglePlayer = (maze % 2 != 0);
                maze = maze / 2;
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

void wallTankCollision(int tankN) {
    struct tankStruct *t = (tankN == 0) ? &p1_tank : &p2_tank;

    if (t->left < 8)   t->left = 8;
    if (t->left > 224) t->left = 224;
    if (t->top < 8)    t->top = 8;
    if (t->top > 168)  t->top = 168;

    int margin = 2;
    bool collisionX = false;

    if (GetTile((t->left + margin) / 8, (t->top + margin) / 8) == WALL_TILE ||
        GetTile((t->left + 7 - margin) / 8, (t->top + margin) / 8) == WALL_TILE ||
        GetTile((t->left + margin) / 8, (t->top + 7 - margin) / 8) == WALL_TILE ||
        GetTile((t->left + 7 - margin) / 8, (t->top + 7 - margin) / 8) == WALL_TILE) {
        collisionX = true;
    }

    if (collisionX) {
        t->left -= (t->vX * 1.1f);
        t->top  -= (t->vY * 1.1f);
        t->advance = false;
    } else {
        t->advance = true;
    }

    t->x = t->left / 8;
    t->y = t->top / 8;
    t->bottom = t->top + 8;
    t->right = t->left + 8;
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
  tank2_current_sprite = tank1_sprites[p2_tank.angle];
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

void processTrig(void) {
    // 0 degrees is UP: vX = 0, vY = -1
    // 90 degrees is RIGHT: vX = 1, vY = 0
    float rad = (angles[p1_tank.angle]) * (M_PI / 180.0f);
    p1_tank.vX = sin(rad);
    p1_tank.vY = -cos(rad);

    rad = (angles[p2_tank.angle]) * (M_PI / 180.0f);
    p2_tank.vX = sin(rad);
    p2_tank.vY = -cos(rad);
}

void processBullets(void) {
    // --- Player 1 Bullet vs Player 2 Tank ---
    if (p1_bullet.active) {
        float oldX = p1_bullet.x;
        float oldY = p1_bullet.y;

        p1_bullet.x += p1_bullet.vX * 3;
        p1_bullet.y += p1_bullet.vY * 3;

        // 1. TANK COLLISION (Check if hitting Player 2)
        if (p1_bullet.x >= p2_tank.left && p1_bullet.x <= (p2_tank.left + 8) &&
            p1_bullet.y >= p2_tank.top  && p1_bullet.y <= (p2_tank.top + 8)) {

            p1_bullet.active = false;
            TriggerFx(SFX_EXPLODE, 0xFF, true);
            Score[0]++;
            if (Score[0] > 9) { Tens[0]++; Score[0] = 0; }
            hyperTanks(); // Respawn
        }
        // 2. WALL COLLISION (Only check if we haven't already hit a tank)
        else if (GetTile(p1_bullet.x / 8, p1_bullet.y / 8) == WALL_TILE) {
            if (!bounce) {
                p1_bullet.active = false;
            } else {
                if (GetTile(oldX / 8, p1_bullet.y / 8) == WALL_TILE) {
                    p1_bullet.vY = -p1_bullet.vY;
                    p1_bullet.y = oldY;
                } else {
                    p1_bullet.vX = -p1_bullet.vX;
                    p1_bullet.x = oldX;
                }
                TriggerNote(2, 38, p1_bullet.pitch, 127);
                p1_bullet.pitch++;
                p1_bullet.age += 33;
                if (p1_bullet.age > 100) p1_bullet.active = false;
            }
        }

        if (p1_bullet.active) MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
        else MoveSprite(1, 0, 240, 1, 1);
    }

    // --- Player 2 Bullet vs Player 1 Tank ---
    if (p2_bullet.active) {
        float oldX2 = p2_bullet.x;
        float oldY2 = p2_bullet.y;

        p2_bullet.x += p2_bullet.vX * 3;
        p2_bullet.y += p2_bullet.vY * 3;

        // 1. TANK COLLISION (Check if hitting Player 1)
        if (p2_bullet.x >= p1_tank.left && p2_bullet.x <= (p1_tank.left + 8) &&
            p2_bullet.y >= p1_tank.top  && p2_bullet.y <= (p1_tank.top + 8)) {

            p2_bullet.active = false;
            TriggerFx(SFX_EXPLODE, 0xFF, true);
            Score[1]++;
            if (Score[1] > 9) { Tens[1]++; Score[1] = 0; }
            hyperTanks(); // Respawn
        }
        // 2. WALL COLLISION
        else if (GetTile(p2_bullet.x / 8, p2_bullet.y / 8) == WALL_TILE) {
            if (!bounce) {
                p2_bullet.active = false;
            } else {
                if (GetTile(oldX2 / 8, p2_bullet.y / 8) == WALL_TILE) {
                    p2_bullet.vY = -p2_bullet.vY;
                    p2_bullet.y = oldY2;
                } else {
                    p2_bullet.vX = -p2_bullet.vX;
                    p2_bullet.x = oldX2;
                }
                TriggerNote(2, 38, p2_bullet.pitch, 127);
                p2_bullet.pitch++;
                p2_bullet.age += 33;
                if (p2_bullet.age > 100) p2_bullet.active = false;
            }
        }

        if (p2_bullet.active) MoveSprite(3, p2_bullet.x, p2_bullet.y, 1, 1);
        else MoveSprite(3, 0, 240, 1, 1);
    }
}

void initMaze(void) {
    ClearVram();
    // mazes[maze] uses the global array of map pointers
    DrawMap2(0, 0, mazes[maze]);
    hyperTanks(); // Reset tank positions and sprites
}

void processTank1(void) {
    // 1. Read Input from Joypad 1 (Index 0)
    tank1Held = ReadJoypad(0);
    tank1Pressed = tank1Held & (tank1Held ^ tank1Prev);

    // 2. Handle Rotation (Left/Right)
    if (tank1Pressed & BTN_RIGHT) {
        p1_tank.angle = (p1_tank.angle + 1) % 16;
        processTrig(); // Updates vX and vY based on new angle
    }
    if (tank1Pressed & BTN_LEFT) {
        p1_tank.angle = (p1_tank.angle + 15) % 16;
        processTrig();
    }

    // 3. Handle Firing (A or B button)
    if (tank1Pressed & BTN_A && !p1_bullet.active) {
        p1_bullet.active = true;
        p1_bullet.age = 0;
        p1_bullet.x = p1_tank.left;
        p1_bullet.y = p1_tank.top;
        p1_bullet.vX = p1_tank.vX;
        p1_bullet.vY = p1_tank.vY;
        p1_bullet.pitch = 75;
        MapSprite2(1, bullet, 0); // P1 bullet is usually sprite 1
        TriggerFx(SFX_FIRE, 0xFF, true);
    }

    // 4. Handle Movement (Up button)
    // p1_tank.advance is a bool set by the collision system
    if ((tank1Held & BTN_UP) && p1_tank.advance) {
        p1_tank.left += p1_tank.vX / 2;
        p1_tank.top += p1_tank.vY / 2;
        TriggerNote(2, 2, 20, 127);
    }

    if ((tank1Held & BTN_B) && p1_tank.advance) {
        p1_tank.left += p1_tank.vX / 2;
        p1_tank.top += p1_tank.vY / 2;
        TriggerNote(2, 2, 20, 127);
    }

    // 5. Update Collision Grid and Visuals
    p1_tank.x = p1_tank.left / 8; // Convert pixel pos to tile grid
    p1_tank.y = p1_tank.top / 8;

    // Check if the move is valid or hitting a wall
    wallTankCollision(0);

    // Map the correct rotation tile to Sprite 0
    MapSprite2(0, tank1_sprites[p1_tank.angle], 0);
    MoveSprite(0, p1_tank.left, p1_tank.top, 1, 1);

    // Store state for the next frame's "Pressed" check
    tank1Prev = tank1Held;
}

void processTank2(void) {
    tank2Held = ReadJoypad(1); // Joypad index 1 is Player 2
    tank2Pressed = tank2Held & (tank2Held ^ tank2Prev);

    if (tank2Pressed & BTN_RIGHT) {
        p2_tank.angle = (p2_tank.angle + 1) % 16;
        processTrig();
    }
    if (tank2Pressed & BTN_LEFT) {
        p2_tank.angle = (p2_tank.angle + 15) % 16;
        processTrig();
    }
    if (tank2Pressed & BTN_A && !p2_bullet.active) {
    p2_bullet.active = true;
    p2_bullet.age = 0;
    p2_bullet.x = p2_tank.left;
    p2_bullet.y = p2_tank.top;
    p2_bullet.vX = p2_tank.vX;
    p2_bullet.vY = p2_tank.vY;
    p2_bullet.pitch = 75; // Initialize pitch like P1 does
    MapSprite2(3, bullet, 0);
    TriggerFx(SFX_FIRE, 0xFF, true);
    }
    if ((tank2Held & BTN_UP) && p2_tank.advance) {
        p2_tank.left += p2_tank.vX / 2;
        p2_tank.top += p2_tank.vY / 2;
        TriggerNote(1, 2, 20, 127);
    }
    if ((tank2Held & BTN_B) && p2_tank.advance) {
        p2_tank.left += p2_tank.vX / 2;
        p2_tank.top += p2_tank.vY / 2;
        TriggerNote(1, 2, 20, 127);
    }

    p2_tank.x = p2_tank.left / 8;
    p2_tank.y = p2_tank.top / 8;
    wallTankCollision(1);

    MapSprite2(2, tank2_sprites[p2_tank.angle], 0);
    MoveSprite(2, p2_tank.left, p2_tank.top, 1, 1);

    tank2Prev = tank2Held;
}
