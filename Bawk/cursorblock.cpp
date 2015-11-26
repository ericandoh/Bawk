//
//  cursorblock.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursorblock.h"
#include "blocktracer.h"
#include "game.h"
#include "gametemplate.h"
#include "blockrender.h"
#include <glm/gtc/matrix_transform.hpp>

/*
#include "includeglfw.h"
#include "cursorblock.h"
#include "worldrender.h"
#include "temporarytemplate.h"
*/

CursorBlock::CursorBlock(block_type type) {
    block = type;
    pos = fvec3(0,0,0);
}

// --- PlaceableObject
bool CursorBlock::set_blocks(Player* player, World* world, SuperObject* object) {
    ivec3 locked_pos;
    // TODO change this to another orientation class
    BlockOrientation orientation;
    ivec3 upper(1, 1, 1);
    if (get_pointing_position(&locked_pos, &orientation, upper)) {
        fvec3 pos = fvec3(locked_pos.x, locked_pos.y, locked_pos.z);
        if (object->get_block(pos.x, pos.y, pos.z).type) {
            // there's already a block here!
            return false;
        }
        object->set_block(pos.x, pos.y, pos.z, block);
        return true;
    }
    return false;
}

// --- CursorItem ---
bool CursorBlock::clicked(Game* game, Action mouse) {
    if (mouse == CLICK_DESTROY) {
        // attempt to remove a block...
        ivec4 looking_at;
        if (get_look_at(&looking_at)) {
            ivec3 at_pos(looking_at.x,looking_at.y,looking_at.z);
            game->world->kill_block(&at_pos);
        }
    }
    else if (mouse == CLICK_CREATE) {
        PlaceableObject::set_blocks(game);
    }
    return true;
}

bool CursorBlock::confirmed(Game* game) {
    return false;
}

void CursorBlock::render_item() {
    if (!mvp_set) {
        ivec3 upper(1, 1, 1);
        set_mvp(upper);
    }
    fvec3 old_pos = pos;
    pos = fvec3(0, 0, 0);
    render_block(&mvp);
    pos = old_pos;
}

void CursorBlock::render_in_world(fmat4* transform) {
    ivec3 locked_pos;
    // TODO change this to another orientation class
    BlockOrientation orientation;
    ivec3 upper(1, 1, 1);
    if (get_pointing_position(&locked_pos, &orientation, upper)) {
        pos = fvec3(locked_pos.x, locked_pos.y, locked_pos.z);
    }
    else {
        return;
    }
    render_block(transform);
}

cursor_item_identifier CursorBlock::get_identifier() {
    cursor_item_identifier val;
    val.is_blk = true;
    val.is_recipe = false;
    val.bid = block.type;
    val.pid = 0;
    val.vid = 0;
    return val;
}

void CursorBlock::render_block(fmat4* transform) {
    
    // Render a box around the block we are pointing at
    GLbyte box[36][3];
    GLbyte box_texture[36][3];
    
    int i = 0;
    
    set_coord_and_texture(box, box_texture, i++, 0, 0, 0, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, 0, 0, 1, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::BACK);
    
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, 0, 0, 1, block, BlockOrientation::BACK);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::BACK);
    
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 0, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::FRONT);
    
    set_coord_and_texture(box, box_texture, i++, 1, 1, 0, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 1, block, BlockOrientation::FRONT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::FRONT);
    
    // y
    set_coord_and_texture(box, box_texture, i++, 0, 0, 0, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, 0, 0, 1, block, BlockOrientation::BOTTOM);
    
    set_coord_and_texture(box, box_texture, i++, 0, 0, 1, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::BOTTOM);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::BOTTOM);
    
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 0, block, BlockOrientation::TOP);
    
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 1, block, BlockOrientation::TOP);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 0, block, BlockOrientation::TOP);
    
    // z
    set_coord_and_texture(box, box_texture, i++, 0, 0, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::RIGHT);
    
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 0, block, BlockOrientation::RIGHT);
    
    set_coord_and_texture(box, box_texture, i++, 0, 0, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::LEFT);
    
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::LEFT);
    
    fmat4 view = glm::translate(fmat4(1), pos);
    fmat4 mvp2 = *transform * view;
    
    set_transform_matrix(mvp2, view);
    set_block_draw_mode(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_texture_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
