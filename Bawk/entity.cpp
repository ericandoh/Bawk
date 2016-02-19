//
//  entity.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity.h"
#include "superobject.h"
#include "entity_loader.h"
#include <glm/gtc/matrix_transform.hpp>

#include "player.h"

#define DEFAULT_VELOCITY_DECAY 0.5f

Entity::Entity() {
    // identifying info for the entity. this should be overwritten
    vid = 0;
    pid = 0;
    entity_class = EntityType::ENTITY;
    can_collide = true;
    stable = false;
    bounds = bounding_box();
    velocity = fvec3(0, 0, 0);
    angular_velocity = fvec3(0, 0, 0);
    velocity_decay = DEFAULT_VELOCITY_DECAY;
    weight = 0;
    // 0 is full health
    health = 0;
    selected = false;
    
    // start out with no parent
    // damn that's depresssing
    parent = 0;
}

void Entity::transform_into_my_coordinates(fvec3* src, float x, float y, float z) {
    src->x = x;
    src->y = y;
    src->z = z;
    if (parent) {
        parent->transform_into_my_coordinates(src, src->x, src->y, src->z);
    }
    Positionable::transform_into_my_coordinates(src, src->x, src->y, src->z);
}

void Entity::transform_into_my_integer_coordinates(ivec3* src, int x, int y, int z) {
    src->x = x;
    src->y = y;
    src->z = z;
    if (parent) {
        parent->transform_into_my_integer_coordinates(src, src->x, src->y, src->z);
    }
    Positionable::transform_into_my_integer_coordinates(src, src->x, src->y, src->z);
}

void Entity::transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z) {
    src->x = x;
    src->y = y;
    src->z = z;
    if (parent) {
        parent->transform_into_my_coordinates_smooth(src, src->x, src->y, src->z);
    }
    Positionable::transform_into_my_coordinates_smooth(src, src->x, src->y, src->z);
}

void Entity::transform_into_world_coordinates(fvec3* src, float x, float y, float z) {
    Positionable::transform_into_world_coordinates(src, x, y, z);
    if (parent) {
        parent->transform_into_world_coordinates(src, src->x, src->y, src->z);
    }
}

void Entity::transform_into_world_integer_coordinates(ivec3* src, int x, int y, int z) {
    Positionable::transform_into_world_integer_coordinates(src, x, y, z);
    if (parent) {
        parent->transform_into_world_integer_coordinates(src, src->x, src->y, src->z);
    }
}

void Entity::transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z) {
    Positionable::transform_into_world_coordinates_smooth(src, x, y, z);
    if (parent) {
        parent->transform_into_world_coordinates_smooth(src, src->x, src->y, src->z);
    }
}

void Entity::transform_into_my_rotation(Rotation* dst, Rotation target) {
    *dst = target;
    if (parent) {
        parent->transform_into_my_rotation(dst, *dst);
    }
    Positionable::transform_into_my_rotation(dst, *dst);
}

void Entity::transform_into_world_rotation(Rotation* dst, Rotation target) {
    Positionable::transform_into_world_rotation(dst, target);
    if (parent) {
        parent->transform_into_world_rotation(dst, *dst);
    }
}

fvec3 Entity::get_world_pos() {
    fvec3 result = Positionable::get_world_pos();
    if (parent) {
        parent->transform_into_world_coordinates_smooth(&result,
                                                        result.x,
                                                        result.y,
                                                        result.z);
        
    }
    return result;
}

Rotation Entity::get_world_rotation() {
    Rotation rot = Positionable::get_world_rotation();
    if (parent) {
        parent->transform_into_world_rotation(&rot, rot);
    }
    return rot;
}

void Entity::set_parent(SuperObject* parent) {
    // transform entity rotation/pos into this object's frame
    // after transformation, the rwc center position should be the same
    fvec3 global_center = get_world_pos();
    Rotation global_rotation = get_world_rotation();
    fvec3 oac;
    if (parent) {
        parent->transform_into_my_coordinates(&oac, global_center.x, global_center.y, global_center.z);
    }
    else {
        oac = global_center;
    }
    this->parent = parent;
    set_pos(oac - get_center_offset());
    Rotation aligned_rot;
    if (parent) {
        parent->transform_into_my_rotation(&aligned_rot, global_rotation);
    }
    else {
        aligned_rot = global_rotation;
    }
    set_angle(aligned_rot);
}

