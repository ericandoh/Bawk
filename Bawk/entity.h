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
#include "positionable.h"
#include "block_loader.h"
#include "entity_type.h"
#include "blockaction.h"

// some special vid (vehicle IDs)
#define VID_TEMPORARY 1
#define VID_PERMANENT 2
#define VID_UNIQUE_START 3

class Game;
class SuperObject;

enum BreakablePolicy {
    ACTIONED, EXECUTED
};

class Entity: public Positionable {
protected:
    // up/dir vectors, derived from angle.x/angle.y
    float get_speed(float force);
public:
    // --- INFO ---
    // identifying information for the entity
    uint32_t vid;
    uint32_t pid;
    
    // used by loader to see how to construct the entity...
    EntityType entity_class;

    // --- MOTION ---
    // whether to enable collision detection for this entity
    bool can_collide;
    // flag set to TRUE when object moves
    bool stable;
    // BB of entire object in OAC
    bounding_box bounds;
    
    // temp variable that tells me my speed at a certain point in time
    fvec3 velocity;
    // temp variable that tells me my angular speed at a certain point in time
    fvec3 angular_velocity;
    // how fast my velocity decays (think of it as friction)
    float velocity_decay;
    
    // ----- PROPERTIES -----
    // weight of the entity
    int weight;
    // height of the entity. 0 is full health (alive!)
    int health;
    // if this is selected, modify how we render it
    bool selected;
    // parent entity, if any
    // entity must be someone capable of being a parent
    SuperObject* parent;
    
    Entity();
    
    // --- Positionable
    // these functions change behaviour now with a parent in the equation
    void transform_into_my_coordinates(fvec3* src, float x, float y, float z) override;
    void transform_into_my_integer_coordinates(ivec3* src, int x, int y, int z) override;
    void transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z) override;
    void transform_into_world_coordinates(fvec3* src, float x, float y, float z) override;
    void transform_into_world_integer_coordinates(ivec3* src, int x, int y, int z) override;
    void transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z) override;
    
    void transform_into_my_rotation(Rotation* dst, Rotation target) override;
    void transform_into_world_rotation(Rotation* dst, Rotation target) override;
    
    fvec3 get_world_pos() override;
    Rotation get_world_rotation() override;
    // this should be called internally by SuperObject::add_entity
    // do not call directly
    void set_parent(SuperObject* parent);
    
    // --- DEBUG ---
    void print_debug();
    
    // --- POSITION ---
    void set_bounds(bounding_box bounds);
    virtual void update_centerpos();
    void update_centerpos(bounding_box bounds);
    void update_centerpos_smooth();
    void set_centerpos(fvec3 cp);
    
    // --- MOTION ---
    void move_forward(float force);
    void move_backward(float force);
    void move_forward_flat(float force);
    void move_backward_flat(float force);
    void move_left(float force);
    void move_right(float force);
    void move_up(float force);
    void move_down(float force);
    void move_up_flat(float force);
    void move_down_flat(float force);
    
    void yaw_left(float force);
    void yaw_right(float force);
    void pitch_up(float force);
    void pitch_down(float force);
    void roll_left(float force);
    void roll_right(float force);
    
    void move_dist(fvec3 off);
    void turn_angle(fvec3 off);
    
    // --- COLLISION ---
    // poke this object at RWC, true if this object says, "OUCH"
    virtual Entity* poke(float x, float y, float z);
    bool can_be_hurt(BreakablePolicy policy, uint32_t pid);
    bool can_be_destroyed(BreakablePolicy policy, uint32_t pid);
    float calculate_damage(float dmg, float resistance);
    // hit this object with some damage
    virtual bool get_hurt(float x, float y, float z, float dmg, BreakablePolicy policy=ACTIONED, uint32_t pid=0);
    
    // --- ACTION ---
    // callback methods for when actions are called on this item
    virtual bool block_keyboard_callback(Game* game, Action key, Entity* ent, int ms) EMPTY_BOOL_FUNCTION;
    virtual bool block_mouse_callback(Game* game, Action button, Entity* ent) EMPTY_BOOL_FUNCTION;
    virtual void step(Game* game, int ms) EMPTY_FUNCTION;
    
    // update how this is rendered depending on player position
    virtual void update_render(fvec3* player_pos) EMPTY_FUNCTION;
    
    // --- MORE COLLISION DETECTION ---
    void add_bounds_to_box(bounding_box* moving_bounds);
    // calculate the moving bounding box for this frame
    virtual void step_motion(fvec3* prev_pos, Rotation* prev_rot, bounding_box* moving_bounds);
    // for more exact collision detection
    virtual bool collides_with(Entity* other);
    // override this with your own int for up to what class of Entities you can handle collision
    // detection for
    virtual int get_collision_level();
    // method for collision detection against base class entities ONLY
    virtual bool collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level);
    // behaviour to do after a collision
    virtual bool after_collision(Game* game);
    
    // --- FILE SAVE ---
    virtual int load_selfs();
    int load_selfs(std::string path);
    virtual void save_selfs();
    virtual std::string get_save_path();
    virtual int load_self(IODataObject* obj);
    virtual void save_self(IODataObject* obj);
    
    Entity* find_top_level_parent();
    void remove_self();
};

#endif /* defined(__Bawk__entity__) */
