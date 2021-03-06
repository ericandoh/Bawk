//
//  superobjectrender.h
//  Bawk
//
//  Some definitions:
//  -RWC: (float) real world coordinates, the base coordinate system
//  -CAC: (int) chunk aligned coordinates. this is usually used to refer to chunk locations
//        within in a superobject. So a distance of 1 in CAC along the x-axis will equal CX in RWC
//  -CRC: (uint8_t) chunk relative coordinates.
//        Ranges from 0-CX, 0-CY, 0-CZ and used to refer to positions within an individual chunk
//  -OAC: (int) object aligned coordinates. coordinates aligned along this object's XYZ
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__superobjectrender__
#define __Bawk__superobjectrender__

#include <stdio.h>
#include <unordered_map>
#include "basic_types.h"
#include "chunkrender.h"
#include "entity.h"
#include "block.h"

struct chunk_bounds {
    ivec3 lower_bound, upper_bound;
};

typedef std::unordered_map<ivec3, RenderableChunk*> chunk_map;
typedef std::unordered_map<ivec3, chunk_bounds> chunk_bound_map;

class RenderableSuperObject: public Entity {
protected:
    // mapping from CAC position to the actual physical bound of that chunk, for ALL chunks
    chunk_bound_map chunk_bounds;
    
    // internal function that will make calls to load and save a chunk given CAC xyz
    int load_chunk(int x, int y, int z);
public:
    // mapping from CAC position to chunk for all chunks currently in memory
    // the actual superobject may have more chunks, but those will be loaded from disk if necessary
    chunk_map chunks;
    
    // makes a renderable super object that is presumed to be empty initially
    RenderableSuperObject();
    
    // --- RenderableSuperObject ---
    void setup_chunk(RenderableChunk* chunk, int x, int y, int z);
    // internal function to transform OAC xyz to CAC cac, CRC crc
    void transform_into_chunk_bounds(ivec3* cac, ivec3* crc, float x, float y, float z);
    // creates an empty chunk at x, y, z
    void create_chunk(int x, int y, int z);
    void delete_chunk(int x, int y, int z);
    
    // gets the block at (RWC) xyz
    block_type* get_block(float x, float y, float z);
    block_type* get_block_integral(int x, int y, int z);
    // sets the block at (RWC) xyz
    void set_block(float x, float y, float z, block_type type);
    void set_block_integral(int x, int y, int z, block_type type);
    // helper function to do appropriate action when some block is added
    virtual void handle_block_addition(int x, int y, int z, block_type type) = 0;
    // helper function to do appropriate action when some block is removed
    virtual void handle_block_removal(int x, int y, int z, block_type type) = 0;
    
    // this should be overriden to provide chunk data at (CAC) xyz, if the chunk doesnt exist return NULL
    virtual RenderableChunk* get_chunk(int x, int y, int z) = 0;
    // this should be overriden to save chunk data at (CAC) xyz
    virtual int save_chunk(RenderableChunk* chunk, int x, int y, int z) = 0;
    
    // check if a chunk coordinate (CAC) xyz is a chunk held by this object
    virtual bool within_dimensions_chunk(int x, int y, int z);
    virtual bool intersects_chunk(ivec3 lower, ivec3 upper, ivec3 chunkpos);
    // updates the dimensions of my object, given a chunk at chunk_pos (CAC) is updated
    // call when initializing from scratch with a lot of blocks, or when removing a block at a periphery
    virtual void update_dimensions_from_chunk(ivec3 chunk_pos);

    // --- Entity ---
    virtual Entity* poke(float x, float y, float z) override;
    bool get_hurt(float x, float y, float z, float dmg, BreakablePolicy policy, Player* player) override;
    virtual void step(int ms) override;
    virtual void render(fmat4* transform) override;
    virtual void render_lights(fmat4* transform, fvec3 player_pos) override;
    virtual void update_render(fvec3* player_pos) override;
    void update_chunks_async(fvec3* player_pos);
    
    virtual int get_collision_level() override;
    virtual bool collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level) override;
    
    virtual int load_self(IODataObject* obj) override;
    virtual void save_self(IODataObject* obj) override;
    
    
    void save_all_chunks();
};

#endif /* defined(__Bawk__superobjectrender__) */
