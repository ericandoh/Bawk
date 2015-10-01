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
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    dir = glm::vec3(0.1f, 0.1f, 0.1f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    angle = glm::vec2(0.0f, 0.0f);
    update_direction(0.0, 0.0);
    playerrender = PlayerRender();
}

void Player::set_camera() {
    playerrender.set_camera(&pos, &forward, &up, &dir);
}

// movement methods. Move these to class Entity
void Player::move_forward() {
    // override this if flying/on land
    pos += forward;
}
void Player::move_backward() {
    pos -= forward;
}
void Player::move_left() {
    pos.x += forward.z;
    pos.z -= forward.x;
}
void Player::move_right() {
    pos.x -= forward.z;
    pos.z += forward.x;
}
void Player::move_up() {
    pos.y += 1.0f;
}
void Player::move_down() {
    pos.y -= 1.0f;
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
    
    forward.x = sinf(angle.x);
    forward.y = 0;
    forward.z = cosf(angle.x);
    
    //right.x = -cosf(angle.x);
    //right.y = 0;
    //right.z = sinf(angle.x);
    
    dir.x = sinf(angle.x) * cosf(angle.y);
    dir.y = sinf(angle.y);
    dir.z = cosf(angle.x) * cosf(angle.y);
    
    //up = glm::cross(right, lookat);
}

void Player::debug() {
    printf("Player at %f,%f,%f pointing at %f,%f,%f\n",
            pos.x, pos.y, pos.z,
            dir.x, dir.y, dir.z);
}