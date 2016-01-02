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
#include "game_info_loader.h"

CursorBlock::CursorBlock(block_type type) {
    block = type;
    pos = fvec3(0,0,0);
    show_item = false;
}

// --- PlaceableObject
bool CursorBlock::set_blocks(Player* player, World* world, SuperObject* object) {
    if (show_item) {
        block_type* blk = object->get_block_integral(pos.x, pos.y, pos.z);
        if (blk && blk->type) {
            // there's already a block here!
            return false;
        }
        if (block.orientation == BlockOrientation::TOP || block.orientation == BlockOrientation::BOTTOM) {
            BlockOrientation player_direction = get_player_direction();
            block.orientation = player_direction;
        }
        object->set_block_integral(pos.x, pos.y, pos.z, block);
        return true;
    }
    return false;
}

// --- CursorItem ---
bool CursorBlock::clicked(Game* game, Action mouse, Entity* on) {
    if (mouse == CLICK_SECONDARY) {
        return PlaceableObject::set_blocks(game);
    }
    return true;
}

bool CursorBlock::confirmed(Game* game) {
    return false;
}

void CursorBlock::step() {
    // track item
    ivec3 locked_pos;
    BlockOrientation orientation;
    ivec3 upper(1, 1, 1);
    if (get_pointing_position(&locked_pos, &orientation, upper)) {
        show_item = true;
        pos = fvec3(locked_pos.x, locked_pos.y, locked_pos.z);
        block.orientation = get_player_direction();
    }
    else {
        show_item = false;
    }
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
    if (show_item) {
        render_block(transform);
    }
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
    
    // x
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
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::RIGHT);
    
    set_coord_and_texture(box, box_texture, i++, 1, 0, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 0, block, BlockOrientation::RIGHT);
    set_coord_and_texture(box, box_texture, i++, 1, 1, 0, block, BlockOrientation::RIGHT);
    
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 0, 0, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::LEFT);
    
    set_coord_and_texture(box, box_texture, i++, 1, 1, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 0, 1, 1, block, BlockOrientation::LEFT);
    set_coord_and_texture(box, box_texture, i++, 1, 0, 1, block, BlockOrientation::LEFT);
    
    fmat4 view = glm::translate(fmat4(1), pos);
    view = *transform * view;
    
    OGLAttr::current_shader->set_transform_matrix(&view);
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::VOXEL);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_BYTE);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_BYTE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CursorBlock::render_light_in_world(fmat4* transform, fvec3 player_pos) {
    // if block has light, render it
    // TODO
    if (show_item) {
        RenderableLight* light = get_block_light(block.type);
        light->render_light(transform, pos, player_pos);
    }
}
