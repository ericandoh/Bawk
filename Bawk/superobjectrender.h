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

namespace std
{
    template<>
    struct hash<ivec3> {
        size_t operator()(const ivec3 &pt) const {
            // turn first 9 bits to a 27 bit string, then add together
            // the result should be XYZXYZXYZXYZ for XXXX, YYYY, ZZZZ (but with more bits)
            uint32_t xb = spread_bits(pt.x);
            uint32_t yb = spread_bits(pt.y) << 1;
            uint32_t zb = spread_bits(pt.z) << 2;
            return xb + yb + zb;
        }
    };
}

struct chunk_bounds {
    ivec3 lower_bound, upper_bound;
};

typedef std::unordered_map<ivec3, RenderableChunk*> chunk_map;
typedef std::unordered_map<ivec3, chunk_bounds> chunk_bound_map;

class RenderableSuperObject {
    // internal function that will make calls to load and save a chunk given CAC xyz
    int load_chunk(int x, int y, int z);
    void delete_chunk(int x, int y, int z);
protected:
    // mapping from CAC position to chunk for all chunks currently in memory
    // the actual superobject may have more chunks, but those will be loaded from disk if necessary
    chunk_map chunks;
    // mapping from CAC position to the actual physical bound of that chunk, for ALL chunks
    chunk_bound_map chunk_bounds;
    // position of the superobject, in RWC
    fvec3 pos;
    // the bounding box over contents of all chunks, in OAC
    ivec3 lower_bound, upper_bound;
    // internal function to transform RWC xyz to OAC src
    void transform_into_my_coordinates(ivec3* src, float x, float y, float z);
    // internal function to transform OAC xyz to RWC src
    void transform_into_world_coordinates(fvec3* src, int x, int y, int z);
    // internal function to transform OAC xyz to CAC cac, CRC crc
    void transform_into_chunk_bounds(ivec3* cac, ivec3* crc, int x, int y, int z);
public:
    // makes a renderable super object that is presumed to be empty initially
    RenderableSuperObject();
    // make a renderable super object with some specified bounds (OAC) and position (RWC)
    // TODO call this at initialization of the game, for when loading in existing objects
    RenderableSuperObject(fvec3 p);
    // cleans up the chunk data held by this object.
    // TODO investigate if this needs overriding
    virtual void remove_self();
    // gets the block at (RWC) xyz
    uint16_t get_block(float x, float y, float z);
    // sets the block at (RWC) xyz
    void set_block(float x, float y, float z, uint16_t type);
    // tries to set my position to (RWC) to_pos, return false if you can't
    bool set_position(fvec3 to_pos);
    // renders the object, given a player viewpoint transform matrix
    void render(fmat4* transform);
    // makes calls to load in/free chunks depending on the
    // former and new (RWC) coordinates of the player
    virtual void update_chunks(fvec3* old_pos, fvec3* new_pos);
    // this should be overriden to provide chunk data at (CAC) xyz
    virtual int get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) = 0;
    // this should be overriden to save chunk data at (CAC) xyz
    virtual int save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) = 0;
    
    // check if a chunk coordinate (CAC) xyz is a chunk held by this object
    virtual bool within_dimensions_chunk(int x, int y, int z) = 0;
    // updates the dimensions of my object, given a chunk at chunk_pos (CAC) is updated
    // call when initializing from scratch with a lot of blocks, or when removing a block at a periphery
    virtual void update_dimensions_from_chunk(ivec3 chunk_pos);
    // check if a bounding box (OAC) (lower, upper) intersects with my own bounds
    // override this for baseworld
    virtual bool intersects_with_my_bounds(ivec3 lower_corner, ivec3 upper_corner);
    // checks if this superobject collides with this other superobject
    // other superobject should NEVER be the baseworld (or other dimensionless entity)
    // to check against dimensionless entities like that, set the dimensionless as the callee
    bool collides_with(RenderableSuperObject* other);
};

#endif /* defined(__Bawk__superobjectrender__) */