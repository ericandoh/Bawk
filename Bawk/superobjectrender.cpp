//
//  superobjectrender.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <climits>
#include <glm/gtc/matrix_transform.hpp>
#include "worldrender.h"
#include "superobjectrender.h"

RenderableSuperObject::RenderableSuperObject(): Entity() {
    pos = fvec3(0.0, 0.0, 0.0);
    // TODO recalculate bound here, or load bound from memory
}

RenderableSuperObject::RenderableSuperObject(fvec3 p): Entity() {
    pos = p;
    // TODO recalculate bound here, or load bound from memory
}

void RenderableSuperObject::transform_into_chunk_bounds(ivec3* cac,
                                                        ivec3* crc,
                                                        float x, float y, float z) {
    int xr = int(floorf(x));
    int yr = int(floorf(y));
    int zr = int(floorf(z));
    // chunk relative coordinates
    crc->x = (xr % CX + CX) % CX;
    crc->y = (yr % CY + CY) % CY;
    crc->z = (zr % CZ + CZ) % CZ;
    
    // chunk aligned coordinates
    cac->x = (xr - crc->x) / CX;
    cac->y = (yr - crc->y) / CY;
    cac->z = (zr - crc->z) / CZ;
}

void RenderableSuperObject::remove_self() {
    int counter = 0;
    for (auto kv : chunks) {
        counter++;
        save_chunk(kv.second->blk, kv.first.x, kv.first.y, kv.first.z);
        kv.second->cleanup();
    }
    printf("Saved %d chunks\n", counter);
}

int RenderableSuperObject::load_chunk(int x, int y, int z) {
    if (chunks.count(ivec3(x, y, z))) {
        // chunk is already loaded
        return 0;
    }
    
    //printf("Loading chunk for %d %d %d\n", x, y, z);
    block_type raw_chunk[CX][CY][CZ];
    if (get_chunk(raw_chunk, x, y, z)) {
        // failed to load chunk
        return 1;
    }
    RenderableChunk* chunk = new RenderableChunk(raw_chunk);
    chunks[ivec3(x, y, z)] = chunk;
    // we load data from disk so this should be consistent, but just to be safe do a check
    update_dimensions_from_chunk(ivec3(x, y, z));
    
    ivec3 left(x - 1, y, z);
    ivec3 right(x + 1, y, z);
    ivec3 below(x, y - 1, z);
    ivec3 above(x, y + 1, z);
    ivec3 front(x, y, z - 1);
    ivec3 back(x, y, z + 1);

    if (chunks.count(left)) {
        chunk->left = chunks[left];
        chunk->left->changed = true;
    }
    if (chunks.count(right)) {
        chunk->right = chunks[right];
        chunk->right->changed = true;
    }
    if (chunks.count(below)) {
        chunk->below = chunks[below];
        chunk->below->changed = true;
    }
    if (chunks.count(above)) {
        chunk->above = chunks[above];
        chunk->above->changed = true;
    }
    if (chunks.count(front)) {
        chunk->front = chunks[front];
        chunk->front->changed = true;
    }
    if (chunks.count(back)) {
        chunk->back = chunks[back];
        chunk->back->changed = true;
    }
    
    return 0;
}

void RenderableSuperObject::delete_chunk(int x, int y, int z) {
    ivec3 pos(x, y, z);
    if (!chunks.count(pos)) {
        // this chunk is already gone mate
        return;
    }
    // this will call destructor which will (hopefully) cleanly erase/detach the chunk!
    chunks[pos]->cleanup();
    chunks.erase(pos);
}

block_type RenderableSuperObject::get_block(float x, float y, float z) {
    
    // first transform to OAC
    fvec3 oac;
    transform_into_my_coordinates(&oac, x, y, z);
    // now transform into cac, crc
    ivec3 cac, crc;
    transform_into_chunk_bounds(&cac, &crc, oac.x, oac.y, oac.z);
   
    if (!within_dimensions_chunk(cac.x, cac.y, cac.z)) {
        // there's no block outside the dimensions of the object
        return block_type();
    }
    
    // try loading the chunk (if it doesn't exist) then get block
    if (!load_chunk(cac.x, cac.y, cac.z)) {
        return chunks[cac]->get(crc.x, crc.y, crc.z);
    }
    // loading the chunk failed...return air
    return block_type();
}

