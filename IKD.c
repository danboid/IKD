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

uint_least8_t     player_x = 80, ///< player x position on screen. 0 is far left
		          player_y = 40; ///< player y position on screen. 0 is top
		          
int btnPrev = 0;     // Previous button
int btnHeld = 0;     // buttons that are held right now
int btnPressed = 0;  // buttons that were pressed this frame
int btnReleased = 0; // buttons that were released this frame 

void drawIntro(void);
void processIntro(void);
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
		}
}

/**
 * \brief Performs some basic initialization functions.
 */
static void initialSetup()
{
	SetSpritesTileTable(tileset); //sets the tiles to be used for our various sprites
	ClearVram(); //fill entire screen with first tile in the tileset (blank the screen)
}

/**
 * \brief Setup for custom intro
 */
void initIntro(void)
{
	MapSprite2(0, tank1_0, 0); //setup tank for drawing
	player_x = 100; //set tank to the middle
	player_y = 100; //center tank vertically
}

/**
 * \brief Draws the custom intro
 */
void drawIntro(void)
{
	ClearVram(); //wipe screen each frame
	MoveSprite(0, player_x, player_y, 1, 1); //position tank sprite
}

/**
 * \processes controller input
 */
void processIntro(void)
{    
    btnHeld = ReadJoypad(0); //read in our player one joypad input
    //btnPressed = btnHeld & (btnHeld ^ btnPrev);
    //btnReleased = btnPrev & (btnHeld ^ btnPrev);

    if(btnHeld & BTN_RIGHT){
        MapSprite2(0,tank1_90,0);
        player_x++;
    }
    if(btnHeld & BTN_LEFT){
        MapSprite2(0,tank1_90,SPRITE_FLIP_X);
        player_x--;
    }
    if(btnHeld & BTN_UP){
        MapSprite2(0,tank1_0,0);
        player_y--;
    }
    if(btnHeld & BTN_DOWN){
        MapSprite2(0,tank1_0,SPRITE_FLIP_Y);
        player_y++;
    }
    btnPrev = btnHeld;
}

