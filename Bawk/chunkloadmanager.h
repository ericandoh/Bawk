//
//  chunkloadmanager.h
//  Bawk
//
//  Created by Eric Oh on 1/1/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__chunkloadmanager__
#define __Bawk__chunkloadmanager__

#include <stdio.h>
#include <vector>
#include "basic_types.h"
#include "importsdl.h"

class RenderableSuperObject;
class RenderableChunk;

struct LoadedChunkInfo {
    ivec3 pos;
    RenderableChunk* chunk;
};

class ChunkLoadManager {
private:
    bool am_updating;   // should only be modified on main thread
    bool mutex_initialized;
    bool needs_update;
    fvec3 most_recent_player_pos;
public:
    RenderableSuperObject* obj;
    int xmin, xmax, ymin, ymax, zmin, zmax;
    std::vector<LoadedChunkInfo> loaded;
    std::vector<LoadedChunkInfo> save_and_free;
    SDL_mutex* am_finished_mutex;
    SDL_mutex* import_loaded_mutex;
    bool am_finished;   // modifiable on both main + side thread, needs lock protection
    ChunkLoadManager();
    void start_update(RenderableSuperObject* obj, fvec3 player_pos);
    void flush_chunks();
};

#endif /* defined(__Bawk__chunkloadmanager__) */
