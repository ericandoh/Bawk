//
//  A superobject represents an entity of blocks that will be saved/loaded ingame
//  -
//  -
//  -
//
//  Used by:
//  -
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
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__Superobject__
#define __Bawk__Superobject__

#include <stdio.h>
#include <string>
#include <map>
#include <unordered_map>
#include "superobjectrender.h"
#include "blockaction.h"

struct key_mapping_info {
    ivec3 position;
    block_type blk;
    block_callback_func func;
};

class Player;

class SuperObject : public RenderableSuperObject {
protected:
    // note that we might have >1 action per key press (ie. both engines X, Y fire on pressing W)
    std::map<Action, std::vector<key_mapping_info>> key_mapping;
    // the reverse mapping of the above, for convenience
    std::unordered_map<ivec3, std::vector<Action>> reverse_key_mapping;
    std::unordered_map<ivec3, block_type> pos_to_active_block_mapping;
    int block_counter;
public:
    // supersuperobject things
    std::vector<Entity*> entities;
    SuperObject();
    SuperObject(uint32_t p, uint32_t v);
    
    // --- SuperObject ---
    virtual void add_entity(Entity* entity);
    virtual void remove_entity(Entity* entity);
    virtual std::string get_chunk_save_path(ivec3* pos);
    void copy_into(Player* player, SuperObject* target);
    
    // if we don't sort the x/y/z position list every time we can recycle that list to do this more efficiently (log time)
    void get_entities_in_range(std::vector<Entity*> &children, bounding_box box, bool include_boundary=false);
    // rough check to see if a bounding box hits any bounds of any chunks currently loaded in
    bool hits_chunk_bounds_in_range(bounding_box box);
    void check_collision_detection_children(Game* game);
    
    // --- RenderableSuperObject ---
    virtual void handle_block_addition(int x, int y, int z, block_type type) override;
    virtual void handle_block_removal(int x, int y, int z, block_type type) override;
    virtual RenderableChunk* get_chunk(int x, int y, int z) override;
    virtual int save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) override;
    
    // --- Entity ---
    void update_centerpos() override;
    Entity* poke(float x, float y, float z) override;
    bool block_keyboard_callback(Game* game, Action key, Entity* ent, int ms) override;
    bool block_mouse_callback(Game* game, Action button, Entity* ent) override;
    virtual void step(Game* game, int ms) override;
    virtual void render(fmat4* transform) override;
    void render_lights(fmat4* transform, fvec3 player_pos) override;
    virtual void update_render(fvec3* player_pos) override;
    
    bool collides_with(Entity* other) override;
    void step_motion(fvec3* prev_pos, Rotation* prev_rot, bounding_box* moving_bounds) override;
    int get_collision_level() override;
    // method for collision detection against base class entities ONLY
    bool collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level) override;

    virtual std::string get_save_path() override;

    int load_self(IODataObject* obj) override;
    void save_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__Superobject__) */
