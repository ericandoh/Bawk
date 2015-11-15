//
//  The basic building block for all tangible objects in our universe
//  - Has a position, and can move around if permitted
//  - Can rotate on both axises
//  - Has some bounds, and other objects can't share space within its bounds
//
//  Used by:
//  - A lot of shit
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__entity__
#define __Bawk__entity__

#include <stdio.h>
#include <string>
#include "basic_types.h"
#include "block_loader.h"
#include "game_actions.h"
#include "rotation.h"

class Game;

class Entity {
protected:
    // up/dir vectors, derived from angle.x/angle.y
    float get_speed(float force);
    void get_mvp(fmat4* dst);
public:
    // ----- IDENTIFYING INFO -----
    // identifying information for the entity
    uint32_t vid;
    uint32_t pid;
    
    // used by loader to see how to construct the entity...
    int entity_class;
    
    // some permissions/flags
    bool can_collide;
    bool can_rotate;
    
    // ----- POSITIONING INFO -----
    // position of the superobject, in RWC
    fvec3 pos;
    
    // angle around dir
    Rotation angle;
    
    // the bounding box over contents of all chunks, in OAC
    // public for convenience of access
    bounding_box bounds;
    // the center position of this entity. usually set to the middle point of the bounding box
    fvec3 center_pos;
    
    // temp variable that tells me my speed at a certain point in time
    fvec3 velocity;
    // temp variable that tells me my angular speed at a certain point in time
    fvec3 angular_velocity;
    
    // if the object has velocity/angular velocity, mark this so collision detector picks it up
    bool stable;
    
    // ----- MISC INFO -----
    // weight of the entity
    int weight;
    // height of the entity. 0 is full health (alive!)
    int health;
    
    // --- TEMP ---
    fvec3 moving_lower, moving_upper;
    
    Entity();
    // internal function to transform RWC xyz to OAC src
    void transform_into_my_coordinates(fvec3* src, float x, float y, float z);
    // do this smoothly
    void transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z);
    // internal function to transform OAC xyz to RWC src
    void transform_into_world_coordinates(fvec3* src, float x, float y, float z);
    void transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z);
    
    // movement methods
    void move_forward(float force);
    void move_backward(float force);
    void move_forward_flat(float force);
    void move_backward_flat(float force);
    void move_left(float force);
    void move_right(float force);
    void move_up(float force);
    void move_down(float force);
    
    void yaw_left(float force);
    void yaw_right(float force);
    void pitch_up(float force);
    void pitch_down(float force);
    void roll_left(float force);
    void roll_right(float force);
    
    void move_dist(fvec3 off);
    void turn_angle(fvec3 off);
    
    //virtual void recalculate_dir();
    
    // poke this object at RWC, true if this object says, "OUCH"
    virtual bool poke(float x, float y, float z);
    
    virtual bool block_keyboard_callback(Game* game, Action key);
    virtual bool block_mouse_callback(Game* game, int button);
    
    virtual void step();
    virtual void render(fmat4* transform);
    virtual void update_chunks(fvec3* old_pos, fvec3* new_pos);

    // this is in RWC!!!!!!!!!!
    void calculate_moving_bounding_box();
    
    // for more exact collision detection
    virtual bool collides_with(Entity* other);
    // override this with your own int for up to what class of Entities you can handle collision
    // detection for
    virtual int get_collision_level();
    // method for collision detection against base class entities ONLY
    virtual bool collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level);
    
    virtual int load_selfs();
    virtual void remove_selfs();
    virtual std::string get_save_path();
    virtual int load_self(IODataObject* obj);
    virtual void remove_self(IODataObject* obj);
};

void test_entity_coordinate_system();

#endif /* defined(__Bawk__entity__) */
