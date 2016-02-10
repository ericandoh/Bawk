//
//  chunk_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "chunk_loader.h"
#include "chunkrender.h"

#define INVALID_CHUNK_COUNT 0

//reader.read_pointer(&(to_arr[0][0][0]), sizeof(to_arr[0][0][0])*CX*CY*CZ);
//writer.save_pointer(&(chunk->blk[0][0][0]), sizeof(chunk->blk[0][0][0])*CX*CY*CZ);

RenderableChunk* read_chunk_data(IODataObject* obj) {
    // use a byte to store count
    RenderableChunk* chunk = new RenderableChunk();
    
    block_type previous;
    uint16_t count = 0;
    bool end = false;
    for (int y = 0; y < CY; y++) {
        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                if (count == 0) {
                    // read next run of blocks and the respective counts
                    count = obj->read_value<uint16_t>();
                    previous = obj->read_value<block_type>();
                    if (count == INVALID_CHUNK_COUNT) {
                        end = true;
                        break;
                    }
                }
                chunk->blk[x][y][z] = previous;
                count--;
            }
            if (end) {
                break;
            }
        }
        if (end) {
            break;
        }
    }
    chunk->update_dimensions();
    
    return chunk;
}

void save_chunk_data(RenderableChunk* chunk, IODataObject* obj) {
    block_type previous;
    bool set_previous = false;
    uint16_t count = 0;
    for (int y = 0; y < CY; y++) {
        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                if (set_previous) {
                    if (previous.equals_exact(&chunk->blk[x][y][z])) {
                        count += 1;
                    }
                    else {
                        // save block count
                        obj->save_value(count);
                        obj->save_value(previous);
                        // setup for next run of blocks
                        set_previous = false;
                    }
                }
                if (!set_previous) {
                    // start counting
                    previous = chunk->blk[x][y][z];
                    count = 1;
                    set_previous = true;
                }
            }
        }
    }
    // save last run of blocks
    obj->save_value(count);
    obj->save_value(previous);
    // save some value to indicate no more blocks
    uint16_t end_count = INVALID_CHUNK_COUNT;
    obj->save_value(end_count);
}