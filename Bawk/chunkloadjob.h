//
//  chunkloadjob.h
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__chunkloadjob__
#define __Bawk__chunkloadjob__

#include <stdio.h>
#include <vector>
#include "threadjob.h"

class RenderableSuperObject;
class RenderableChunk;

struct LoadedChunkInfo {
    ivec3 pos;
    RenderableChunk* chunk;
};

class ChunkLoadJob: public ThreadJob {
    // update chunks near this player pos
    fvec3 player_pos;
    int xmin, xmax, ymin, ymax, zmin, zmax;
    
    std::vector<LoadedChunkInfo> loaded;
    std::vector<LoadedChunkInfo> save_and_free;
    RenderableSuperObject* target;
    
    // lock on loaded/save_and_free variables above
    SDL_mutex* loaded_chunk_info_lock;
    
public:
    ChunkLoadJob(int tid, RenderableSuperObject* t, fvec3 ppos);
    
    void init() override;
    void run() override;
    void flush() override;
    void canceled() override;

};

#endif /* defined(__Bawk__chunkloadjob__) */