void Entity::set_bounds(bounding_box bounds) {
    this->bounds = bounds;
    update_centerpos();
}

void Entity::update_centerpos() {
    update_centerpos(bounds);
}
void Entity::update_centerpos(bounding_box bounds) {
    // this should be called whenever our bounds change
    // use bounds to set centerpos
    if (!can_rotate || bounds.lower.x == FLT_MAX || bounds.lower.x == FLT_MIN) {
        // TODO this is hack, do this properly
        center_pos = fvec3(0, 0, 0);
        return;
    }
    // we will always be pointing in the positive x-direction
    // we want to put the centerpos such that it is offset by 0.5 at each coordinate
    // or integer at each coordinate. this ensures nice rotations for our blocks
    // in addition, we want the vehicle to be perfectly centered in the positive x direction
    
    // first, get the center z coordinate (perpendicular to our direction)
    fvec3 exact_centerpos = fvec3((bounds.upper.x - bounds.lower.x) / 2.0f,
                                  (bounds.upper.y - bounds.lower.y) / 2.0f,
                                  (bounds.upper.z - bounds.lower.z) / 2.0f);
    exact_centerpos = exact_centerpos + bounds.lower;
    // see if it is closer to 0.5 or to 1.0
    bool align_to_half = false;
    int rounded_centerz = roundf(exact_centerpos.z * 2.0f);
    if (get_positive_mod(rounded_centerz, 2) == 1) {
        align_to_half = true;
    }
    
    if (align_to_half) {
        set_centerpos(fvec3(roundf(exact_centerpos.x - 0.5f) + 0.5f,
                            roundf(exact_centerpos.y - 0.5f) + 0.5f,
                            roundf(exact_centerpos.z - 0.5f) + 0.5f));
    }
    else {
        set_centerpos(fvec3(roundf(exact_centerpos.x),
                            roundf(exact_centerpos.y),
                            roundf(exact_centerpos.z)));
    }
    
    // TODO update position so that despite center pos having changed, blocks are still where they used to be
    
}

void Entity::update_centerpos_smooth() {
    set_centerpos(bounds.lower + fvec3((bounds.upper.x - bounds.lower.x) / 2.0f,
                                       (bounds.upper.y - bounds.lower.y) / 2.0f,
                                       (bounds.upper.z - bounds.lower.z) / 2.0f));
}

void Entity::set_centerpos(fvec3 cp) {
    // convert our centerpos to rwc, before our centerpos has been updated
    fvec3 rwc_new_centerpos;
    Positionable::transform_into_world_coordinates(&rwc_new_centerpos, cp.x, cp.y, cp.z);
    // get pos offset of the centerpos from (0,0,0)
    fvec3 rwc_pos = rwc_new_centerpos - cp;
    center_pos = cp;
    set_pos(rwc_pos);
}

float Entity::get_speed(float force) {
    // if we have negative/zero weight, treat as us having 1 weight
    if (weight <= 0) {
        return force;
    }
    return force/weight;
}

// movement methods. Move these to class Entity
void Entity::move_forward(float force) {
    // override this if flying/on land
    move_dist(angle.get_dir() * get_speed(force));
}

void Entity::move_backward(float force) {
    move_dist(angle.get_dir() * (-get_speed(force)));
}

void Entity::move_forward_flat(float force) {
    // override this if flying/on land
    move_dist(angle.get_forward() * get_speed(force));
}

void Entity::move_backward_flat(float force) {
    move_dist(angle.get_forward() * (-get_speed(force)));
}

void Entity::move_left(float force) {
    fvec3 forward = angle.get_forward();
    move_dist(fvec3(forward.z * get_speed(force),
                    0,
                    -forward.x * get_speed(force)));
}

void Entity::move_right(float force) {
    fvec3 forward = angle.get_forward();
    move_dist(fvec3(-forward.z * get_speed(force),
                    0,
                    forward.x * get_speed(force)));
}

void Entity::move_up(float force) {
    fvec3 up = angle.get_up();
    move_dist(get_speed(force)*up);
}

void Entity::move_down(float force) {
    fvec3 up = angle.get_up();
    move_dist(-get_speed(force)*up);
}

void Entity::move_up_flat(float force) {
    move_dist(fvec3(0.0,
                    get_speed(force),
                    0.0));
}

void Entity::move_down_flat(float force) {
    move_dist(fvec3(0.0,
                    -get_speed(force),
                    0.0));
}

