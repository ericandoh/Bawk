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

CursorBlock::CursorBlock(CursorItemInfo* i): CursorItem(i) {
    pos = fvec3(0,0,0);
    orientation = BlockOrientation::FRONT;
}

// --- PlaceableObject
bool CursorBlock::set_blocks(Player* player, World* world, SuperObject* object) {
    if (show_item) {
        if (info->count <= 0) {
            // no blocks left to place
            return false;
        }
        block_type* blk = object->get_block_integral(pos.x, pos.y, pos.z);
        if (blk && blk->type) {
            // there's already a block here!
            return false;
        }
        info->count -= 1;
        block_type placeme = block_type((uint16_t)info->vid, orientation, player->pid);
        object->set_block_integral(pos.x, pos.y, pos.z, placeme);
        printf("Placing block at: ");
        printf_fvec3(pos);
        printf("\n");
        return true;
    }
    return false;
}

// --- CursorItem ---
void CursorBlock::reset() {
    show_item = false;
}

bool CursorBlock::clicked(Game* game, Action mouse) {
    if (mouse == CLICK_SECONDARY) {
        set_blocks(game->player, game->world, target);
        return true;
    }
    return false;
}

bool CursorBlock::confirmed(Game* game) {
    return false;
}

void CursorBlock::step(Game* game, int ms) {
    update_pointing_position(game, ivec3(1,1,1));
    pos = fvec3(pointing_pos.x, pointing_pos.y, pointing_pos.z);
    orientation = pointing_orientation;
    if (orientation == BlockOrientation::TOP || orientation == BlockOrientation::BOTTOM) {
        BlockOrientation player_direction = get_player_direction();
        orientation = player_direction;
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

void CursorBlock::render_block(fmat4* transform) {
    
    // Render a box around the block we are pointing at
    GLbyte box[36][3];
    GLbyte box_texture[36][3];
    
    int i = 0;
    
    block_type block = block_type((uint16_t)info->vid, orientation, 0);
    
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
    if (show_item) {
        uint16_t block_type = (uint16_t)info->vid;
        RenderableLight* light = &(get_block_info(block_type)->light);
        light->render_light(transform, pos, player_pos);
    }
}

bool CursorBlock::has_count() {
    return true;
}
