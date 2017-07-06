#pragma bank 2
#include "StateGameOver.h"
UINT8 bank_STATE_GAMEOVER = 2;

#include "ZGBMain.h"
#include "Keys.h"

#include "../res/src/gameover.h"
#include "../res/src/screen_start_tileset.h"
#include "../res/src/font.h"

#include "Scroll.h"
#include "Print.h"

extern const unsigned char * const polka_lose_mod_Data[];
void Start_STATE_GAMEOVER() {
	InitScrollTiles(0, 256, screen_start_tileset, 3);
	InitScroll(gameoverWidth, gameoverHeight, gameover, 0, 0, 3);
	SHOW_BKG;
	HIDE_WIN;

	INIT_FONT(font, 3, PRINT_BKG);
	PRINT(5, 8, "GAME OVER");

	PlayMusic(polka_lose_mod_Data, 4, 0);
}

void Update_STATE_GAMEOVER() {
	if(previous_keys && !keys) {
		SetState(STATE_MENU);
	}
}

