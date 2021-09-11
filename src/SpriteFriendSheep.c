#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"
UINT8 anim_friendsheep_idle[] = {2, 0, 1};
Sprite* friendsheep_sprite = 0;

void Start_SpriteFriendSheep() {
	friendsheep_sprite = THIS;
	SetSpriteAnim(THIS, anim_friendsheep_idle, 4);
}

void Update_SpriteFriendSheep() {
}

void Destroy_SpriteFriendSheep() {
}