void RenderableSuperObject::set_block(float x, float y, float z, block_type type) {
    // first transform to OAC
    fvec3 oac;
    transform_into_my_coordinates(&oac, x, y, z);
    // now transform into cac, crc
    ivec3 cac, crc;
    transform_into_chunk_bounds(&cac, &crc, oac.x, oac.y, oac.z);
    
    if (type.type == 0 && !within_dimensions_chunk(cac.x, cac.y, cac.z)) {
        // no point setting a block to 0 outside chunk bounds
        return;
    }
    
    // try loading the chunk (if it doesn't exist) then set block
    if (!load_chunk(cac.x, cac.y, cac.z)) {
        chunks[cac]->set(crc.x, crc.y, crc.z, type);
        // update the dimension vectors
        update_dimensions_from_chunk(cac);
    }
    // loading the chunk failed, probably because this is out of bounds
}

void RenderableSuperObject::render(fmat4* transform) {
    for (auto iterator = chunks.begin(); iterator != chunks.end(); iterator++) {
        ivec3 sub_pos = iterator->first;
        
        fvec3 chunk_rwc;
        transform_into_world_coordinates(&chunk_rwc, sub_pos.x * CX,
                                                     sub_pos.y * CY,
                                                     sub_pos.z * CZ);
        fmat4 model = glm::translate(fmat4(1.0f),
                                     chunk_rwc);
        fmat4 mvp = *transform * model;
        
        // Is this chunk on the screen?
        glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
        
        //float d = glm::length(center);
        center.x /= center.w;
        center.y /= center.w;
        
        // If it is behind the camera, don't bother drawing it
        if(center.z < -CY / 2)
            continue;
        
        // If it is outside the screen, don't bother drawing it
        if(fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
            continue;
        
        set_transform_matrix(mvp);
        iterator->second->render();
    }
}

// helper function, sees if the absolute distance is less than the chunk render distance
bool is_within_range(ivec3* chunk_pos, int x, int y, int z) {
    return abs(chunk_pos->x - x) <= CHUNK_RENDER_DIST &&
            abs(chunk_pos->y - y) <= CHUNK_RENDER_DIST &&
            abs(chunk_pos->z - z) <= CHUNK_RENDER_DIST;
}

void RenderableSuperObject::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    // get player position in this object's axis
    fvec3 new_oac;
    ivec3 old_cac, new_cac;
    transform_into_my_coordinates(&new_oac, new_pos->x, new_pos->y, new_pos->z);
    // now transform into cac, crc. we will only need the CAC coordinates
    ivec3 crc;
    transform_into_chunk_bounds(&new_cac, &crc, new_oac.x, new_oac.y, new_oac.z);
    
    int xmin, xmax, ymin, ymax, zmin, zmax;
    bool check_old = true;
    if (old_pos) {
        // the old position tells us where we have already loaded the chunks
        // by doing this check we can stop us from loading chunks we already have!
        fvec3 old_oac;
        transform_into_my_coordinates(&old_oac, old_pos->x, old_pos->y, old_pos->z);
        // now transform into cac, crc. we will only need the CAC coordinates
        ivec3 crc;
        transform_into_chunk_bounds(&old_cac, &crc, old_oac.x, old_oac.y, old_oac.z);
        
        if (new_cac == old_cac) {
            return;
        }
        
        xmin = imin(old_cac.x, new_cac.x) - CHUNK_RENDER_DIST;
        xmax = imax(old_cac.x, new_cac.x) + CHUNK_RENDER_DIST + 1;
        ymin = imin(old_cac.y, new_cac.y) - CHUNK_RENDER_DIST;
        ymax = imax(old_cac.y, new_cac.y) + CHUNK_RENDER_DIST + 1;
        zmin = imin(old_cac.z, new_cac.z) - CHUNK_RENDER_DIST;
        zmax = imax(old_cac.z, new_cac.z) + CHUNK_RENDER_DIST + 1;
    }
    else {
        // if this chunk has never been updated before, all chunks must be new
        xmin = new_cac.x - CHUNK_RENDER_DIST;
        xmax = new_cac.x + CHUNK_RENDER_DIST + 1;
        ymin = new_cac.y - CHUNK_RENDER_DIST;
        ymax = new_cac.y + CHUNK_RENDER_DIST + 1;
        zmin = new_cac.z - CHUNK_RENDER_DIST;
        zmax = new_cac.z + CHUNK_RENDER_DIST + 1;
        check_old = false;
    }

    bool within_old, within_new;
    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            for (int z = zmin; z < zmax; z++) {
                if (!within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                within_old = check_old && is_within_range(&old_cac, x, y, z);
                within_new = is_within_range(&new_cac, x, y, z);
                if (within_old && !within_new) {
                    delete_chunk(x, y, z);
                }
                else if (!within_old && within_new) {
                    load_chunk(x, y, z);
                }
            }
        }
    }
}

