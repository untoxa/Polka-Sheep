#include "Banks/SetAutoBank.h"

#include <gbdk/platform.h>

#include "main.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include "Math.h"
#include "Palette.h"
#include "string.h"
#include "Keys.h"
#include "Music.h"

IMPORT_TILES(font);
IMPORT_MAP(window);
IMPORT_MAP(level_complete);

IMPORT_MAP(maplevel1);
IMPORT_MAP(maplevellobo);
IMPORT_MAP(maplevelpajaro);
IMPORT_MAP(maplevelpuzzlesencilloylobo);
IMPORT_MAP(maplevelsubida);
IMPORT_MAP(maplevelpinchosabajo);
IMPORT_MAP(maplevelplataformashorizontal);
IMPORT_MAP(maplevelplataformaslobopajaro);
IMPORT_MAP(maplevelsubidapajaritos);
IMPORT_MAP(maplevelescondrijo);
IMPORT_MAP(mapleveldospisos);
IMPORT_MAP(maplevelsubidapro);
IMPORT_MAP(maplevelsubidapinchos);

#define BANKED_MAP(MAP) {BANK(MAP), &MAP}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(maplevel1),
	BANKED_MAP(maplevellobo),
	BANKED_MAP(maplevelpuzzlesencilloylobo),
	BANKED_MAP(maplevelpajaro),
	BANKED_MAP(maplevelsubida),
	BANKED_MAP(maplevelplataformashorizontal),
	BANKED_MAP(maplevelpinchosabajo),
	BANKED_MAP(maplevelplataformaslobopajaro),
	BANKED_MAP(maplevelsubidapinchos),
	BANKED_MAP(maplevelsubidapajaritos),
	BANKED_MAP(maplevelescondrijo),
	BANKED_MAP(mapleveldospisos),
	BANKED_MAP(maplevelsubidapro),
};

UINT8 current_level = 0;
const UINT8 num_levels = 13;

UINT8 collisions[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 0};

INT16 countdown;
INT8 countdown_tick;
extern UINT16 lifes_y[];

extern Sprite* friendsheep_sprite;
extern Sprite* player_sprite;
UINT16 level_complete_time;
typedef enum {
	PLAYING,
	LEVEL_COMPLETE,
} GameState;
GameState game_state;

DECLARE_MUSIC(polka_level1);
DECLARE_MUSIC(polka_win);

void RefreshLife(void) BANKED;

void START(void) {
	UINT16 start_x, start_y;
	const struct MapInfoBanked* level = &levels[current_level];
	UINT8 level_w, level_h;

#ifdef CGB
	//TODO: ensure the Player has its own palette for the blinking effect when hit
	if(_cpu == CGB_TYPE) {
		spritePalsOffset[SpritePlayer] = 1;
	}
#endif
	
	GetMapSize(level->bank, level->map, &level_w, &level_h);
	ScrollFindTile(level->bank, level->map, 4, 0, 0, level_w, level_h, &start_x, &start_y);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, ((start_y - 1) << 3) + 8);
	InitScroll(level->bank, level->map, collisions, 0);

#if defined(NINTENDO)
	INIT_HUD(window);
	RefreshLife();

	INIT_FONT(font, PRINT_WIN);
	PRINT_POS(0, 1);
	Printf("Level %d", (UINT16)(current_level + 1));
#elif defined(SEGA)
	INIT_FONT(font, PRINT_BKG);
	RefreshLife();
#endif

	countdown = 1024;
	countdown_tick = -1; //Force first update

	lifes_y[0] = 0; //reset lifes y array
	
	friendsheep_sprite = 0;
	game_state = PLAYING;

	PlayMusic(polka_level1, 1);

	INIT_SOUND();
}

#define END_Y 85
#define END_X 20

void UPDATE(void) {
	Sprite* spr;
	UINT8 i;

	switch(game_state) {
		case PLAYING:
			//Timer update
			/*countdown_tick -= 1 << delta_time;
			if(U_LESS_THAN(countdown_tick, 0)) {
				countdown_tick += 60;
				countdown --;
				PRINT_POS(0, 1);
				Printf("t: %u ", countdown);
			}*/

			if(friendsheep_sprite != 0) {
				game_state = LEVEL_COMPLETE;
				scroll_target = 0;
				level_complete_time = 0;
				SPRITEMANAGER_ITERATE(i, spr) {
					if(spr->type != SpritePlayer && spr->type != SpriteFriendSheep && spr->type != SpritePop) {
						SpriteManagerRemove(i);
					}
				}
				PlayMusic(polka_win, 0);
			}

#ifndef NDEBUG
			if(KEY_TICKED(J_SELECT)) {
				current_level ++;
				SetState(StateGame);
			}
#endif
			break;

		case LEVEL_COMPLETE:
			level_complete_time += 1;

			if(level_complete_time ==  10) { //SetGBFade(1);
			} else if(level_complete_time ==  20) { //SetGBFade(2);
			} else if(level_complete_time ==  30) { /*SetGBFade(3);*/ HIDE_WIN;
			} else if(level_complete_time ==  80) {
				player_sprite->x = player_sprite->x - scroll_x; friendsheep_sprite->x = friendsheep_sprite->x - scroll_x;
				player_sprite->y = player_sprite->y - scroll_y; friendsheep_sprite->y = friendsheep_sprite->y - scroll_y;
				InitScroll(BANK(level_complete), &level_complete, 0, 0);
			} else if(level_complete_time ==  90) {//SetGBFade(2);
			} else if(level_complete_time == 100) {//SetGBFade(1);
			} else if(level_complete_time == 110) {//SetGBFade(0);
			} else if(level_complete_time == 120) {
				print_target = PRINT_BKG;
				PRINT(3, 6, "LEVEL COMPLETE!");
			} else if(level_complete_time > 130) {
					if(previous_keys && !keys) {
						current_level ++;
						SetState(current_level == num_levels ? StateGameEnd : StateGame);
					}
			}
			

			if(player_sprite->x != scroll_x + (150 - END_X))
				player_sprite->x += U_LESS_THAN(player_sprite->x, scroll_x + (150 - END_X)) ? 1 : -1;

			if(player_sprite->y != scroll_y + END_Y)
				player_sprite->y += U_LESS_THAN(player_sprite->y, scroll_y + END_Y) ? 1 : -1;

			if(friendsheep_sprite->x != scroll_x + END_X)
				friendsheep_sprite->x += U_LESS_THAN(friendsheep_sprite->x, scroll_x + END_X) ? 1 : -1;

			if(friendsheep_sprite->y != scroll_y + END_Y)
				friendsheep_sprite->y += U_LESS_THAN(friendsheep_sprite->y, scroll_y + END_Y) ? 1 : -1;

			break;
	}
}