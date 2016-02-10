//
//  chunkloadjob.cpp
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "chunkloadjob.h"
#include "superobjectrender.h"

ChunkLoadJob::ChunkLoadJob(int tid, RenderableSuperObject* t, fvec3 ppos): ThreadJob(tid) {
    job_name = "chunk_loading";
    
    target = t;
    player_pos = ppos;
    loaded_chunk_info_lock = SDL_CreateMutex();
}

void ChunkLoadJob::init() {
    // fill details of what chunks to load and what to save/free
    fvec3 oac;
    target->transform_into_my_coordinates(&oac, player_pos.x, player_pos.y, player_pos.z);
    // now transform into cac, crc. we will only need the CAC coordinates
    ivec3 cac, crc;
    target->transform_into_chunk_bounds(&cac, &crc, oac.x, oac.y, oac.z);
    
    xmin = cac.x - CHUNK_RENDER_DIST;
    xmax = cac.x + CHUNK_RENDER_DIST + 1;
    ymin = cac.y - CHUNK_RENDER_DIST;
    ymax = cac.y + CHUNK_RENDER_DIST + 1;
    zmin = cac.z - CHUNK_RENDER_DIST;
    zmax = cac.z + CHUNK_RENDER_DIST + 1;
    
    std::vector<ivec3> chunks_to_delete;
    SDL_LockMutex(loaded_chunk_info_lock);
    for (auto &i: target->chunks) {
        if (i.first.x < xmin || i.first.x >= xmax
            || i.first.y < ymin || i.first.y >= ymax
            || i.first.z < zmin || i.first.z >= zmax) {
            LoadedChunkInfo info = LoadedChunkInfo();
            info.pos = i.first;
            info.chunk = i.second;
            save_and_free.push_back(info);
            chunks_to_delete.push_back(i.first);
        }
    }
    for (auto &i: chunks_to_delete) {
        target->delete_chunk(i.x, i.y, i.z);
    }
    SDL_UnlockMutex(loaded_chunk_info_lock);
}

void ChunkLoadJob::run() {
    for (auto &i: save_and_free) {
        target->save_chunk(i.chunk, i.pos.x, i.pos.y, i.pos.z);
        delete i.chunk;
    }
    
    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            for (int z = zmin; z < zmax; z++) {
                if (!target->within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                if (target->chunks.count(ivec3(x,y,z))) {
                    // chunk already exists + is loaded in
                    continue;
                }
                if (is_finished()) {
                    // periodically do this to stop the thread prematurely if we need
                    return;
                }
                RenderableChunk* chunk = target->get_chunk(x, y, z);
                
                if (chunk) {
                    LoadedChunkInfo info = LoadedChunkInfo();
                    info.pos = ivec3(x, y, z);
                    info.chunk = chunk;
                    
                    SDL_LockMutex(loaded_chunk_info_lock);
                    loaded.push_back(info);
                    SDL_UnlockMutex(loaded_chunk_info_lock);
                }
            }
        }
    }
    printf("Thread job finished successfully %d\n", thread_id);
    mark_finished();
}

void ChunkLoadJob::flush() {
    SDL_LockMutex(loaded_chunk_info_lock);
    for (auto &i: loaded) {
        if (target->chunks.count(i.pos)) {
            printf("this chunk is already loaded in kek\n");
        }
        target->chunks[i.pos] = i.chunk;
        target->setup_chunk(i.chunk, i.pos.x, i.pos.y, i.pos.z);
    }
    loaded.clear();
    SDL_UnlockMutex(loaded_chunk_info_lock);
}

void ChunkLoadJob::canceled() {
    SDL_LockMutex(loaded_chunk_info_lock);
    for (auto &i: loaded) {
        // free all chunks we were about to add...
        delete i.chunk;
    }
    loaded.clear();
    SDL_UnlockMutex(loaded_chunk_info_lock);
    
    ThreadJob::canceled();
}