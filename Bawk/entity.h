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

#define MAX_HEALTH 1024

#include <stdio.h>
#include <string>
#include "basic_types.h"
#include "positionable.h"
#include "block_loader.h"
#include "entity_type.h"
#include "blockaction.h"

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
    // BB of entire moving object in RWC
    bounding_box moving_bounds;
    
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
    // parent entity, if any
    // entity must be someone capable of being a parent
    SuperObject* parent;
    
    Entity();
    
    // --- Positionable
    virtual void recalculate_transform() override;
    
    // --- DEBUG ---
    void print_debug();
    
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
    virtual bool block_keyboard_callback(Game* game, Action key, Entity* ent);
    virtual bool block_mouse_callback(Game* game, Action button, Entity* ent);
    virtual void step(Game* game) EMPTY_FUNCTION;
    
    // update how this is rendered depending on player position
    virtual void update_render(fvec3* player_pos) EMPTY_FUNCTION;
    
    // --- MORE COLLISION DETECTION ---
    // calculate the moving bounding box for this frame
    virtual void calculate_moving_bounding_box();
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
    virtual void save_selfs();
    virtual std::string get_save_path();
    virtual int load_self(IODataObject* obj);
    virtual void save_self(IODataObject* obj);
    
    Entity* find_top_level_parent();
    void remove_self();
};

#endif /* defined(__Bawk__entity__) */
