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
    BlockOrientation orientation;
    ivec3 upper(1, 1, 1);
    if (get_pointing_position(&locked_pos, &orientation, upper)) {
        if (object->get_block_integral(locked_pos.x, locked_pos.y, locked_pos.z).type) {
            // there's already a block here!
            return false;
        }
        if (orientation == BlockOrientation::TOP || orientation == BlockOrientation::BOTTOM) {
            BlockOrientation player_direction = get_player_direction();
            block.orientation = player_direction;
        }
        else {
            block.orientation = orientation;
        }
        object->set_block_integral(locked_pos.x, locked_pos.y, locked_pos.z, block);
        return true;
    }
    return false;
}

// --- CursorItem ---
bool CursorBlock::clicked(Game* game, Action mouse, Entity* on) {
    if (mouse == CLICK_CREATE) {
        return PlaceableObject::set_blocks(game);
    }
    return true;
}

bool CursorBlock::confirmed(Game* game) {
    return false;
}

void CursorBlock::render_item() {
    if (!mvp_set) {
        ivec3 upper(1, 1, 1);
        calculate_mvp(upper);
    }
    fvec3 old_pos = pos;
    pos = fvec3(0, 0, 0);
    set_mvp();
    fmat4 one(1);
    render_block(&one);
    pos = old_pos;
}

void CursorBlock::render_in_world(fmat4* transform) {
    ivec3 locked_pos;
    BlockOrientation orientation;
    ivec3 upper(1, 1, 1);
    if (get_pointing_position(&locked_pos, &orientation, upper)) {
        pos = fvec3(locked_pos.x, locked_pos.y, locked_pos.z);
        block.orientation = orientation;
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
    view = *transform * view;
    
    set_transform_matrix(&view);
    set_block_draw_mode(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_texture_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
