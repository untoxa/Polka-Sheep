#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"

UINT8 anim_pop_idle[] = {4, 0, 1, 1, 1};

void START(void) {
	SetSpriteAnim(THIS, anim_pop_idle, 7);
}

void UPDATE(void) {
	if(THIS->anim_frame == 2) {
		SpriteManagerRemove(THIS_IDX);
	}
}

void DESTROY(void) {
}