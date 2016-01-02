//
//  chunkloadmanager.cpp
//  Bawk
//
//  Created by Eric Oh on 1/1/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "chunkloadmanager.h"
#include "superobjectrender.h"

ChunkLoadManager::ChunkLoadManager() {
    mutex_initialized = false;
    
    am_updating = false;
    needs_update = false;
    most_recent_player_pos = fvec3(0,0,0);
    obj = 0;
    am_finished = 0;
}

int load_chunks_async(void* ptr) {
    ChunkLoadManager* manager = (ChunkLoadManager*) ptr;
    
    for (auto &i: manager->save_and_free) {
        manager->obj->save_chunk(i.chunk->blk, i.pos.x, i.pos.y, i.pos.z);
        delete i.chunk;
    }
    
    for (int x = manager->xmin; x < manager->xmax; x++) {
        for (int y = manager->ymin; y < manager->ymax; y++) {
            for (int z = manager->zmin; z < manager->zmax; z++) {
                if (!manager->obj->within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                if (manager->obj->chunks.count(ivec3(x,y,z))) {
                    // chunk already exists + is loaded in
                    continue;
                }
                RenderableChunk* chunk = manager->obj->obtain_chunk(x, y, z);
                
                if (chunk) {
                    LoadedChunkInfo info = LoadedChunkInfo();
                    info.pos = ivec3(x, y, z);
                    info.chunk = chunk;
                    
                    SDL_LockMutex(manager->import_loaded_mutex);
                    manager->loaded.push_back(info);
                    SDL_UnlockMutex(manager->import_loaded_mutex);
                }
            }
        }
    }
    SDL_LockMutex(manager->am_finished_mutex);
    manager->am_finished = true;
    SDL_UnlockMutex(manager->am_finished_mutex);
    return 0;
}

void ChunkLoadManager::start_update(RenderableSuperObject* obj, fvec3 player_pos) {
    if (am_updating) {
        // an update already in progress, can't just cancel it
        // queue the next update
        needs_update = true;
        this->obj = obj;
        most_recent_player_pos = player_pos;
    }
    else {
        am_updating = true;
        needs_update = false;
        am_finished = false;
        
        loaded.clear();
        save_and_free.clear();
        
        // delete all chunks
        fvec3 oac;
        obj->transform_into_my_coordinates(&oac, player_pos.x, player_pos.y, player_pos.z);
        // now transform into cac, crc. we will only need the CAC coordinates
        ivec3 cac, crc;
        obj->transform_into_chunk_bounds(&cac, &crc, oac.x, oac.y, oac.z);
        
        xmin = cac.x - CHUNK_RENDER_DIST;
        xmax = cac.x + CHUNK_RENDER_DIST + 1;
        ymin = cac.y - CHUNK_RENDER_DIST;
        ymax = cac.y + CHUNK_RENDER_DIST + 1;
        zmin = cac.z - CHUNK_RENDER_DIST;
        zmax = cac.z + CHUNK_RENDER_DIST + 1;
        
        this->obj = obj;
        
        // make bounding box here and check
        
        std::vector<ivec3> chunks_to_delete;
        for (auto &i: obj->chunks) {
            if (i.first.x < xmin || i.first.x >= xmax
                || i.first.y < ymin || i.first.y >= ymax
                || i.first.z < zmin || i.first.z >= zmax) {
                chunks_to_delete.push_back(i.first);
            }
        }
        for (auto &i: chunks_to_delete) {
            if (obj->chunks.count(i)) {
                LoadedChunkInfo info = LoadedChunkInfo();
                info.pos = i;
                info.chunk = obj->chunks[i];
                save_and_free.push_back(info);
            }
            obj->delete_chunk(i.x, i.y, i.z);
        }
        // multithread this part
        if (!mutex_initialized) {
            am_finished_mutex = SDL_CreateMutex();
            import_loaded_mutex = SDL_CreateMutex();
            mutex_initialized = true;
        }
        
        //load_chunks_async(this);
        
        // TODO make threads spawned here waited by an end thread manager which will fire at the end of display.cpp
        SDL_Thread* thread;
        thread = SDL_CreateThread(load_chunks_async, "update_chunks", (void*)this);
        if (thread == 0) {
            printf("Thread creation failed\n");
            load_chunks_async(this);
        }
    }
}

void ChunkLoadManager::flush_chunks() {
    if (am_updating) {
        // grab list of chunks from thread to add/remove
        SDL_LockMutex(import_loaded_mutex);
        int counter = (int)obj->chunks.size();
        for (auto &i: loaded) {
            if (obj->chunks.count(i.pos)) {
                printf("wtfx2\n");
            }
            obj->chunks[i.pos] = i.chunk;
            obj->setup_chunk(i.chunk, i.pos.x, i.pos.y, i.pos.z);
        }
        if (loaded.size() + counter != obj->chunks.size()) {
            printf("wtf\n");
        }
        loaded.clear();
        SDL_UnlockMutex(import_loaded_mutex);
        SDL_LockMutex(am_finished_mutex);
        if (am_finished) {
            am_updating = false;
        }
        SDL_UnlockMutex(am_finished_mutex);
    }
    
    if (!am_updating && needs_update) {
        // start the next update if needed
        start_update(obj, most_recent_player_pos);
    }
}
