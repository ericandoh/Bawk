//
//  entity.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity.h"
#include "superobject.h"
#include <glm/gtc/matrix_transform.hpp>

Entity::Entity() {
    // identifying info for the entity. this should be overwritten
    vid = 0;
    pid = 0;
    entity_class = EntityType::ENTITY;
    
    // start out with no parent
    // damn that's depresssing
    parent = 0;
    
    // this should be overridden as needed
    can_collide = true;
    can_rotate = false;
    
    // set initial pos/angle to origin, overwrite for most objects
    pos = fvec3(0,0,0);
    angle = Rotation();
    
    // set initial bounds to be maxed out
    bounds = bounding_box();
    center_pos = fvec3(0,0,0);
    
    recalculate_transform();
    
    // this object is (hopefully) not moving to start out with
    velocity = fvec3(0, 0, 0);
    angular_velocity = fvec3(0, 0, 0);
    velocity_decay = 0.5f;
    // do a stable check to begin with
    stable = false;
    
    // start with no weight
    weight = 0;
    // 0 is full health
    health = 0;
}

void Entity::transform_into_my_coordinates(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_my_mat * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_my_integer_coordinates(ivec3* src, int x, int y, int z) {
    fvec3 result;
    transform_into_my_coordinates(&result, x + 0.5f, y + 0.5f, z + 0.5f);
    src->x = floorf(result.x);
    src->y = floorf(result.y);
    src->z = floorf(result.z);
}

void Entity::transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_my_mat_smooth * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_world_coordinates(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_world_mat * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::transform_into_world_integer_coordinates(ivec3* src, int x, int y, int z) {
    fvec3 result;
    transform_into_world_coordinates(&result, x + 0.5f, y + 0.5f, z + 0.5f);
    src->x = floorf(result.x);
    src->y = floorf(result.y);
    src->z = floorf(result.z);
}

void Entity::transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_world_mat_smooth * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Entity::set_pos(fvec3 p) {
    pos = p;
    recalculate_transform();
}

void Entity::set_angle(Rotation a) {
    angle = a;
    recalculate_transform();
}

void Entity::recalculate_transform() {
    // into my coordinates
    into_my_mat = fmat4(1);
    if (can_rotate) {
        into_my_mat = glm::translate(into_my_mat, center_pos);
        angle.add_my_rotation_rounded(&into_my_mat);
        into_my_mat = glm::translate(into_my_mat, -center_pos);
    }
    into_my_mat = glm::translate(into_my_mat, -pos);
    
    into_my_mat_smooth = fmat4(1);
    if (can_rotate) {
        into_my_mat_smooth = glm::translate(into_my_mat_smooth, center_pos);
        angle.add_my_rotation(&into_my_mat_smooth);
        into_my_mat_smooth = glm::translate(into_my_mat_smooth, -center_pos);
    }
    into_my_mat_smooth = glm::translate(into_my_mat_smooth, -pos);

    // into world coordinates
    into_world_mat = glm::translate(fmat4(1), pos);
    if (can_rotate) {
        // round angle to nearest angle
        into_world_mat = glm::translate(into_world_mat, center_pos);
        angle.add_world_rotation_rounded(&into_world_mat);
        into_world_mat = glm::translate(into_world_mat, -center_pos);
    }
    
    // into world coordinates, smoothly (without rounded angles)
    into_world_mat_smooth = glm::translate(fmat4(1), pos);
    if (can_rotate) {
        into_world_mat_smooth = glm::translate(into_world_mat_smooth, center_pos);
        angle.add_world_rotation(&into_world_mat_smooth);
        into_world_mat_smooth = glm::translate(into_world_mat_smooth, -center_pos);
    }
    
    if (parent) {
        // apply parent matrices
        into_my_mat = into_my_mat * parent->into_my_mat;
        into_my_mat_smooth = into_my_mat_smooth * parent->into_my_mat_smooth;
        into_world_mat = parent->into_world_mat * into_world_mat;
        into_world_mat_smooth = parent->into_world_mat_smooth * into_world_mat_smooth;
    }
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
    move_dist(angle.dir * get_speed(force));
}

void Entity::move_backward(float force) {
    move_dist(angle.dir * (-get_speed(force)));
}

void Entity::move_forward_flat(float force) {
    // override this if flying/on land
    //fvec3 forward = fvec3(dir.x, 0, dir.z);
    //forward = glm::normalize(forward);
    move_dist(angle.forward * get_speed(force));
}

void Entity::move_backward_flat(float force) {
    //fvec3 forward = -fvec3(dir.x, 0, dir.z);
    //forward = glm::normalize(forward);
    move_dist(angle.forward * (-get_speed(force)));
}

void Entity::move_left(float force) {
    //fvec3 forward = fvec3(dir.x, 0, dir.z);
    //forward = glm::normalize(forward);
    move_dist(fvec3(angle.forward.z * get_speed(force),
                    0,
                    -angle.forward.x * get_speed(force)));
}

void Entity::move_right(float force) {
    //fvec3 forward = fvec3(dir.x, 0, dir.z);
    //forward = glm::normalize(forward);
    move_dist(fvec3(-angle.forward.z * get_speed(force),
                    0,
                    angle.forward.x * get_speed(force)));
}

void Entity::move_up(float force) {
    move_dist(get_speed(force)*fvec3(angle.up.x,
                                     angle.up.y,
                                     angle.up.z));
}

void Entity::move_down(float force) {
    move_dist(-get_speed(force)*fvec3(angle.up.x,
                                      angle.up.y,
                                      angle.up.z));
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
    printf("\n");
    print_entity_type(entity_class);
    printf(" (%d,%d) of class %d", pid, vid, entity_class);
    printf("\nPos: ");
    printf_fvec3(pos);
    printf("\nDir: ");
    printf_fvec3(angle.dir);
    printf("\nCenter: ");
    printf_fvec3(center_pos);
    printf("\nBounds:\n");
    printf_fvec3(bounds.lower);
    printf("\n");
    printf_fvec3(bounds.upper);
    printf("\n\n");
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

bool Entity::break_block(float x, float y, float z) {
    // behaviour for destroying a block on a regular entity is undefined
    // to remove an entity entirely use other pathways
    return false;
}

void Entity::get_hurt(float x, float y, float z, float dmg) {
    if (poke(x,y,z) != this) {
        return;
    }
    // i have no resistance. take dmg directly to health
    health += dmg;
    if (health >= MAX_HEALTH) {
        // become disabled, but don't actually remove
        printf("I AM DEFEATED\n");
        /*if (parent) {
            ((SuperObject*)parent)->remove_entity(this);
        }*/
    }
}

bool Entity::block_keyboard_callback(Game* game, Action key, Entity* ent) {
    return false;
}

bool Entity::block_mouse_callback(Game* game, Action button, Entity* ent) {
    return false;
}

void Entity::step(Game* game) {
    // do nothing
}

void Entity::get_mvp(fmat4 *dst) {
    *dst = into_world_mat_smooth;
}

void Entity::render(fmat4* transform) {
    // do nothing
}

void Entity::render_lights(fmat4* transform, fvec3 player_pos) {
    // do nothing
}

void Entity::update_chunks(fvec3* start_pos) {
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
        angle.apply_angles(angular_velocity);
    }
    recalculate_transform();
    
    transform_into_world_coordinates(&lower_rwc, bounds.lower.x, bounds.lower.y, bounds.lower.z);
    transform_into_world_coordinates(&upper_rwc, bounds.upper.x, bounds.upper.y, bounds.upper.z);
    fvec3 after_lower_corner(std::min(lower_rwc.x, upper_rwc.x),
                             std::min(lower_rwc.y, upper_rwc.y),
                             std::min(lower_rwc.z, upper_rwc.z));
    fvec3 after_upper_corner(std::max(lower_rwc.x, upper_rwc.x),
                             std::max(lower_rwc.y, upper_rwc.y),
                             std::max(lower_rwc.z, upper_rwc.z));
    
    moving_bounds.lower = fvec3(std::min(before_lower_corner.x, after_lower_corner.x),
                                std::min(before_lower_corner.y, after_lower_corner.y),
                                std::min(before_lower_corner.z, after_lower_corner.z));
    moving_bounds.upper = fvec3(std::max(before_upper_corner.x, after_upper_corner.x),
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
    if (my_bounds->hits(*other_bounds)) {
        // printf("collision detected\n");
        return true;
    }
    return false;
}

void Entity::after_collision(Game* game) {
    // do nothing
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
    //up = obj->read_value<fvec3>();
    pos = obj->read_value<fvec3>();
    angle = obj->read_value<Rotation>();
    
    bounds = obj->read_value<bounding_box>();
    center_pos = obj->read_value<fvec3>();
    
    weight = obj->read_value<int>();
    health = obj->read_value<int>();
    
    recalculate_transform();
    return 0;
}

void Entity::remove_self(IODataObject* obj) {
    //obj->save_value(up);
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