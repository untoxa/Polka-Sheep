#include "Banks/SetAutoBank.h"

#include <gbdk/platform.h>

#include "main.h"

#include "ZGBMain.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "Trig.h"
#include "Print.h"
#include "Math.h"
#include "Vector.h"
#include "Scroll.h"
#include "Sound.h"
#include "Palette.h"

extern const INT8 gravity;

UINT8 anim_idle[]   = {2, 0, 1};
UINT8 anim_flying_r[] = {8, 2, 3, 4, 5, 6, 7, 8, 9};
UINT8 anim_flying_l[] = {8, 9, 8, 7, 6, 5, 4, 3, 2};
UINT8 anim_dancing[] = {2, 10, 11};

Sprite* crossHair;
Sprite* player_sprite;
UINT8 sheepAngStart = 64;
UINT8 sheepAngOffset = 0;
UINT8 sheepAngMax = 128;
INT8 sheepIncr = 1;

extern UINT8 lifes_y[];

typedef enum {
	AIMING,
	FLYING, 
	DANCING,

	NONE
} SheepState;
SheepState sheep_state;
fixed accum_x;
fixed accum_y;
INT16 speed_x;
INT16 speed_y;

Sprite* player_parent = 0;
Sprite* last_platform = 0;

extern const UINT8 max_energy;
UINT8 current_energy;

extern const INT8 inmunity_time;
INT16 inmunity = 0;


const UINT8 HEART_TILE = 1;
const UINT8 EMPTY_HEART_TILE = 2;
void RefreshLife(void) BANKED {
#ifdef INITENDO
	UINT8 i;
	for(i = 0; i != current_energy; ++i) {
		set_win_tiles(10 + i, 1, 1, 1, &HEART_TILE);
	}
	for(; i != max_energy; ++i) {
		set_win_tiles(10 + i, 1, 1, 1, &EMPTY_HEART_TILE);
	}
#endif
}

void ChangeState(SheepState next);
void START(void) {
	THIS->lim_y = 255;

	sheep_state = NONE;
	player_parent = 0;
	last_platform = 0;
	speed_x = 0;
	speed_y = 0;
	accum_x.w = 0;
	accum_y.w = 0;
	ChangeState(AIMING);

	sheepAngStart = 192;
	sheepAngOffset = 64;
	sheepIncr = 2;

	inmunity = 0;

	player_sprite = THIS;
}

void ChangeState(SheepState next) {
	switch (sheep_state) {
		case AIMING:
			SpriteManagerRemoveSprite(crossHair);
			break;
	}

	sheep_state = next;

	switch (next) {
		case AIMING:
			crossHair = SpriteManagerAdd(SpriteCrosshair, THIS->x, THIS->y);
			SetSpriteAnim(THIS, anim_idle, 5);
			if(player_parent == 0) {
				sheepAngMax = 128;
				last_platform = 0;
			} else {
				sheepAngMax = 255;
			}
			break;

		case FLYING:
			PlayFx(CHANNEL_1, 10, 0x37, 0x80, 0xf4, 0xb7, 0x84);
			SetSpriteAnim(THIS, speed_x > 0 ? anim_flying_r : anim_flying_l, 25);
			last_platform = player_parent;
			player_parent = 0;
			break;

		case DANCING:
			SetSpriteAnim(THIS, anim_dancing, 4);
			break;
	}
}

void Hit(void) {
	if(inmunity == 0) {
			PlayFx(CHANNEL_4, 10, 0x0f, 0xf2, 0x64, 0x80);
			
			current_energy--;
			RefreshLife();
			if(current_energy == 0) {
				SetState(StateGameOver);
			} else {
				inmunity = inmunity_time;
			}
	}
}

//Wolf anim laughing
extern UINT8 anim_laughing[];

UINT8 current_pal = 0;
INT8 pal_tick = 0;

#ifdef NINTENDO 
const UINT8 pals[] = {PAL_DEF(0, 1, 2, 3), PAL_DEF(0, 0, 0, 0)};
#ifdef CGB
const UINT16 pal_on[]  = {RGB(31, 31, 31), RGB(20, 20, 20), RGB(10, 10, 10), RGB(0,   0,  0)};
const UINT16 pal_off[] = {RGB(31, 31, 31), RGB(31, 31, 31), RGB(31, 31, 31), RGB(31, 31, 31)};
const UINT16* pals_color[] = {pal_on, pal_off};
#endif
#endif

