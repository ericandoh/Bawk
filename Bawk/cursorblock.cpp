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
#include "blockrender.h"

CursorBlock::CursorBlock(block_type type) {
    block = type;
}

// sets the blocks in this representation into the world, and if template is not null, into the
// template as well
bool CursorBlock::set_blocks(Player* player, World* world, TemporaryTemplate* temp) {
    int mx, my, mz;
    BlockOrientation orient;
    if (!update_pointing_position(&mx, &my, &mz, &orient, block.type)) {
        return false;
    }
    ivec3 block_pos = ivec3(mx, my, mz);
    block.orientation = orient;
    block.owner = player->getID();
    world->place_block(block_pos, block);
    if (temp)
        temp->add_block(block_pos, block);
    return true;
}
// for a single block, this will call set_blocks (above) directly.
// for a template block, this will lock the position of the current cursoritem template
// then a call to set_blocks will be made later
bool CursorBlock::place_blocks(Player* player, World* world, TemporaryTemplate* temp) {
    set_blocks(player, world, temp);
    // this gets passed to placed_current_item, that needs to be set to false
    // since this is a single block and is already placed
    return false;
}
// only needed for instances of template. the default does jack shit
void CursorBlock::move_block(ivec3 dir) {
    printf("Warning: Move block called on a singe block entity");
}

void CursorBlock::get_bounds(ivec3* upper) {
    upper->x = 1;
    upper->y = 1;
    upper->z = 1;
}

void CursorBlock::render_block(fmat4* transform, float bx, float by, float bz) {
    
    // Render a box around the block we are pointing at
    GLbyte box[36][3];
    GLbyte box_texture[36][3];
    
    int i = 0;
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 0, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 1, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 0, block, BlockOrientation::BACK);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 0, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 1, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 1, block, BlockOrientation::BACK);
    
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 0, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 0, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 1, block, BlockOrientation::FRONT);
    
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 0, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 1, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 1, block, BlockOrientation::FRONT);
    
    // y
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 0, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 0, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 1, block, BlockOrientation::BOTTOM);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 1, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 0, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 1, block, BlockOrientation::BOTTOM);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 0, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 1, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 0, block, BlockOrientation::TOP);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 1, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 1, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 0, block, BlockOrientation::TOP);
    
    // z
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 0, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 0, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 0, block, BlockOrientation::LEFT);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 0, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 0, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 0, block, BlockOrientation::LEFT);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 0, bz + 1, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 1, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 1, block, BlockOrientation::RIGHT);
    
    set_coord_and_texture(box, box_texture, i++, bx + 0, by + 1, bz + 1, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 1, bz + 1, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, bx + 1, by + 0, bz + 1, block, BlockOrientation::RIGHT);
    
    set_transform_matrix(*transform);
    
    set_block_draw_mode(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(texture_attribute_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

void CursorBlock::render_at_zero(fmat4* transform) {
    if (block.type == 0)
        return;
    render_block(transform, 0, 0, 0);
}

void CursorBlock::render_and_position(fmat4* transform) {
    if (block.type == 0)
        return;
    int mx, my, mz;
    BlockOrientation orient;
    if (!update_pointing_position(&mx, &my, &mz, &orient, block.type)) {
        return;
    }
    render_block(transform, mx, my, mz);
}

cursor_item_distinguisher CursorBlock::get_distinguisher() {
    cursor_item_distinguisher val;
    val.is_blk = true;
    val.is_recipe = false;
    val.bid = block.type;
    val.pid = 0;
    val.vid = 0;
    return val;
}

void CursorBlock::delete_self() {
    delete this;
}
