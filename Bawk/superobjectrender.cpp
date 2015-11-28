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
    // nothing special needed here
}

// --- RenderableSuperObject
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
        chunks[left]->right = chunk;
        chunk->left->changed = true;
    }
    if (chunks.count(right)) {
        chunk->right = chunks[right];
        chunks[right]->left = chunk;
        chunk->right->changed = true;
    }
    if (chunks.count(below)) {
        chunk->below = chunks[below];
        chunks[below]->above = chunk;
        chunk->below->changed = true;
    }
    if (chunks.count(above)) {
        chunk->above = chunks[above];
        chunks[above]->below = chunk;
        chunk->above->changed = true;
    }
    if (chunks.count(front)) {
        chunk->front = chunks[front];
        chunks[front]->back = chunk;
        chunk->front->changed = true;
    }
    if (chunks.count(back)) {
        chunk->back = chunks[back];
        chunks[back]->front = chunk;
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
    // TODO WRONG WRONG WRONG THIS WONT SAVE THE CHUNK FUCK
    chunks[pos]->cleanup();
    delete chunks[pos];
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
    // transform to OAC
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
        block_type current = chunks[cac]->get(crc.x, crc.y, crc.z);
        if (current.type) {
            handle_block_removal(crc.x, crc.y, crc.z, current);
        }
        chunks[cac]->set(crc.x, crc.y, crc.z, type);
        if (type.type) {
            handle_block_addition(crc.x, crc.y, crc.z, type);
        }
        // update the dimension vectors
        update_dimensions_from_chunk(cac);
    }
    else {
        // loading the chunk failed, probably because this is out of bounds
        // make a new chunk here
        // this should only be called while creating vehicles from blocks while
        // finishing up a template
        
        block_type raw_chunk[CX][CY][CZ];
        get_empty_chunk(raw_chunk);
        RenderableChunk* chunk = new RenderableChunk(raw_chunk);
        chunks[cac] = chunk;
        
        block_type current = chunks[cac]->get(crc.x, crc.y, crc.z);
        if (current.type) {
            handle_block_removal(crc.x, crc.y, crc.z, current);
        }
        chunks[cac]->set(crc.x, crc.y, crc.z, type);
        if (type.type) {
            handle_block_addition(crc.x, crc.y, crc.z, type);
        }
        // we load data from disk so this should be consistent, but just to be safe do a check
        update_dimensions_from_chunk(ivec3(x, y, z));
        
        ivec3 left(cac.x - 1, cac.y, cac.z);
        ivec3 right(cac.x + 1, cac.y, cac.z);
        ivec3 below(cac.x, cac.y - 1, cac.z);
        ivec3 above(cac.x, cac.y + 1, cac.z);
        ivec3 front(cac.x, cac.y, cac.z - 1);
        ivec3 back(cac.x, cac.y, cac.z + 1);
        
        if (chunks.count(left)) {
            chunk->left = chunks[left];
            chunks[left]->right = chunk;
            chunk->left->changed = true;
        }
        if (chunks.count(right)) {
            chunk->right = chunks[right];
            chunks[right]->left = chunk;
            chunk->right->changed = true;
        }
        if (chunks.count(below)) {
            chunk->below = chunks[below];
            chunks[below]->above = chunk;
            chunk->below->changed = true;
        }
        if (chunks.count(above)) {
            chunk->above = chunks[above];
            chunks[above]->below = chunk;
            chunk->above->changed = true;
        }
        if (chunks.count(front)) {
            chunk->front = chunks[front];
            chunks[front]->back = chunk;
            chunk->front->changed = true;
        }
        if (chunks.count(back)) {
            chunk->back = chunks[back];
            chunks[back]->front = chunk;
            chunk->back->changed = true;
        }
    }
}

void RenderableSuperObject::kill_block(float x, float y, float z) {
    set_block(x, y, z, block_type());
}

// helper function, sees if the absolute distance is less than the chunk render distance
bool is_within_range(ivec3* chunk_pos, int x, int y, int z) {
    return abs(chunk_pos->x - x) <= CHUNK_RENDER_DIST &&
            abs(chunk_pos->y - y) <= CHUNK_RENDER_DIST &&
            abs(chunk_pos->z - z) <= CHUNK_RENDER_DIST;
}

// check if a chunk coordinate (CAC) xyz is a chunk held by this object
bool RenderableSuperObject::within_dimensions_chunk(int x, int y, int z) {
    if (chunk_bounds.count(ivec3(x, y, z))) {
        return true;
    }
    return false;
}