void UPDATE(void) {
	UINT16 expected_x;
	UINT16 expected_y;
	UINT8 coll_tile;
	UINT8 i;
	Sprite* spr;
	UINT8 sheepAng;

	switch(sheep_state) {
		case AIMING:
			if(!KEY_PRESSED(J_A)) {
				sheepAngOffset += sheepIncr << delta_time;

				if(sheepIncr == 2 && sheepAngOffset > sheepAngMax) {
					sheepIncr = -2;
					sheepAngOffset = sheepAngMax - 1;
				}
				if(sheepIncr == -2 && (0x80 & sheepAngOffset)) {
					sheepIncr = 2;
					sheepAngOffset = 0;
				}
			}

			sheepAng = sheepAngStart + sheepAngOffset;			
			crossHair->x = THIS->x - 4 + 3 + (SIN(sheepAng) >> 3); //-4 to center the cross, +3 to center in the sprite
			crossHair->y = THIS->y - 4 + (COS(sheepAng) >> 3);

			if(KEY_RELEASED(J_A)) {
				speed_x = SIN(sheepAng) << 2;
				speed_y = COS(sheepAng) << 3;
				ChangeState(FLYING);
			}

#ifndef NDEBUG
			if(KEY_PRESSED(J_LEFT)) {
				TranslateSprite(THIS, -1 << delta_time, 0);
			}
			if(KEY_PRESSED(J_RIGHT)) {
				TranslateSprite(THIS, 1 << delta_time, 0);
			}
			if(KEY_PRESSED(J_UP)) {
				TranslateSprite(THIS, 0, -1 << delta_time);
			}
			if(KEY_PRESSED(J_DOWN)) {
				TranslateSprite(THIS, 0, 1 << delta_time);
			}
#endif
			break;

		case FLYING:
			accum_x.w += speed_x << delta_time;
			accum_y.w += speed_y << delta_time;
			
			if(speed_y < 1024) {
				speed_y += gravity << delta_time;
			}
			
			expected_x = (THIS->x + (INT8)accum_x.b.h);
			expected_y = (THIS->y + (INT8)accum_y.b.h);
			coll_tile = TranslateSprite(THIS, accum_x.b.h, accum_y.b.h);
			if(coll_tile) {
				if(coll_tile == 18) {
					Hit();
				}

				if(expected_y > THIS->y || (coll_tile > 6 && coll_tile < 18)) {
					
					if(expected_y > THIS->y) {
						PlayFx(CHANNEL_1, 10, 0x1B, 0x00, 0xf1, 0x73, 0x86);
					} else {
						PlayFx(CHANNEL_4, 10, 0x08, 0xf1, 0x3b, 0x80);
					}

					//Adjust crosshair
					sheepAngOffset = 64;
					if(expected_x != THIS->x) {
						if(expected_x > THIS->x) {
							sheepAngStart = 128;
							sheepIncr = 2;
						} else {
							sheepAngStart = 0;
							sheepIncr = -2;
						}
					} else if(expected_y != THIS->y) { 
						if(expected_y > THIS->y) {
							sheepAngStart = 192;
							
							//Look on the current direction
							if(speed_x > 0) {
								sheepAngOffset = 128;
							} else {
								sheepAngOffset = 0;
							}

						} else if(expected_y < THIS->y) {
							sheepAngStart = 64;
						}
					}

					ChangeState(AIMING);
				} else {
					if(speed_y < 0 && (expected_y != THIS->y)) {
						speed_y = 0;
					}
				}
			}
			accum_x.b.h = 0;
			accum_y.b.h = 0;
			break;
	}

	SPRITEMANAGER_ITERATE(i, spr) {
		if(i == THIS_IDX || spr->type == SpriteCrosshair)
			continue;

		if(CheckCollision(THIS, spr)) {
			if(spr->type == SpriteBird) {
				Hit();
			} else if(spr->type == SpriteWolf) {
				Hit();
				spr->custom_data[0] = 100; //Laughing See SpriteWolf.c
				spr->anim_data = anim_laughing;
			} else if(spr->type == SpriteLife) {
				PlayFx(CHANNEL_1, 10, 0x45, 0x00, 0xff, 0x1f, 0x86);
				SpriteManagerRemove(i);

				VectorAdd(lifes_y, spr->y >> 3);

				if(current_energy != max_energy) {
					current_energy++;
					RefreshLife();
				}
			} else if(spr->type == SpritePlatform && last_platform != spr && sheep_state == FLYING) {
				PlayFx(CHANNEL_4, 10, 0x08, 0xf1, 0x3b, 0x80);
				player_parent = spr;
				ChangeState(AIMING);
			} else if(spr->type == SpriteBubble) {
				SpriteManagerRemove(i);
				SpriteManagerAdd(SpritePop, spr->x, spr->y);
				SpriteManagerAdd(SpriteFriendSheep, spr->x, spr->y + 8);
				ChangeState(DANCING);
			}
		}
	}

	if(inmunity != 0) {
		inmunity -= (1 << delta_time);
		if(inmunity < 1) {
			inmunity = 0;

			if(sheep_state == AIMING && sheepAngStart == 192) {
				if(TranslateSprite(THIS, 0, 1) == 18) {
					Hit();
				}
			}

			pal_tick = 0; //This forces the palette to be 0 after after the next if
			current_pal = 1;
		}

		pal_tick -= 1 << delta_time;
		if(U_LESS_THAN(pal_tick, 0)) {
			pal_tick += 3;
			current_pal ++;
#ifdef NINTENDO
#ifdef CGB
			if(_cpu == CGB_TYPE) {
				SetPalette(SPRITES_PALETTE, 1, 1, pals_color[current_pal % 2], 2);
			} else
#endif
			OBP1_REG = pals[current_pal % 2];
#endif
		}
	}
}

void DESTROY(void) {
}