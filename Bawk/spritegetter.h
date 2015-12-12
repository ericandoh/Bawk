//
//  spritegetter.h
//  Bawk
//
//  Created by Eric Oh on 12/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__spritegetter__
#define __Bawk__spritegetter__

#include <stdio.h>
#include "spriterender.h"

enum SpriteBehaviour {
    DEFAULT, EXPLOSION_RENDER
};

namespace Json {
    class Value;
}

SpriteRender* get_sprite_instance(int sid, Player* p);
void set_sprite_properties(SpriteRender* render, Json::Value node);

#endif /* defined(__Bawk__spritegetter__) */