void RenderableSuperObject::update_dimensions_from_chunk(ivec3 chunk_pos) {
    if (!chunks.count(chunk_pos))
        return;
    // these are the new bounds, they should be updated
    ivec3 chunk_lower = chunks[chunk_pos]->lower_bound;
    ivec3 chunk_upper = chunks[chunk_pos]->upper_bound;
    bool recalculate_needed = false;
    if (chunk_bounds.count(chunk_pos)) {
        ivec3 old_chunk_lower = chunk_bounds[chunk_pos].lower_bound;
        ivec3 old_chunk_upper = chunk_bounds[chunk_pos].upper_bound;
        // we need to recalculate if any of our total bounds depended on this chunk, or seemed to
        recalculate_needed = int(lower_bound.x) == (old_chunk_lower.x + CX*chunk_pos.x)  ||
                            int(lower_bound.y) == (old_chunk_lower.y + CY*chunk_pos.y) ||
                            int(lower_bound.z) == (old_chunk_lower.z + CZ*chunk_pos.z) ||
                            lower_bound.x == FLT_MAX;
        recalculate_needed = recalculate_needed ||
                            int(upper_bound.x) == (old_chunk_upper.x + CX*chunk_pos.x)  ||
                            int(upper_bound.y) == (old_chunk_upper.y + CY*chunk_pos.y) ||
                            int(upper_bound.z) == (old_chunk_upper.z + CZ*chunk_pos.z) ||
                            upper_bound.x == -FLT_MAX;
    }
    else {
        recalculate_needed = true;
    }
    chunk_bounds[chunk_pos] = {chunk_lower, chunk_upper};
    
    // iterate through all our chunks and update dimensions
    if (recalculate_needed) {
        lower_bound = fvec3(FLT_MAX, FLT_MAX, FLT_MAX);
        upper_bound = fvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto &i : chunk_bounds) {
            ivec3 cpos = i.first;
            struct chunk_bounds bounds = i.second;
            lower_bound.x = fminf(float(bounds.lower_bound.x + CX*cpos.x), lower_bound.x);
            lower_bound.y = fminf(float(bounds.lower_bound.y + CY*cpos.y), lower_bound.y);
            lower_bound.z = fminf(float(bounds.lower_bound.z + CZ*cpos.z), lower_bound.z);
            upper_bound.x = fmaxf(float(bounds.upper_bound.x + CX*cpos.x), upper_bound.x);
            upper_bound.y = fmaxf(float(bounds.upper_bound.y + CY*cpos.y), upper_bound.y);
            upper_bound.z = fmaxf(float(bounds.upper_bound.z + CZ*cpos.z), upper_bound.z);
        }
    }
    //printf("Bounds updated to %f %f %f, %f %f %f\n", lower_bound.x, lower_bound.y, lower_bound.z,
    //                        upper_bound.x, upper_bound.y, upper_bound.z);
}

int RenderableSuperObject::get_collision_priority() {
    return 1;
}

bool RenderableSuperObject::check_collision_vs(Entity* other) {
    if (other->get_collision_priority() == 0) {
        return collides_with_entity(other);
    }
    else {
        return collides_with_superobject((RenderableSuperObject*)other);
    }
}

