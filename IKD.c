/** \file IKD.c
*   \brief The main source file for IKD, a Uzebox remake of the tank games in Atari's Combat
*   \author Dan MacDonald
*   \date 2020
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include "data/tileset.inc"

uint_least8_t   tank1_current_frame = 0, tank1_max_frames = 16;
		          
int btnPrev = 0;     // Previous button
int btnHeld = 0;     // buttons that are held right now
int btnPressed = 0;  // buttons that were pressed this frame
int btnReleased = 0; // buttons that were released this frame

const char * tank1_sprites[16] = {tank1_000, tank1_023, tank1_045, tank1_068, tank1_090, tank1_113, tank1_135, tank1_158, tank1_180, tank1_203, tank1_225, tank1_248, tank1_270, tank1_293, tank1_315, tank1_338};
const char * tank1_current_sprite; ///< used as an index in tank1_sprites array to display the correct sprite

struct bulletStruct{
	unsigned char x;
	unsigned char y;
    bool active;
};

struct bulletStruct p1_bullet;

struct tankStruct{
	unsigned char x;
	unsigned char y;
};

struct tankStruct p1_tank;

void drawIntro(void);
void processIntro(void);
void processBullet(void);
void initIntro(void);
static void initialSetup(void);

/**
 * \brief The main game loop. This just cycles endlessly.
 */
int main()
{
		//some basic prep work
        initialSetup();
        initIntro();
		//Main loop
		while(1)
		{
			//wait until the next frame
			WaitVsync(1);
			drawIntro();
			processIntro();
            processBullet();
		}
}

/**
 * \brief Performs some basic initialization functions.
 */
static void initialSetup()
{
	SetSpritesTileTable(tileset); //sets the tiles to be used for our various sprites
    SetTileTable(tileset); //Tile set to use for ClearVram()
	ClearVram(); //fill entire screen with first tile in the tileset (blank the screen)
}

/**
 * \brief Setup for custom intro
 */
void initIntro(void)
{
      MapSprite2(0, tank1_000, 0); //setup tank for drawing
      
	p1_tank.x = 100; //set tank to the middle
	p1_tank.y = 100; //center tank vertically
	
	p1_bullet.active = false;
}

/**
 * \brief Draws the custom intro
 */
void drawIntro(void)
{
	ClearVram(); //wipe screen each frame
	MoveSprite(0, p1_tank.x, p1_tank.y, 1, 1); //position tank sprite
}

/**
 * \processes controller input
 */
void processIntro(void)
{    
    btnHeld = ReadJoypad(0); //read in our player one joypad input
    btnPressed = btnHeld & (btnHeld ^ btnPrev);
    //btnReleased = btnPrev & (btnHeld ^ btnPrev);

    if(btnPressed & BTN_RIGHT){
        tank1_current_frame++; //move forward to next animation frame
		if(tank1_current_frame == tank1_max_frames)
		{
			tank1_current_frame=0;
		}
        tank1_current_sprite = tank1_sprites[tank1_current_frame]; //change our tracking variable to the correct sprite based on new frame
		MapSprite2(0, tank1_current_sprite, 0); //actually reassign the sprites in memory to the correct images
    }
    if(btnPressed & BTN_LEFT){
		if(tank1_current_frame == 0)
		{
			tank1_current_frame=15;
		}
		else
        {
            tank1_current_frame--; //move back to previous animation frame
        }
        tank1_current_sprite = tank1_sprites[tank1_current_frame]; //change our tracking variable to the correct sprite based on new frame
		MapSprite2(0, tank1_current_sprite, 0); //actually reassign the sprites in memory to the correct images
    }
    if(btnPressed & BTN_A){
        if(p1_bullet.active == false ){
            p1_bullet.x = p1_tank.x;
            p1_bullet.y = p1_tank.y;
            p1_bullet.active = true;
            MapSprite2(1, bullet, 0); //map bullet
            MoveSprite(1, p1_bullet.x, p1_bullet.y, 1, 1);
        }
    }
    btnPrev = btnHeld;
}

void processBullet(void){
    if(p1_bullet.y > 0 && p1_bullet.active == true ){
        p1_bullet.y -= 1;
        MoveSprite(1, 100, p1_bullet.y, 1, 1);
	}else {
		p1_bullet.active = false;
	}
}
