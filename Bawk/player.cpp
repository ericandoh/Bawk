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
    vid = 0;
    pid = p;
    entity_class = EntityType::PLAYER;
    can_collide = true;
    can_rotate = false;
    
    // initialize things
    pos = fvec3(0.0f, 8.0f, 0.0f);
    
    //lower_bound = fvec3(-0.4f, -0.9f, 0.4f);
    //upper_bound = fvec3(0.4f, 0.9f, 0.4f);
    fvec3 lower_bound = fvec3(0.0f, 0.0f, 0.0f);
    fvec3 upper_bound = fvec3(0.9f, 0.9f, 0.9f);
    bounds = bounding_box(lower_bound, upper_bound);
    center_pos = fvec3(0.45f, 0.45f, 0.45f);
    
    recalculate_transform();
    
    weight = 20;
    velocity_decay = 0.5f;
    
    update_direction(0.0, 0.0);
    
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
    uint32_t id_to_assign = (uint32_t)(id_assign++);
    // save the player
    remove_selfs();
    return id_to_assign;
}

void Player::update_direction(double xdiff, double ydiff) {
    float anglex = -xdiff*STRAIGHT_ANGLE;
    float angley = -ydiff*STRAIGHT_ANGLE;
    
    angle.apply_angles(fvec3(anglex, angley, 0));
}

ivec3 Player::get_rounded_left() {
    if (fabsf(angle.dir.x) > fabsf(angle.dir.z)) {
        if (angle.dir.x < 0) {
            return ivec3(0, 0, -1);
        }
        return ivec3(0, 0, 1);
    }
    else {
        if (angle.dir.z < 0) {
            return ivec3(1, 0, 0);
        }
        return ivec3(-1, 0, 0);
    }
}

ivec3 Player::get_rounded_forward() {
    if (fabsf(angle.dir.x) > fabsf(angle.dir.z)) {
        if (angle.dir.x < 0) {
            return ivec3(-1, 0, 0);
        }
        return ivec3(1, 0, 0);
    }
    else {
        if (angle.dir.z < 0) {
            return ivec3(0, 0, -1);
        }
        return ivec3(0, 0, 1);
    }
}

void Player::step(Game* game) {
    RenderablePlayer::step(game);
}

void Player::set_mount(SuperObject* m, fvec3 rwc) {
    if (!m)
        return;
    if (mount) {
        // unmount first
        mount->remove_entity(this);
    }
    printf_fvec3(get_rwc_pos());
    printf("\n");
    mount = m;
    // no moving around while mounted!
    velocity = fvec3(0, 0, 0);
    // TODO this is hack fix
    this->set_pos(rwc);
    mount->add_entity(this);
    //m->transform_into_my_coordinates_smooth(&offset_to_mount, pos.x, pos.y, pos.z);
    //set_pos(pos - this->center_pos);
    can_collide = false;
    printf_fvec3(get_rwc_pos());
}

bool Player::unmount(World* world) {
    if (!mount) {
        return true;
    }
    printf_fvec3(get_rwc_pos());
    printf("\n");
    mount->remove_entity(this);
    // try setting pos to about 2 blocks above current position
    pos.y += 2.0f;
    if (world->will_collide_with_anything(this)) {
        // return false if we can't unmount...because not room for player
        pos.y -= 2.0f;
        mount->add_entity(this);
        return false;
    }
    velocity = mount->velocity;
    mount = 0;
    can_collide = true;
    angle.set_to_point(angle.forward, fvec3(0,1,0));
    recalculate_transform();
    printf_fvec3(get_rwc_pos());
    world->add_entity(this);
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
    id_assign = obj->read_value<unsigned long>();
    inventory->load_self(obj);
    return 0;
}

void Player::remove_self(IODataObject* obj) {
    RenderablePlayer::remove_self(obj);
    // save player stuff here
    obj->save_value(id_assign);
    
    inventory->remove_self(obj);
}

void Player::debug() {
    printf("Player at %f,%f,%f pointing at %f,%f,%f\n",
            pos.x, pos.y, pos.z,
            angle.dir.x, angle.dir.y, angle.dir.z);
}