//
//  entity.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity.h"
#include <climits>

Entity::Entity() {
    // initialize things
    pos = fvec3(0.0f, 0.0f, 0.0f);
    up = fvec3(0.0f, 1.0f, 0.0f);
    dir = fvec3(1.0f, 0.0f, 0.0f);
    lower_bound = fvec3(FLT_MAX, FLT_MAX, FLT_MAX);
    upper_bound = fvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    
    velocity = fvec3(0, 0, 0);
    speed = 0.1f;
}

Entity::Entity(fvec3 p, fvec3 v, fvec3 u, fvec3 d, fvec3 lower, fvec3 upper) {
    pos = p;
    up = u;
    dir = d;
    lower_bound = lower;
    upper_bound = upper;
    
    velocity = v;
    speed = 0.1f;
}

void Entity::transform_into_my_coordinates(fvec3* src, float x, float y, float z) {
    // TODO this needs work to compensate for rotations and whatnot
    src->x = x - pos.x;
    src->y = y - pos.y;
    src->z = z - pos.z;
    
    //view = glm::lookAt(pos, pos + dir, up);
    //fvec3(src.x, src.y, src.z
}

void Entity::transform_into_world_coordinates(fvec3* src, float x, float y, float z) {
    src->x = x + pos.x;
    src->y = y + pos.y;
    src->z = z + pos.z;
}

// movement methods. Move these to class Entity
void Entity::move_forward() {
    // override this if flying/on land
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity += forward;
}
void Entity::move_backward() {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity -= forward;
}
void Entity::move_left() {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity.x += forward.z;
    velocity.z -= forward.x;
}
void Entity::move_right() {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity.x -= forward.z;
    velocity.z += forward.x;
}
void Entity::move_up() {
    velocity.y += 1.0f;
}
void Entity::move_down() {
    velocity.y -= 1.0f;
}

fvec3* Entity::get_pos() {
    return &pos;
}

fvec3 Entity::set_pos(fvec3 to_pos) {
    // this OVERRIDES collision detection checks and should be ONLY called
    // for special scenarios
    fvec3 old_pos = pos;
    pos = to_pos;
    return old_pos;
}

bool Entity::has_moved() {
    return velocity.x != 0 || velocity.y != 0 || velocity.z != 0;
}

fvec3 Entity::step() {
    fvec3 new_pos = fvec3(pos.x + velocity.x * speed,
                          pos.y + velocity.y * speed,
                          pos.z + velocity.z * speed);
    velocity = fvec3(0, 0, 0);
    return set_pos(new_pos);
}

void Entity::render(fmat4* transform) {
    // do nothing
}

void Entity::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    // do nothing
}

bool Entity::intersects_with_my_bounds(fvec3 lower_corner, fvec3 upper_corner) {
    return      !(lower_bound.x > upper_corner.x || lower_corner.x > upper_bound.x
               || lower_bound.y > upper_corner.y || lower_corner.y > upper_bound.y
               || lower_bound.z > upper_corner.z || lower_corner.z > upper_bound.z);
}

bool Entity::collides_with(Entity* other) {
    if (get_collision_priority() >= other->get_collision_priority()) {
        return check_collision_vs(other);
    }
    // other entity has a better way of dealing with collision
    return other->check_collision_vs(this);
}

bool Entity::check_collision_vs(Entity* other) {
    // we can handle this
    // we GOT this
    // other entity MUST be also an entity
    return collides_with_entity(other);
}

int Entity::get_collision_priority() {
    return 0;
}

bool Entity::collides_with_entity(Entity* other) {
    fvec3 lower, upper;
    fvec3 other_lower = other->lower_bound;
    fvec3 other_upper = other->upper_bound;
    // TODO the below might give us not a bounding box if the item is rotated
    // make a new function that calculates the "future" to the nearest 90 rotation (so that we have
    // square transformations)
    other->transform_into_world_coordinates(&lower, other_lower.x, other_lower.y, other_lower.z);
    other->transform_into_world_coordinates(&upper, other_upper.x, other_upper.y, other_upper.z);
    
    // transform into OAC
    fvec3 lower_oac;
    // TODO same note as above
    transform_into_my_coordinates(&lower_oac, lower.x, lower.y, lower.z);
    
    // transform into OAC
    fvec3 upper_oac;
    // TODO same note as above
    transform_into_my_coordinates(&upper_oac, upper.x, upper.y, upper.z);
    
    // see if lower_oac, upper_oac intersect with lower, upper
    if (!intersects_with_my_bounds(lower_oac, upper_oac)) {
        return false;
    }
    return true;
}

std::string Entity::get_save_path() {
    return "";
}

int Entity::load_selfs() {
    std::string path = this->get_save_path();
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

int Entity::load_self(IODataObject* obj) {
    velocity = obj->read_value<fvec3>();
    speed = obj->read_value<float>();
    pos = obj->read_value<fvec3>();
    up = obj->read_value<fvec3>();
    dir = obj->read_value<fvec3>();
    lower_bound = obj->read_value<fvec3>();
    upper_bound = obj->read_value<fvec3>();
    return 0;
}

void Entity::remove_selfs() {
    std::string path = this->get_save_path();
    if (path.compare("") == 0)
        return;
    IODataObject write(path);
    if (write.save())
        return;
    remove_self(&write);
    write.close();
}

void Entity::remove_self(IODataObject* obj) {
    obj->save_value(velocity);
    obj->save_value(speed);
    obj->save_value(pos);
    obj->save_value(up);
    obj->save_value(dir);
    obj->save_value(lower_bound);
    obj->save_value(upper_bound);
}
