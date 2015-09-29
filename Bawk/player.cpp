//
//  player.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "player.h"

Player::Player() {
    pos = {0.0f, 0.0f, 0.0f};
    dir = {0.0f, 0.0f, 0.0f};
    playerrender = PlayerRender();
}

void Player::set_camera() {
    playerrender.set_camera(&pos, &dir);
}