bool RenderableSuperObject::intersects_chunk(ivec3 lower, ivec3 upper, ivec3 chunkpos) {
    ivec3 lower_corner = chunk_bounds[chunkpos].lower_bound;
    ivec3 upper_corner = chunk_bounds[chunkpos].upper_bound;
    return   !(lower.x > upper_corner.x || lower_corner.x > upper.x
               || lower.y > upper_corner.y || lower_corner.y > upper.y
               || lower.z > upper_corner.z || lower_corner.z > upper.z);
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
        recalculate_needed = int(bounds.lower.x) == (old_chunk_lower.x + CX*chunk_pos.x)  ||
                            int(bounds.lower.y) == (old_chunk_lower.y + CY*chunk_pos.y) ||
                            int(bounds.lower.z) == (old_chunk_lower.z + CZ*chunk_pos.z) ||
                            bounds.lower.x == FLT_MAX;
        recalculate_needed = recalculate_needed ||
                            int(bounds.upper.x) == (old_chunk_upper.x + CX*chunk_pos.x + 1)  ||
                            int(bounds.upper.y) == (old_chunk_upper.y + CY*chunk_pos.y + 1) ||
                            int(bounds.upper.z) == (old_chunk_upper.z + CZ*chunk_pos.z + 1) ||
                            bounds.upper.x == -FLT_MAX;
    }
    else {
        recalculate_needed = true;
    }
    chunk_bounds[chunk_pos] = {chunk_lower, chunk_upper};
    
    // iterate through all our chunks and update dimensions
    if (recalculate_needed) {
        bounds.lower = fvec3(FLT_MAX, FLT_MAX, FLT_MAX);
        bounds.upper = fvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto &i : chunk_bounds) {
            ivec3 cpos = i.first;
            struct chunk_bounds cbounds = i.second;
            if (cbounds.lower_bound.x == CX) {
                // our chunk bound is invalid
                continue;
            }
            bounds.lower.x = fminf(float(cbounds.lower_bound.x + CX*cpos.x), bounds.lower.x);
            bounds.lower.y = fminf(float(cbounds.lower_bound.y + CY*cpos.y), bounds.lower.y);
            bounds.lower.z = fminf(float(cbounds.lower_bound.z + CZ*cpos.z), bounds.lower.z);
            bounds.upper.x = fmaxf(float(cbounds.upper_bound.x + CX*cpos.x + 1), bounds.upper.x);
            bounds.upper.y = fmaxf(float(cbounds.upper_bound.y + CY*cpos.y + 1), bounds.upper.y);
            bounds.upper.z = fmaxf(float(cbounds.upper_bound.z + CZ*cpos.z + 1), bounds.upper.z);
        }
    }
    //printf("Bounds updated to %f %f %f, %f %f %f\n", lower_bound.x, lower_bound.y, lower_bound.z,
    //                        upper_bound.x, upper_bound.y, upper_bound.z);
}

void RenderableSuperObject::save_all_chunks() {
    // save chunks not in memory
    int counter = 0;
    for (auto kv : chunks) {
        counter++;
        save_chunk(kv.second->blk, kv.first.x, kv.first.y, kv.first.z);
        kv.second->cleanup();
    }
    printf("Saved %d chunks\n", counter);
}

// --- Entity ---
Entity* RenderableSuperObject::poke(float x, float y, float z) {
    if (Entity::poke(x, y, z)) {
        if (get_block(x, y, z).type){
            return this;
        }
    }
    return 0;
}

bool RenderableSuperObject::break_block(float x, float y, float z) {
    set_block(x, y, z, block_type());
    return true;
}

void RenderableSuperObject::render(fmat4* transform) {
    for (auto iterator = chunks.begin(); iterator != chunks.end(); iterator++) {
        ivec3 sub_pos = iterator->first;
        
        fmat4 view;
        get_mvp(&view);
        view = glm::translate(view, fvec3(sub_pos.x * CX,
                                          sub_pos.y * CY,
                                          sub_pos.z * CZ));
        
        fmat4 mvp = *transform * view;
        
        // Is this chunk on the screen?
        glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
        
        //float d = glm::length(center);
        center.x /= center.w;
        center.y /= center.w;
        
        // If it is behind the camera, don't bother drawing it
        // was 0.5f before, but rounding errors forced me to bump this value up
        if(center.z < -CY * 0.75f)
            continue;
        
        // If it is outside the screen, don't bother drawing it
        if(fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
            continue;
        
        set_transform_matrix(mvp, view);
        iterator->second->render();
    }
}

void RenderableSuperObject::update_chunks(fvec3* player_pos) {
    // get player position in this object's axis
    fvec3 oac;
    transform_into_my_coordinates(&oac, player_pos->x, player_pos->y, player_pos->z);
    // now transform into cac, crc. we will only need the CAC coordinates
    ivec3 cac, crc;
    transform_into_chunk_bounds(&cac, &crc, oac.x, oac.y, oac.z);
    
    int xmin, xmax, ymin, ymax, zmin, zmax;
    xmin = cac.x - CHUNK_RENDER_DIST;
    xmax = cac.x + CHUNK_RENDER_DIST + 1;
    ymin = cac.y - CHUNK_RENDER_DIST;
    ymax = cac.y + CHUNK_RENDER_DIST + 1;
    zmin = cac.z - CHUNK_RENDER_DIST;
    zmax = cac.z + CHUNK_RENDER_DIST + 1;
    
    // make bounding box here and check
    
    std::vector<ivec3> chunks_to_delete;
    for (auto &i: chunks) {
        if (i.first.x < xmin || i.first.x >= xmax
            || i.first.y < ymin || i.first.y >= ymax
            || i.first.z < zmin || i.first.z >= zmax) {
            chunks_to_delete.push_back(i.first);
        }
    }
    for (auto &i: chunks_to_delete) {
        delete_chunk(i.x, i.y, i.z);
    }
    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            for (int z = zmin; z < zmax; z++) {
                if (!within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                load_chunk(x, y, z);
            }
        }
    }
}

