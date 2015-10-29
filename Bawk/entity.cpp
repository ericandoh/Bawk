//
//  entity.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity.h"
#include <climits>
#include <glm/gtc/matrix_transform.hpp>

Entity::Entity() {
    // initialize things
    pos = fvec3(0.0f, 0.0f, 0.0f);
    up = fvec3(0.0f, 1.0f, 0.0f);
    dir = fvec3(1.0f, 0.0f, 0.0f);
    lower_bound = fvec3(FLT_MAX, FLT_MAX, FLT_MAX);
    upper_bound = fvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    
    center_pos = fvec3(0.0f, 0.0f, 0.0f);
    weight = 0;
    // 0 is full health
    health = 0;
    
    velocity = fvec3(0, 0, 0);
    angular_velocity = fvec2(0, 0);
    speed = 0.1f;
    vid = 0;
    pid = 0;
    entity_class = 0;   // please set me in future constructors!
    
    rpos = ivec3(0, 0, 0);
    rdir = ivec3(0, 0, 0);
}

void Entity::transform_into_my_coordinates(fvec3* src, float x, float y, float z) {
    // round center_pos to nearest 0.5
    fvec3 integral_center_pos(roundf(center_pos.x - 0.5f)+0.5f,
                              roundf(center_pos.y - 0.5f)+0.5f,
                              roundf(center_pos.z - 0.5f)+0.5f);
    // round angle to nearest angle
    fvec2 rounded_angle = fvec2(roundf(angle.x / M_PI) * M_PI,
                                roundf(angle.y / M_PI) * M_PI);

    fmat4 reverse(1);
    if (rounded_angle.x != 0 || rounded_angle.y != 0) {
        reverse = glm::translate(fmat4(1), integral_center_pos);
        reverse = glm::rotate(reverse, -rounded_angle.y, fvec3(cosf(rounded_angle.x), 0, -sinf(rounded_angle.x)));
        reverse = glm::rotate(reverse, -rounded_angle.x, fvec3(0, 1, 0));
        reverse = glm::translate(reverse, -integral_center_pos);
    }
    reverse = glm::translate(reverse, -pos);
    fvec4 result(x, y, z, 1.0f);
    result = reverse * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_world_coordinates(fvec3* src, float x, float y, float z) {
    // round center_pos to nearest 0.5
    fvec3 integral_center_pos(roundf(center_pos.x - 0.5f)+0.5f,
                              roundf(center_pos.y - 0.5f)+0.5f,
                              roundf(center_pos.z - 0.5f)+0.5f);
    // round angle to nearest angle
    fvec2 rounded_angle = fvec2(roundf(angle.x / M_PI) * M_PI,
                                roundf(angle.y / M_PI) * M_PI);
    // S * R * T
    fmat4 view = glm::translate(fmat4(1), pos);
    if (rounded_angle.x != 0 || rounded_angle.y != 0) {
        view = glm::translate(view, integral_center_pos);
        view = glm::rotate(view, rounded_angle.x, fvec3(0, 1, 0));
        view = glm::rotate(view, rounded_angle.y, fvec3(cosf(rounded_angle.x), 0, -sinf(rounded_angle.x)));
        view = glm::translate(view, -integral_center_pos);
    }
    fvec4 result(x, y, z, 1.0f);
    result = view * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

// movement methods. Move these to class Entity
void Entity::move_forward() {
    // override this if flying/on land
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity += forward * speed;
    stable = false;
}
void Entity::move_backward() {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity -= forward * speed;
    stable = false;
}
void Entity::move_left() {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity.x += forward.z * speed;
    velocity.z -= forward.x * speed;
    stable = false;
}
void Entity::move_right() {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    velocity.x -= forward.z * speed;
    velocity.z += forward.x * speed;
    stable = false;
}
void Entity::move_up() {
    velocity.y += 1.0f * speed;
    stable = false;
}
void Entity::move_down() {
    velocity.y -= 1.0f * speed;
    stable = false;
}

void Entity::turn_left() {
    angular_velocity.x += 0.3f;
    recalculate_dir();
}

void Entity::turn_right() {
    angular_velocity.x -= 0.3f;
    recalculate_dir();
}

void Entity::recalculate_dir() {
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
    // TODO recalculate pos
}

fvec3* Entity::get_pos() {
    // TODO deprecate this
    return &pos;
}

bool Entity::poke(float x, float y, float z) {
    // transform into OAC
    fvec3 oac;
    // TODO same note as above
    transform_into_my_coordinates(&oac, x, y, z);
    
    return (lower_bound.x <= oac.x) && (oac.x <= upper_bound.x) &&
            (lower_bound.y <= oac.y) && (oac.y <= upper_bound.y) &&
            (lower_bound.z <= oac.z) && (oac.z <= upper_bound.z);
}

bool Entity::block_keyboard_callback(Game* game, int key) {
    return false;
}

bool Entity::block_mouse_callback(Game* game, int button) {
    /*block_mouse_callback_func callback = get_block_mouse_callback_from(something);
    if (callback) {
        (*callback)(game,
                    this,
                    0,
                    fvec3(0, 0, 0),
                    button);
        return true;
    }*/
    return false;
}

void Entity::step() {
    // do nothing
}

void Entity::render(fmat4* transform) {
    // do nothing
}

void Entity::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    // do nothing
}

void Entity::calculate_moving_bounding_box() {
    // transform my bounds into RWC
    fvec3 lower_rwc, upper_rwc;
    transform_into_world_coordinates(&lower_rwc, lower_bound.x, lower_bound.y, lower_bound.z);
    transform_into_world_coordinates(&upper_rwc, upper_bound.x, upper_bound.y, upper_bound.z);
    fvec3 before_lower_corner(std::min(lower_rwc.x, upper_rwc.x),
                              std::min(lower_rwc.y, upper_rwc.y),
                              std::min(lower_rwc.z, upper_rwc.z));
    fvec3 before_upper_corner(std::max(lower_rwc.x, upper_rwc.x),
                              std::max(lower_rwc.y, upper_rwc.y),
                              std::max(lower_rwc.z, upper_rwc.z));
    
    apply_velocity();
    apply_rotation();
    
    transform_into_world_coordinates(&lower_rwc, lower_bound.x, lower_bound.y, lower_bound.z);
    transform_into_world_coordinates(&upper_rwc, upper_bound.x, upper_bound.y, upper_bound.z);
    fvec3 after_lower_corner(std::min(lower_rwc.x, upper_rwc.x),
                             std::min(lower_rwc.y, upper_rwc.y),
                             std::min(lower_rwc.z, upper_rwc.z));
    fvec3 after_upper_corner(std::max(lower_rwc.x, upper_rwc.x),
                             std::max(lower_rwc.y, upper_rwc.y),
                             std::max(lower_rwc.z, upper_rwc.z));
    
    moving_lower = fvec3(std::min(before_lower_corner.x, after_lower_corner.x),
                         std::min(before_lower_corner.y, after_lower_corner.y),
                         std::min(before_lower_corner.z, after_lower_corner.z));
    moving_upper = fvec3(std::max(before_upper_corner.x, after_upper_corner.x),
                         std::max(before_upper_corner.y, after_upper_corner.y),
                         std::max(before_upper_corner.z, after_upper_corner.z));
    
}

void Entity::revert_velocities() {
    pos -= velocity;
    angle -= angular_velocity;
    recalculate_dir();
}

void Entity::apply_velocity() {
    pos += velocity;
}

void Entity::apply_rotation() {
    angle -= angular_velocity;
    recalculate_dir();
}

void Entity::reset_velocities() {
    angular_velocity = fvec2(0, 0);
    velocity = fvec3(0, 0, 0);
    stable = true;
}

bool Entity::intersects_with_my_bounds(fvec3 lower_corner, fvec3 upper_corner) {
    return      !(lower_bound.x >= upper_corner.x || lower_corner.x >= upper_bound.x
               || lower_bound.y >= upper_corner.y || lower_corner.y >= upper_bound.y
               || lower_bound.z >= upper_corner.z || lower_corner.z >= upper_bound.z);
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
    
    fvec3 real_lower_oac = fvec3(std::min(lower_oac.x, upper_oac.x),
                                 std::min(lower_oac.y, upper_oac.y),
                                 std::min(lower_oac.z, upper_oac.z));
    fvec3 real_upper_oac = fvec3(std::max(lower_oac.x, upper_oac.x),
                                 std::max(lower_oac.y, upper_oac.y),
                                 std::max(lower_oac.z, upper_oac.z));
    
    // see if lower_oac, upper_oac intersect with lower, upper
    if (!intersects_with_my_bounds(real_lower_oac, real_upper_oac)) {
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
    angle = obj->read_value<fvec2>();
    lower_bound = obj->read_value<fvec3>();
    upper_bound = obj->read_value<fvec3>();
    
    center_pos = obj->read_value<fvec3>();
    weight = obj->read_value<int>();
    health = obj->read_value<int>();
    
    recalculate_dir();
    
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
    obj->save_value(angle);
    obj->save_value(lower_bound);
    obj->save_value(upper_bound);
    obj->save_value(center_pos);
    obj->save_value(weight);
    obj->save_value(health);
}
