//
//  cursorsuperobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursorsuperobject.h"
#include "block.h"


// heavily modify this so that
// 1. it doesn't mark itself as a folder to disk
// 2. it doesn't load/save chunks to disk (superobjectrender has the capacity to hold all the chunks
//    if it wasn't for that pesky loading/unloading)
// 3. override SuperObjectRender directly instead
// 4. give superobjectrender a position attribute to scale stuff
// 5. we'll handle rotation later (fuck that shit mate)
CursorSuperObject::CursorSuperObject() {
    locked = false;
}

// sets the blocks in this representation into the world, and if template is not null, into the
// template as well
bool CursorSuperObject::set_blocks(World* world, TemporaryTemplate* temp) {
    
    if (world->will_collide_with_anything(this)) {
        return false;
    }
    
    printf("Publishing blocks from a template!\n");
    int counter = 0;
    for (auto& kv : chunks) {
        for (int x = 0; x < CX; x++) {
            for (int y = 0; y < CY; y++) {
                for (int z = 0; z < CZ; z++) {
                    uint16_t block = kv.second->blk[x][y][z];
                    if (block) {
                        counter++;
                        fvec3 world_coord;
                        ivec3 chunk_pos = kv.first;
                        transform_into_world_coordinates(&world_coord,  chunk_pos.x*CX+x,
                                                                        chunk_pos.y*CY+y,
                                                                        chunk_pos.z*CZ+z);
                        ivec3 block_pos = ivec3(int(world_coord.x),
                                                int(world_coord.y),
                                                int(world_coord.z));
                        world->place_block(block_pos, block);
                        printf("Placing to (%d, %d, %d)\n", block_pos.x, block_pos.y, block_pos.z);
                        if (temp)
                            temp->add_block(block_pos, block);
                    }
                }
            }
        }
    }
    printf("Set %d blocks\n", counter);
    locked = false;
    return true;
}

// for a single block, this will call set_blocks (above) directly.
// for a template block, this will lock the position of the current cursoritem template
// then a call to set_blocks will be made later
bool CursorSuperObject::place_blocks(World* world, TemporaryTemplate* temp) {
    if (locked)
        return true;
    ivec4 looking_at;
    if (!get_look_at(&looking_at)) {
        return false;
    }
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    // TODO make sure my lookat is from the world and the world only
    // ie query the world here
    if(face == 0)
        mx++;
    if(face == 3)
        mx--;
    if(face == 1)
        my++;
    if(face == 4)
        my--;
    if(face == 2)
        mz++;
    if(face == 5)
        mz--;
    // save position
    set_position(fvec3(mx, my, mz));
    locked = true;
    return true;
}

// only needed for instances of template. the default does jack shit
void CursorSuperObject::move_block(ivec3 dir) {
    if (!locked) {
        return;
    }
    pos = fvec3(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z);
}

void CursorSuperObject::render_and_position(fmat4* transform) {
    if (!locked) {
        ivec4 looking_at;
        if (!get_look_at(&looking_at)) {
            return;
        }
        int mx = looking_at.x;
        int my = looking_at.y;
        int mz = looking_at.z;
        int face = looking_at.w;
        // TODO make sure my lookat is from the world and the world only
        // ie query the world here
        if(face == 0)
            mx++;
        if(face == 3)
            mx--;
        if(face == 1)
            my++;
        if(face == 4)
            my--;
        if(face == 2)
            mz++;
        if(face == 5)
            mz--;
        // TODO if it is too far away don't render it!
        // save position
        if (pos != fvec3(mx, my, mz)) {
            set_position(fvec3(mx, my, mz));
        }
    }
    render(transform);
}

void CursorSuperObject::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    // always render all objects in this cursorsuperobject
    // do nothing
}

// called when a request to load a chunk from disk is made
// in this case, we'll keep all chunks in memory, so this should be only called
// if the chunk didn't exist before, in which case we give back an empty chunk
int CursorSuperObject::get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) {
    get_empty_chunk(to_arr);
    return 0;
}


// called when a chunk goes out of scope and no longer needs to be rendered
int CursorSuperObject::save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) {
    // do nothing. in fact, this shouldn't ever be called, ever. bro.
    printf("Tried to save a cursorsuperobject chunk %d,%d,%d\n", x, y, z);
    return 0;
}

void CursorSuperObject::remove_self() {
    // do nothing - when removing self, no chunks should be saved to disk
    // maybe save the template or something but thats about it
}

bool CursorSuperObject::within_dimensions_chunk(int x, int y, int z) {
    if (chunks.count(ivec3(x, y, z))) {
        return true;
    }
    return false;
}

void CursorSuperObject::unlock() {
    locked = false;
}

CursorSuperObject* create_from_template(World* world, TemporaryTemplate* temp) {
    printf("Publishing template!\n");
    // package our blocks into a cursorsuperobject
    CursorSuperObject* object = new CursorSuperObject();
    
    std::vector<block_data> blocks = temp->publish(world);
    
    if (!blocks.size()) {
        // we published an empty template...brah wtf you doing
        return 0;
    }
    
    // first, find min(x), min(y), min(z) so we can set that to normalize to (0,0,0)
    ivec3 lower_corner = blocks[0].position;
    for (auto &i : blocks) {
        ivec3 position = i.position;
        lower_corner.x = minimum(lower_corner.x, position.x);
        lower_corner.y = minimum(lower_corner.y, position.y);
        lower_corner.z = minimum(lower_corner.z, position.z);
    }
    
    for (auto &i : blocks) {
        ivec3 position = ivec3(i.position.x - lower_corner.x,
                               i.position.y - lower_corner.y,
                               i.position.z - lower_corner.z);
        printf("Publishing block %d at (%d,%d,%d)\n", i.block, position.x, position.y, position.z);
        object->set_block(position.x, position.y, position.z, i.block);
    }
    
    return object;
}

void CursorSuperObject::cleanup_all() {
    remove_self();
}