int RenderableSuperObject::get_collision_level() {
    return 1;
}

bool RenderableSuperObject::collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level) {
    if (my_collision_lvl == 0) {
        return Entity::collides_with(other, my_bounds, other_bounds, my_collision_lvl, other_collision_level);
    }
    else if (my_collision_lvl == 1) {
        // see which chunks hit other_bounds, then call each chunk bounds on me
        // find intersection of my_bounds, other_bounds
        bounding_box intersection_box = get_bounding_box_intersection(*my_bounds, *other_bounds);
        // convert intersection_box into my coordinates
        bounding_box oac_intersection_box;
        transform_into_my_coordinates(&oac_intersection_box.lower,
                                      intersection_box.lower.x,
                                      intersection_box.lower.y,
                                      intersection_box.lower.z);
        transform_into_my_coordinates(&oac_intersection_box.upper,
                                      intersection_box.upper.x,
                                      intersection_box.upper.y,
                                      intersection_box.upper.z);
        oac_intersection_box.refit_for_rotation();
        
        // now transform into cac, crc
        ivec3 lower_cac, lower_crc;
        transform_into_chunk_bounds(&lower_cac, &lower_crc,
                                    oac_intersection_box.lower.x,
                                    oac_intersection_box.lower.y,
                                    oac_intersection_box.lower.z);
        // now transform into cac, crc
        ivec3 upper_cac, upper_crc;
        transform_into_chunk_bounds(&upper_cac, &upper_crc,
                                    ceilf(oac_intersection_box.upper.x),
                                    ceilf(oac_intersection_box.upper.y),
                                    ceilf(oac_intersection_box.upper.z));
        
        for (int x = lower_cac.x; x <= upper_cac.x; x++) {
            for (int y = lower_cac.y; y <= upper_cac.y; y++) {
                for (int z = lower_cac.z; z <= upper_cac.z; z++) {
                    if (!within_dimensions_chunk(x, y, z)) {
                        continue;
                    }
                    ivec3 lower_corner(0, 0, 0);
                    ivec3 upper_corner(CX, CY, CZ);
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
                    if (intersects_chunk(lower_corner, upper_corner, chunk_pos)) {
                        if (!load_chunk(x, y, z)) {
                            RenderableChunk* chunk = chunks[chunk_pos];
                            for (int xt = lower_corner.x; xt < upper_corner.x; xt++) {
                                for (int yt = lower_corner.y; yt < upper_corner.y; yt++) {
                                    for (int zt = lower_corner.z; zt < upper_corner.z; zt++) {
                                        if (chunk->blk[xt][yt][zt].type) {
                                            bounding_box subbox;
                                            subbox.lower = fvec3(x*CX + xt,
                                                                 y*CY + yt,
                                                                 z*CZ + zt);
                                            subbox.upper = fvec3(x*CX + xt + 1,
                                                                 y*CY + yt + 1,
                                                                 z*CZ + zt + 1);
                                            transform_into_world_coordinates(&subbox.lower, subbox.lower.x, subbox.lower.y, subbox.lower.z);
                                            transform_into_world_coordinates(&subbox.upper, subbox.upper.x, subbox.upper.y, subbox.upper.z);
                                            subbox.refit_for_rotation();
                                            if (other_collision_level > my_collision_lvl - 1) {
                                                if (other->collides_with(this, other_bounds, &subbox, other_collision_level, my_collision_lvl - 1)) {
                                                    return true;
                                                }
                                            }
                                            else {
                                                if (collides_with(other, &subbox, other_bounds, my_collision_lvl - 1, other_collision_level)) {
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
        }
    }
    return false;
}

int RenderableSuperObject::load_self(IODataObject* obj) {
    if (Entity::load_self(obj))
        return 1;
    
    // read in existing chunk bounds
    int chunk_count = obj->read_value<int>();
    for (int i = 0; i < chunk_count; i++) {
        ivec3 pos = obj->read_value<ivec3>();
        struct chunk_bounds bounds = obj->read_value<struct chunk_bounds>();
        chunk_bounds[pos] = bounds;
    }

    return 0;
}

void RenderableSuperObject::remove_self(IODataObject* obj) {
    Entity::remove_self(obj);
    
    // save chunk bounds
    int chunk_count = (int)chunk_bounds.size();
    obj->save_value(chunk_count);
    for (auto &i : chunk_bounds) {
        obj->save_value(i.first);
        obj->save_value(i.second);
    }
    
    save_all_chunks();
}
