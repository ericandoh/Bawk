//
//  player.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "player.h"

#define PI 3.14159265358979323846

Player::Player() {
    // replace these with vectors later on
    angle = fvec2(0.0f, 0.0f);
    update_direction(0.0, 0.0);
    // TODO randomly generate pid from name of player
    pid = 7;
}

uint32_t Player::getID() {
    return pid;
}

void Player::update_direction(double xdiff, double ydiff) {
    
    angle.x -= xdiff * M_PI;
    angle.y -= ydiff * M_PI;
    
    if(angle.x < -M_PI)
        angle.x += M_PI * 2;
    if(angle.x > M_PI)
        angle.x -= M_PI * 2;
    if(angle.y < -M_PI / 2)
        angle.y = -M_PI / 2;
    if(angle.y > M_PI / 2)
        angle.y = M_PI / 2;
    
    //right.x = -cosf(angle.x);
    //right.y = 0;
    //right.z = sinf(angle.x);
    
    dir.x = sinf(angle.x) * cosf(angle.y);
    dir.y = sinf(angle.y);
    dir.z = cosf(angle.x) * cosf(angle.y);
    
    //up = glm::cross(right, lookat);
}

ivec3 Player::get_rounded_left() {
    if (fabsf(dir.x) > fabsf(dir.z)) {
        if (dir.x < 0) {
            return ivec3(0, 0, -1);
        }
        return ivec3(0, 0, 1);
    }
    else {
        if (dir.z < 0) {
            return ivec3(1, 0, 0);
        }
        return ivec3(-1, 0, 0);
    }
}

ivec3 Player::get_rounded_forward() {
    if (fabsf(dir.x) > fabsf(dir.z)) {
        if (dir.x < 0) {
            return ivec3(-1, 0, 0);
        }
        return ivec3(1, 0, 0);
    }
    else {
        if (dir.z < 0) {
            return ivec3(0, 0, -1);
        }
        return ivec3(0, 0, 1);
    }
}

void Player::debug() {
    printf("Player at %f,%f,%f pointing at %f,%f,%f\n",
            pos.x, pos.y, pos.z,
            dir.x, dir.y, dir.z);
}