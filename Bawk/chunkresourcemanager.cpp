//
//  chunkresourcemanager.cpp
//  Bawk
//
//  Created by Eric Oh on 11/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "chunkresourcemanager.h"
#include <unordered_map>
#include <deque>
#include "superobjectrender.h"

#define CHUNK_RESOURCE_SLOTS 2000

/*
 Algorithm Summary
 
 --- Partly Inspired by Jerry-Daycare (of Rick and Morty) ---
 
 Think of each chunk as a Jerry. We drop off a Jerry. If it gets lost, we'll just make a call and reload it no problem capiche. 
 Give each Jerry a placecard, if a Jerry exists too long and we need space we'll just toss it (and later if its needed some 
 Rick will come in with an invalid placecard and we'll just remake it for him no problem)
 
 Some Jerries we should absolutely never evict. Mark those with -dne (do not evict). Examples would be 
 chunks that we are rendering at a location.
 
 Some Jerries we'll use for a moment then we wont need it again. For example, if we're calculating new bounds of a superobject, 
 we'll need to bring in all chunks and then redo calculations. In that case, set the chunk to be allwoed to be freed after it is 
 used
 
 Jerries by default when loaded are marked with the -dne flag. unmark them if we don't need them necessarily (but maybe memory caching 
 will make it nice???)
 
 note that owners should be classified by PID,VID so if we have duplicate objects of the same PID, VID (like cursorobjects) they 
 will be pointing to the same chunk info if it is loaded in. this provides some sort of caching which is nice.  
 
 */

namespace ChunkResources {
    enum ChunkStatus {
        UNLOADED, LOADING, LOADED
    };
    enum ChunkPolicy {
        DNE, EVICT_OK, EVICTED
    };
    
    struct ChunkStatusInfo {
        ChunkStatus status;
        ChunkPolicy policy;
        RenderableSuperObject* owner;
        ivec3 pos;
    };
}

using namespace ChunkResources;

ChunkStatusInfo chunk_info[CHUNK_RESOURCE_SLOTS];
RenderableChunk chunk_data[CHUNK_RESOURCE_SLOTS];
std::deque<int> evicted_list;
int last_filled = 0;


int find_chunk_index() {
    if (last_filled < CHUNK_RESOURCE_SLOTS) {
        return (last_filled++);
    }
    else if (evicted_list.size()) {
        int placecard = evicted_list.front();
        evicted_list.pop_front();
        return placecard;
    }
    else {
        // we have no evictable chunks, and we apparently don't keep track of holes...
        // go through our chunk list and evict a chunk by force
        int placecard = last_filled % CHUNK_RESOURCE_SLOTS;
        save_chunk_resource(&chunk_data[placecard]);
        // find owner it belongs to and tell that to evict that chunk from its list
        ivec3 pos = chunk_info[placecard].pos;
        chunk_info[placecard].owner->chunks.erase(pos);
        last_filled++;
        return placecard;
    }
}

RenderableChunk* reserve_chunk_resource(RenderableSuperObject* owner, ivec3 pos) {
    int placecard = find_chunk_index();
    chunk_info[placecard].policy = DNE;
    chunk_info[placecard].owner = owner;
    chunk_info[placecard].pos = pos;
    chunk_data[placecard].reset();
    
    // now copy data into our chunk
    block_type raw_chunk[CX][CY][CZ];
    chunk_info[placecard].status = LOADING;
    if (owner->get_chunk(raw_chunk, pos.x, pos.y, pos.z)) {
        // failed to load chunk
        return 0;
    }
    chunk_info[placecard].status = LOADED;
    memcpy(&chunk_data[placecard].blk[0][0][0], &raw_chunk[0][0][0], sizeof(block_type)*CX*CY*CZ);
    chunk_data[placecard].update_dimensions();
    return &chunk_data[placecard];
}

RenderableChunk* reserve_empty_chunk_resource(RenderableSuperObject* owner, ivec3 pos) {
    int placecard = find_chunk_index();
    chunk_info[placecard].policy = DNE;
    chunk_info[placecard].owner = owner;
    chunk_info[placecard].pos = pos;
    chunk_data[placecard].reset();
    
    chunk_info[placecard].status = LOADING;
    get_empty_chunk(chunk_data[placecard].blk);
    chunk_info[placecard].status = LOADED;
    chunk_data[placecard].update_dimensions();
    return &chunk_data[placecard];
}

/*
RenderableChunk* get_chunk_resource(int placecard) {
    return &chunk_data[placecard];
}

RenderableChunk* get_chunk_resource(int placecard, RenderableSuperObject* owner, ivec3 chunk_pos) {
    if (chunk_info[placecard].owner == owner &&
        chunk_info[placecard].pos == chunk_pos &&
        chunk_info[placecard].status == LOADED &&
        chunk_info[placecard].policy != EVICTED) {
        return &chunk_data[placecard];
    }
    return 0;
}

RenderableChunk* get_chunk_resource_async(int placecard, RenderableSuperObject* owner, ivec3 chunk_pos) {
    // TODO implement this
    return get_chunk_resource(placecard, owner, chunk_pos);
}*/

int get_placecard_from(RenderableChunk* chunk) {
    return (int)(chunk - chunk_data);
}

void save_chunk_resource(RenderableChunk* chunk) {
    int placecard = get_placecard_from(chunk);
    int x = chunk_info[placecard].pos.x;
    int y = chunk_info[placecard].pos.y;
    int z = chunk_info[placecard].pos.z;
    chunk_info[placecard].owner->save_chunk(chunk_data[placecard].blk, x, y, z);
}

void free_chunk_resource(RenderableChunk* chunk) {
    int placecard = get_placecard_from(chunk);
    chunk_info[placecard].policy = EVICT_OK;
    chunk_data[placecard].cleanup();
    evicted_list.push_back(placecard);
}

void save_all_chunk_resources() {
    int total = std::min(last_filled, CHUNK_RESOURCE_SLOTS);
    for (int i = 0; i < total; i++) {
        if (chunk_info[i].owner &&
            chunk_info[i].status == LOADED &&
            chunk_info[i].policy != EVICTED) {
            save_chunk_resource(&chunk_data[i]);
        }
    }
}
