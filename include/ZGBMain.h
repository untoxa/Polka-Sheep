#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES        \
_STATE(StateGame)     \
_STATE(StateMenu)     \
_STATE(StateGameOver) \
_STATE(StateGameEnd)  \
STATE_DEF_END

#define SPRITES \
_SPRITE(SpritePlayer,      player,       NO_MIRROR) \
_SPRITE(SpriteCrosshair,   crossair,     NO_MIRROR) \
_SPRITE(SpriteBird,        bird,         V_MIRROR)  \
_SPRITE(SpriteWolf,        wolf,       	 V_MIRROR)  \
_SPRITE(SpritePolkaman,    polkaman,     NO_MIRROR) \
_SPRITE(SpriteLife,        life,         NO_MIRROR) \
_SPRITE(SpritePlatform,    platform,     NO_MIRROR) \
_SPRITE(SpriteBubble,      bubble,       NO_MIRROR) \
_SPRITE(SpritePop,         pop,          NO_MIRROR) \
_SPRITE(SpriteFriendSheep, friendsheep,  NO_MIRROR) \
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif