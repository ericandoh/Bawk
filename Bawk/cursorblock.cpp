//
//  cursorblock.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <GLFW/glfw3.h>
#include "cursorblock.h"
#include "worldrender.h"

struct blockvboholder {
    CursorBlock* block;
    GLuint coord_vbo;
    GLuint texture_vbo;
};

static struct blockvboholder block_vbo_slot[1] = {0};

CursorBlock::CursorBlock(uint16_t type) {
    block = type;
}

// sets the blocks in this representation into the world, and if template is not null, into the
// template as well
void CursorBlock::set_blocks(World* world, TemporaryTemplate* temp) {
    ivec4 looking_at = get_look_at();
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    // TODO check that the block picked by world is valid
    if (block) {
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
    }
    ivec3 block_pos = ivec3(mx, my, mz);
    world->place_block(block_pos, block);
    //printf("Placing at (%d, %d, %d)   (face %d)\n",
    //       mx, my, mz, face);
    if (temp)
        temp->add_block(block_pos, block);
}
// for a single block, this will call set_blocks (above) directly.
// for a template block, this will lock the position of the current cursoritem template
// then a call to set_blocks will be made later
bool CursorBlock::place_blocks(World* world, TemporaryTemplate* temp) {
    set_blocks(world, temp);
    // this gets passed to placed_current_item, that needs to be set to false
    // since this is a single block and is already placed
    return false;
}
// only needed for instances of template. the default does jack shit
void CursorBlock::move_block(ivec3 dir) {
    printf("Warning: Move block called on a singe block entity");
}

void CursorBlock::render_and_position(fmat4* transform) {
    if (block == 0)
        return;
    // TODO if it is too far away don't render it!
    ivec4 looking_at = get_look_at();
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
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
    
    float bx = mx;
    float by = my;
    float bz = mz;
    
    /*
    // Render a box around the block we are pointing at
    float box[24][3] = {
        {bx + 0, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 0, by + 1, bz + 0},
        {bx + 1, by + 1, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 1, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 1},
        
        {bx + 0, by + 0, bz + 0},
        {bx + 0, by + 1, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 1, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 0, bz + 1},
        {bx + 1, by + 1, bz + 1},
        
        {bx + 0, by + 0, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 0},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 0},
        {bx + 1, by + 1, bz + 1},
    };*/
    
    // Render a box around the block we are pointing at
    float box[36][3] = {
        // x
        {bx + 0, by + 0, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 0},
        
        {bx + 0, by + 1, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 1},
        
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 1, bz + 0},
        {bx + 1, by + 0, bz + 1},
        
        {bx + 1, by + 1, bz + 0},
        {bx + 1, by + 1, bz + 1},
        {bx + 1, by + 0, bz + 1},
        
        // y
        {bx + 0, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 0, by + 0, bz + 1},
        
        {bx + 0, by + 0, bz + 1},
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 1},
        
        {bx + 0, by + 1, bz + 0},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 0},
        
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 0},
        
        // z
        {bx + 0, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 0, by + 1, bz + 0},
        
        {bx + 0, by + 1, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 1, bz + 0},
        
        {bx + 0, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 0, bz + 1},
        
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 1},
        {bx + 1, by + 0, bz + 1},
        
    };
    float box_texture[36][3];
    for (int p = 0; p < 36; p++) {
        // first 8 bits. Represents the x-axis in our texture atlas
        box_texture[p][0] = block & 0xFF;
        // last 8 bits. Represents the y-axis in our texture atlas
        box_texture[p][1] = block >> 0x8;
        box_texture[p][2] = (p >= 12 && p < 24) ? 1 : 0;
    }
    
    set_transform_matrix(*transform);
    
    if (block_vbo_slot[0].block != this) {
        // steal this
        if (!block_vbo_slot[0].block) {
            // if not initialized, make it
            glGenBuffers(1, &block_vbo_slot[0].coord_vbo);
            glGenBuffers(1, &block_vbo_slot[0].texture_vbo);
        }
        block_vbo_slot[0].block = this;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, block_vbo_slot[0].coord_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, block_vbo_slot[0].texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(texture_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CursorBlock::cleanup_all() {
    if (block_vbo_slot[0].block == this) {
        glDeleteBuffers(1, &block_vbo_slot[0].coord_vbo);
        glDeleteBuffers(1, &block_vbo_slot[0].texture_vbo);
    }
}