bool RenderableSuperObject::collides_with_entity(Entity* other) {
    // calculate RWC of other's bounds
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
    
    // now transform into cac, crc
    ivec3 lower_cac, lower_crc;
    transform_into_chunk_bounds(&lower_cac, &lower_crc, lower_oac.x, lower_oac.y, lower_oac.z);
    // now transform into cac, crc
    ivec3 upper_cac, upper_crc;
    transform_into_chunk_bounds(&upper_cac, &upper_crc, upper_oac.x, upper_oac.y, upper_oac.z);
    
    // iterate through chunks directly and see if there is intersection
    // if so, return region of conflict
    
    // iterate through our chunk region and see if it exists in our mapping
    for (int x = lower_cac.x; x <= upper_cac.x; x++) {
        for (int y = lower_cac.y; y <= upper_cac.y; y++) {
            for (int z = lower_cac.z; z <= upper_cac.z; z++) {
                if (!within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                if (!load_chunk(x, y, z)) {
                    ivec3 lower_corner(0, 0, 0);
                    ivec3 upper_corner(CX - 1, CY - 1, CZ - 1);
                    if (x == lower_cac.x)
                        lower_corner.x = lower_crc.x;
                    if (x == upper_cac.x)
                        upper_corner.x = upper_crc.x;
                    if (y == lower_cac.y)
                        lower_corner.y = lower_crc.y;
                    if (y == upper_cac.y)
                        upper_corner.y = upper_crc.y;
                    if (z == lower_cac.z)
                        lower_corner.z = lower_crc.z;
                    if (z == upper_cac.z)
                        upper_corner.z = upper_crc.z;
                    ivec3 chunk_pos = ivec3(x, y, z);
                    RenderableChunk* chunk = chunks[chunk_pos];
                    if (chunk->intersects_my_bounds(lower_corner, upper_corner)) {
                        // do a more careful check
                        for (int xt = lower_corner.x; xt <= upper_corner.x; xt++) {
                            for (int yt = lower_corner.y; yt <= upper_corner.y; yt++) {
                                for (int zt = lower_corner.z; zt <= upper_corner.z; zt++) {
                                    if (chunk->blk[xt][yt][zt].type) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool RenderableSuperObject::collides_with_superobject(RenderableSuperObject* other) {
    // calculate RWC of other's bounds
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
    
    // now transform into cac, crc
    ivec3 lower_cac, lower_crc;
    transform_into_chunk_bounds(&lower_cac, &lower_crc, lower_oac.x, lower_oac.y, lower_oac.z);
    // now transform into cac, crc
    ivec3 upper_cac, upper_crc;
    transform_into_chunk_bounds(&upper_cac, &upper_crc, upper_oac.x, upper_oac.y, upper_oac.z);
    
    // iterate through chunks directly and see if there is intersection
    // if so, return region of conflict
    
    // iterate through our chunk region and see if it exists in our mapping
    for (int x = lower_cac.x; x <= upper_cac.x; x++) {
        for (int y = lower_cac.y; y <= upper_cac.y; y++) {
            for (int z = lower_cac.z; z <= upper_cac.z; z++) {
                if (!within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                if (!load_chunk(x, y, z)) {
                    ivec3 lower_corner(0, 0, 0);
                    ivec3 upper_corner(CX - 1, CY - 1, CZ - 1);
                    if (x == lower_cac.x)
                        lower_corner.x = lower_crc.x;
                    if (x == upper_cac.x)
                        upper_corner.x = upper_crc.x;
                    if (y == lower_cac.y)
                        lower_corner.y = lower_crc.y;
                    if (y == upper_cac.y)
                        upper_corner.y = upper_crc.y;
                    if (z == lower_cac.z)
                        lower_corner.z = lower_crc.z;
                    if (z == upper_cac.z)
                        upper_corner.z = upper_crc.z;
                    ivec3 chunk_pos = ivec3(x, y, z);
                    RenderableChunk* chunk = chunks[chunk_pos];
                    if (chunk->intersects_my_bounds(lower_corner, upper_corner)) {
                        // do a more careful check
                        for (int xt = lower_corner.x; xt <= upper_corner.x; xt++) {
                            for (int yt = lower_corner.y; yt <= upper_corner.y; yt++) {
                                for (int zt = lower_corner.z; zt <= upper_corner.z; zt++) {
                                    if (chunk->blk[xt][yt][zt].type) {
                                        // if any block other than air intersects we fucked
                                        fvec3 world_coord;
                                        transform_into_world_coordinates(&world_coord,
                                                                         xt + CX*x,
                                                                         yt + CY*y,
                                                                         zt + CZ*z);
                                        if (other->get_block(world_coord.x,
                                                             world_coord.y,
                                                             world_coord.z).type) {
                                            // break here to debug
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