void Entity::yaw_left(float force) {
    // 1 unit of force turns 1 weight by M_PI/2
    float angular_force = get_speed(force)*RIGHT_ANGLE;
    turn_angle(fvec3(angular_force,
                     0,
                     0));
}

void Entity::yaw_right(float force) {
    float angular_force = -get_speed(force)*RIGHT_ANGLE;
    turn_angle(fvec3(angular_force,
                     0,
                     0));
}

void Entity::pitch_up(float force) {
    float angular_force = get_speed(force)*RIGHT_ANGLE;
    turn_angle(fvec3(0,
                     angular_force,
                     0));
}

void Entity::pitch_down(float force) {
    float angular_force = -get_speed(force)*RIGHT_ANGLE;
    turn_angle(fvec3(0,
                     angular_force,
                     0));
}

void Entity::roll_left(float force) {
    float angular_force = -get_speed(force)*RIGHT_ANGLE;
    turn_angle(fvec3(0,
                     0,
                     angular_force));
}

void Entity::roll_right(float force) {
    float angular_force = get_speed(force)*RIGHT_ANGLE;
    turn_angle(fvec3(0,
                     0,
                     angular_force));
}

void Entity::move_dist(fvec3 off) {
    velocity += off;
    stable = false;
}

void Entity::turn_angle(fvec3 off) {
    angular_velocity += off;
    stable = false;
}

void Entity::print_debug() {
    print_entity_type(entity_class);
    printf(" (%d,%d) of class %d", pid, vid, entity_class);
    printf("\nPos: ");
    printf_fvec3(pos);
    printf("\nDir: ");
    printf_fvec3(angle.get_dir());
    printf("\nCenter: ");
    printf_fvec3(center_pos);
    printf("\nBounds:\n");
    printf_fvec3(bounds.lower);
    printf("\n");
    printf_fvec3(bounds.upper);
    printf("\n");
}

Entity* Entity::poke(float x, float y, float z) {
    // transform into OAC
    fvec3 oac;
    transform_into_my_coordinates(&oac, x, y, z);
    if (bounds.hits(oac)){
        return this;
    }
    return 0;
}

bool Entity::poke_rough(bounding_box box) {
    // transform box into my coordinates
    bounding_box my_box;
    transform_into_my_coordinates(&my_box.lower, box.lower.x, box.lower.y, box.lower.z);
    transform_into_my_coordinates(&my_box.upper, box.upper.x, box.upper.y, box.upper.z);
    my_box.refit_for_rotation();
    if (bounds.hits(my_box)) {
        return true;
    }
    return false;
}

bool Entity::can_be_hurt(BreakablePolicy policy, uint32_t o_pid) {
    if (policy == ACTIONED) {
        // if actioned, a player can't hurt himself unless he is the world (pid 0)
        return (pid != o_pid) || (o_pid == 0);
    }
    else if (policy == EXECUTED) {
        // if executed, only the player can hurt himself, unless he is the world (pid 0) in
        // which a world executed action can hurt anything
        return (pid == o_pid) || (o_pid == 0) || (pid == 0);
    }
    else {
        return false;
    }
}

bool Entity::can_be_destroyed(BreakablePolicy policy, uint32_t o_pid) {
    if (policy == ACTIONED) {
        // no actioned policies can destroy an object, unless this comes from the world
        return pid == 0;
    }
    else if (policy == EXECUTED) {
        // only the player or the world (0) can destroy an object with executive privileges
        return (pid == o_pid) || (o_pid == 0) || (pid == 0);
    }
    else {
        return false;
    }
}

float Entity::calculate_damage(float dmg, float resistance) {
    return ((10.0f / (10.0f + resistance)) * dmg);
}

bool Entity::get_hurt(float x, float y, float z, float dmg, BreakablePolicy policy, Player* player) {
    uint32_t pid = 0;
    if (player) {
        pid = player->getID();
    }
    if (!can_be_hurt(policy, pid)) {
        return false;
    }
    /*if (poke(x,y,z) != this) {
        return false;
    }*/
    // i have no resistance. take dmg directly to health
    health += dmg;
    if (health >= MAX_HEALTH) {
        health = MAX_HEALTH;
        if (can_be_destroyed(policy, pid)) {
            drop_loot(player);
            remove_self();
        }
    }
    return true;
}

