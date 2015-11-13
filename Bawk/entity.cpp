//
//  entity.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity.h"
#include <glm/gtc/matrix_transform.hpp>

Entity::Entity() {
    // these will be overwritten by a call to set angle
    up = fvec3(0.0f, 1.0f, 0.0f);
    dir = fvec3(1.0f, 0.0f, 0.0f);
    // identifying info for the entity. this should be overwritten
    vid = 0;
    pid = 0;
    entity_class = 0;
    
    // this should be overridden as needed
    can_collide = true;
    can_rotate = false;
    
    // set initial pos/angle to origin, overwrite for most objects
    pos = fvec3(0.0f, 0.0f, 0.0f);
    angle = fvec2(0.0f, 0.0f);
    
    // set initial bounds to be maxed out
    bounds = bounding_box();
    center_pos = fvec3(0.0f, 0.0f, 0.0f);
    
    // this object is (hopefully) not moving to start out with
    velocity = fvec3(0, 0, 0);
    angular_velocity = fvec2(0, 0);
    // do a stable check to begin with
    stable = false;
    
    // start with no weight
    weight = 0;
    // 0 is full health
    health = 0;
}

void Entity::transform_into_my_coordinates(fvec3* src, float x, float y, float z) {
    fmat4 reverse(1);
    if (can_rotate) {
        // round angle to nearest angle
        fvec2 rounded_angle = fvec2(roundf(angle.x * 2 / M_PI) * M_PI / 2,
                                    roundf(angle.y * 2 / M_PI) * M_PI / 2);
        
        
        reverse = glm::translate(fmat4(1), center_pos);
        reverse = glm::rotate(reverse, -rounded_angle.y, fvec3(cosf(rounded_angle.x), 0, -sinf(rounded_angle.x)));
        reverse = glm::rotate(reverse, -rounded_angle.x, fvec3(0, 1, 0));
        reverse = glm::translate(reverse, -center_pos);
    }
    reverse = glm::translate(reverse, -pos);
    fvec4 result(x, y, z, 1.0f);
    result = reverse * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z) {
    fmat4 reverse(1);
    if (can_rotate) {
        // round angle to nearest angle
        reverse = glm::translate(fmat4(1), center_pos);
        reverse = glm::rotate(reverse, -angle.y, fvec3(cosf(angle.x), 0, -sinf(angle.x)));
        reverse = glm::rotate(reverse, -angle.x, fvec3(0, 1, 0));
        reverse = glm::translate(reverse, -center_pos);
    }
    reverse = glm::translate(reverse, -pos);
    fvec4 result(x, y, z, 1.0f);
    result = reverse * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_world_coordinates(fvec3* src, float x, float y, float z) {
    fmat4 view = glm::translate(fmat4(1), pos);
    if (can_rotate) {
        // round angle to nearest angle
        fvec2 rounded_angle = fvec2(roundf(angle.x * 2 / M_PI) * M_PI / 2,
                                    roundf(angle.y * 2 / M_PI) * M_PI / 2);
        view = glm::translate(view, center_pos);
        view = glm::rotate(view, rounded_angle.x, fvec3(0, 1, 0));
        view = glm::rotate(view, rounded_angle.y, fvec3(cosf(rounded_angle.x), 0, -sinf(rounded_angle.x)));
        view = glm::translate(view, -center_pos);
    }
    fvec4 result(x, y, z, 1.0f);
    result = view * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z) {
    fmat4 view = glm::translate(fmat4(1), pos);
    if (can_rotate) {
        view = glm::translate(view, center_pos);
        view = glm::rotate(view, angle.x, fvec3(0, 1, 0));
        view = glm::rotate(view, angle.y, fvec3(cosf(angle.x), 0, -sinf(angle.x)));
        view = glm::translate(view, -center_pos);
    }
    fvec4 result(x, y, z, 1.0f);
    result = view * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
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
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    move_dist(forward * get_speed(force));
}

void Entity::move_backward(float force) {
    fvec3 forward = -fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    move_dist(forward * get_speed(force));
}

void Entity::move_left(float force) {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    move_dist(fvec3(forward.z * get_speed(force),
                    0,
                    -forward.x * get_speed(force)));
}

void Entity::move_right(float force) {
    fvec3 forward = fvec3(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    move_dist(fvec3(-forward.z * get_speed(force),
                    0,
                    forward.x * get_speed(force)));
}

void Entity::move_up(float force) {
    move_dist(fvec3(0.0f,
                    get_speed(force),
                    0.0f));
}

void Entity::move_down(float force) {
    move_dist(fvec3(0.0f,
                    -get_speed(force),
                    0.0f));
}

void Entity::turn_left(float force) {
    // 1 unit of force turns 1 weight by M_PI/2
    turn_angle(fvec2(get_speed(force)*M_PI/2,
                     0.0f));
}

void Entity::turn_right(float force) {
    turn_angle(fvec2(-get_speed(force)*M_PI/2,
                     0.0f));
}

void Entity::move_dist(fvec3 off) {
    velocity += off;
    stable = false;
}

void Entity::turn_angle(fvec2 off) {
    angular_velocity += off;
    stable = false;
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
}

bool Entity::poke(float x, float y, float z) {
    // transform into OAC
    fvec3 oac;
    transform_into_my_coordinates(&oac, x, y, z);
    return bounds.hits(oac);
}

bool Entity::block_keyboard_callback(Game* game, Action key) {
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
    transform_into_world_coordinates(&lower_rwc, bounds.lower.x, bounds.lower.y, bounds.lower.z);
    transform_into_world_coordinates(&upper_rwc, bounds.upper.x, bounds.upper.y, bounds.upper.z);
    fvec3 before_lower_corner(std::min(lower_rwc.x, upper_rwc.x),
                              std::min(lower_rwc.y, upper_rwc.y),
                              std::min(lower_rwc.z, upper_rwc.z));
    fvec3 before_upper_corner(std::max(lower_rwc.x, upper_rwc.x),
                              std::max(lower_rwc.y, upper_rwc.y),
                              std::max(lower_rwc.z, upper_rwc.z));
    
    pos += velocity;
    if (can_rotate) {
        angle += angular_velocity;
        recalculate_dir();
    }
    
    transform_into_world_coordinates(&lower_rwc, bounds.lower.x, bounds.lower.y, bounds.lower.z);
    transform_into_world_coordinates(&upper_rwc, bounds.upper.x, bounds.upper.y, bounds.upper.z);
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

int Entity::load_self(IODataObject* obj) {
    up = obj->read_value<fvec3>();
    pos = obj->read_value<fvec3>();
    angle = obj->read_value<fvec2>();
    
    bounds = obj->read_value<bounding_box>();
    center_pos = obj->read_value<fvec3>();
    
    weight = obj->read_value<int>();
    health = obj->read_value<int>();
    
    recalculate_dir();
    return 0;
}

void Entity::remove_self(IODataObject* obj) {
    obj->save_value(up);
    obj->save_value(pos);
    obj->save_value(angle);
    
    obj->save_value(bounds);
    obj->save_value(center_pos);
    
    obj->save_value(weight);
    obj->save_value(health);
}


void test_entity_coordinate_system() {
    /*
    Entity test;
    
    test.pos = fvec3(1, 2, 3);
    // we are 2 by 3
    test.lower_bound = fvec3(0, 0, 0);
    test.upper_bound = fvec3(2, 1, 3);
    // centered at 1, 1.5 (middle!)
    test.center_pos = fvec3(1.0f, 0.5f, 1.5f);
    test.turn_angle(fvec2(1.57f, 0.0f));
    test.apply_rotation();

    fvec3 test_point = fvec3(2, 0, 0);
    fvec3 result;
    test.transform_into_world_coordinates(&result, test_point.x, test_point.y, test_point.z);
    printf("%f %f %f\n", result.x,result.y,result.z);
    test.transform_into_my_coordinates(&result, result.x, result.y, result.z);
    printf("%f %f %f\n", result.x,result.y,result.z);
    
    
    test_point = fvec3(1.0f, 0.5f, 1.5f);
    test.transform_into_world_coordinates(&result, test_point.x, test_point.y, test_point.z);
    printf("%f %f %f\n", result.x,result.y,result.z);
    test.transform_into_my_coordinates(&result, result.x, result.y, result.z);
    printf("%f %f %f\n", result.x,result.y,result.z);
    
    test_point = fvec3(1.0f, 0.0f, 2.0f);
    test.transform_into_world_coordinates(&result, test_point.x, test_point.y, test_point.z);
    printf("%f %f %f\n", result.x,result.y,result.z);
    test.transform_into_my_coordinates(&result, result.x, result.y, result.z);
    printf("%f %f %f\n", result.x,result.y,result.z);*/
}