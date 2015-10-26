//
//  player.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "player.h"
#include "block_loader.h"
#include "superobject.h"
#include "world.h"

#define PI 3.14159265358979323846

Player::Player(uint32_t p) {
    // replace these with vectors later on
    angle = fvec2(0.0f, 0.0f);
    update_direction(0.0, 0.0);
    // TODO randomly generate pid from name of player
    pid = p;
    vid = 0;
    entity_class = 2;
    id_assign = 0;
    inventory = new PlayerInventory();
    mount = 0;
}

Player::~Player() {
    if (inventory)
        delete inventory;
}

uint32_t Player::getID() {
    return pid;
}

uint32_t Player::assignID() {
    return (uint32_t)(id_assign++);
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

fvec3 Player::step() {
    if (mount) {
        // track mount
        fvec3 old_pos = pos;
        pos = fvec3(mount->pos.x + offset_to_mount.x,
                    mount->pos.y + offset_to_mount.y,
                    mount->pos.z + offset_to_mount.z);
        return old_pos;
    }
    else {
        return RenderablePlayer::step();
    }
}

bool Player::collides_with(Entity* other) {
    if (mount) {
        return false;
    }
    return RenderablePlayer::collides_with(other);
}

void Player::set_mount(SuperObject* m, fvec3 pos) {
    mount = m;
    offset_to_mount = fvec3(pos.x - m->pos.x, pos.y - m->pos.y, pos.z - m->pos.z);
}

bool Player::unmount(World* world) {
    // try setting pos to about 2 blocks above current position
    pos.y += 2.0f;
    if (world->will_collide_with_anything(this)) {
        // return false if we can't unmount...because not room for player
        pos.y -= 2.0f;
        return false;
    }
    mount = 0;
    return true;
}

SuperObject* Player::get_mount() {
    return mount;
}

std::string Player::get_save_path() {
    return get_path_to_player(pid);
}

int Player::load_selfs() {
    if (RenderablePlayer::load_selfs()) {
        inventory->new_inv();
        return 1;
    }
    return 0;
}

int Player::load_self(IODataObject* obj) {
    if (RenderablePlayer::load_self(obj))
        return 1;
    // load player stuff here
    angle = obj->read_value<fvec2>();
    id_assign = obj->read_value<long>();
    inventory->load_self(obj);
    return 0;
}

void Player::remove_self(IODataObject* obj) {
    RenderablePlayer::remove_self(obj);
    // save player stuff here
    obj->save_value(angle);
    obj->save_value(id_assign);
    
    inventory->remove_self(obj);
}

void Player::debug() {
    printf("Player at %f,%f,%f pointing at %f,%f,%f\n",
            pos.x, pos.y, pos.z,
            dir.x, dir.y, dir.z);
}