void Entity::add_bounds_to_box(bounding_box* moving_bounds) {
    // transform my bounds into RWC
    fvec3 lower_rwc, upper_rwc;
    transform_into_world_coordinates(&lower_rwc, bounds.lower.x, bounds.lower.y, bounds.lower.z);
    transform_into_world_coordinates(&upper_rwc, bounds.upper.x, bounds.upper.y, bounds.upper.z);
    moving_bounds->expand(lower_rwc);
    moving_bounds->expand(upper_rwc);
}

void Entity::step_motion(fvec3* prev_pos, Rotation* prev_rot, bounding_box* moving_bounds) {
    *prev_pos = pos;
    *prev_rot = angle;
    
    // transform my bounds into RWC
    add_bounds_to_box(moving_bounds);
    pos += velocity;
    if (can_rotate) {
        angle.apply_angles(angular_velocity);
    }
    add_bounds_to_box(moving_bounds);
}

bool Entity::collides_with(Entity* other) {
    if (!can_collide || !other->can_collide) {
        return false;
    }
    
    int my_collision_level = get_collision_level();
    int other_collision_level = other->get_collision_level();
    
    bounding_box rwc_bounds;
    transform_into_world_coordinates(&rwc_bounds.lower, bounds.lower.x, bounds.lower.y, bounds.lower.z);
    transform_into_world_coordinates(&rwc_bounds.upper, bounds.upper.x, bounds.upper.y, bounds.upper.z);
    rwc_bounds.refit_for_rotation();
    
    bounding_box other_rwc_bounds;
    other->transform_into_world_coordinates(&other_rwc_bounds.lower, other->bounds.lower.x, other->bounds.lower.y, other->bounds.lower.z);
    other->transform_into_world_coordinates(&other_rwc_bounds.upper, other->bounds.upper.x, other->bounds.upper.y, other->bounds.upper.z);
    other_rwc_bounds.refit_for_rotation();
    
    if (!rwc_bounds.hits(other_rwc_bounds)) {
        // my big fat bounds doesn't even hit the other bounds
        return false;
    }
    
    if (my_collision_level >= other_collision_level) {
        return collides_with(other, &rwc_bounds, &other_rwc_bounds, my_collision_level, other_collision_level);
    }
    else {
        return other->collides_with(this, &other_rwc_bounds, &rwc_bounds, other_collision_level, my_collision_level);
    }
}

int Entity::get_collision_level() {
    return 0;
}

bool Entity::collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level) {
    // see if other (with bounds other_bounds) intersects me at my collision level
    
    // my collision level must be 0 if we're calling it at this state
    if (my_collision_lvl > 0 || other_collision_level > 0) {
        printf("Error: We skipped a collision level somewhere: %d,%d\n", my_collision_lvl, other_collision_level);
    }
    if (my_bounds->hits(*other_bounds)) {
        // printf("collision detected\n");
        return true;
    }
    return false;
}

std::string Entity::get_save_path() {
    return "";
}

int Entity::load_selfs() {
    std::string path = this->get_save_path();
    return load_selfs(path);
}

int Entity::load_selfs(std::string path) {
    if (path.compare("") == 0)
        return 1;
    IODataObject read(path);
    if (read.read())
        return 1;
    if (load_self(&read)) {
        read.close();
        return 1;
    }
    read.close();
    return 0;
}

void Entity::save_selfs() {
    std::string path = this->get_save_path();
    if (path.compare("") == 0)
        return;
    IODataObject write(path);
    if (write.save())
        return;
    save_self(&write);
    write.close();
}

int Entity::load_self(IODataObject* obj) {
    //up = obj->read_value<fvec3>();
    set_pos(obj->read_value<fvec3>());
    set_angle(obj->read_value<Rotation>());
    
    bounds = obj->read_value<bounding_box>();
    center_pos = obj->read_value<fvec3>();
    
    weight = obj->read_value<int>();
    health = obj->read_value<int>();
    
    return 0;
}

void Entity::save_self(IODataObject* obj) {
    //obj->save_value(up);
    obj->save_value(pos);
    obj->save_value(angle);
    
    obj->save_value(bounds);
    obj->save_value(center_pos);
    
    obj->save_value(weight);
    obj->save_value(health);
}

Entity* Entity::find_top_level_parent() {
    if (parent) {
        Entity* src = this;
        while (src->parent->parent) {
            src = src->parent;
        }
        return src;
    }
    else {
        return this;
    }
}

void Entity::remove_self() {
    parent->remove_entity(this);
    // TODO skip this check if entity doenst need to be written/deleted, to speedup
    delete_entity_from_memory(this);
    delete this;
}
