//
//  cursorscantool.cpp
//  Bawk
//
//  Created by Eric Oh on 1/2/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "cursorscantool.h"
#include "blocktracer.h"
#include "worldrender.h"
#include "uirenderhelper.h"

GLfloat scan_tool_box[36][3] = {
    {-1,-1,-1},
    {-1,-1, 1},
    {-1, 1, 1},
    {1, 1,-1},
    {-1,-1,-1},
    {-1, 1,-1},
    {1,-1, 1},
    {-1,-1,-1},
    {1,-1,-1},
    {1, 1,-1},
    {1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1, 1, 1},
    {-1, 1,-1},
    {1,-1, 1},
    {-1,-1, 1},
    {-1,-1,-1},
    {-1, 1, 1},
    {-1,-1, 1},
    {1,-1, 1},
    {1, 1, 1},
    {1,-1,-1},
    {1, 1,-1},
    {1,-1,-1},
    {1, 1, 1},
    {1,-1, 1},
    {1, 1, 1},
    {1, 1,-1},
    {-1, 1,-1},
    {1, 1, 1},
    {-1, 1,-1},
    {-1, 1, 1},
    {1, 1, 1},
    {-1, 1, 1},
    {1,-1, 1}
};

const fvec3 SELECTING_COLOR = fvec3(0.5f, 0.0f, 0.0f);
const fvec3 SELECTED_COLOR = fvec3(0.0f, 0.5f, 0.0f);
const fvec3 EXTENDED_COLOR = fvec3(0.0f, 0.0f, 0.5f);

CursorScanTool::CursorScanTool() {
    current_stage = ScanStages::SETTING_LOWER;
    show_item = false;
}

void CursorScanTool::reset() {
    current_stage = ScanStages::SETTING_LOWER;
    show_item = false;
}

bool CursorScanTool::clicked(Game* game, Action mouse) {
    if (current_stage == ScanStages::SETTING_LOWER) {
        // we've selected a lower corner, upgrade
        if (show_item) {
            current_stage = ScanStages::SETTING_UPPER;
        }
    }
    else if (current_stage == ScanStages::SETTING_UPPER) {
        if (show_item) {
            current_stage = ScanStages::SELECTED;
        }
    }
    else if (current_stage == ScanStages::EXTENDING) {
        // do nothing?
        if (show_item) {
            current_stage = ScanStages::EXTENDED;
        }
    }
    else if (current_stage == ScanStages::SELECTED) {
        if (mouse == Action::CLICK_MAIN) {
            // reset
            current_stage = ScanStages::SETTING_LOWER;
        }
    }
    return true;
}

bool CursorScanTool::pushed(Game* game, Action key) {
    if (key == Action::MOUNTING) {
        if (current_stage == ScanStages::SELECTED) {
            current_stage = ScanStages::EXTENDING;
            extending_box = box;
            return true;
        }
    }
    return false;
}

bool CursorScanTool::confirmed(Game* game) {
    if (current_stage == ScanStages::SELECTED) {
        // TODO copy into
        current_stage = ScanStages::SETTING_LOWER;
    }
    else if (current_stage == ScanStages::EXTENDED) {
        // TODO find new int_bounding_box
        // for every coordinate in new BB not in old BB
        // find modded coordinate in the old BB and fill with selected block
        
        current_stage = ScanStages::SELECTED;
    }
    current_stage = ScanStages::SETTING_LOWER;
    return true;
}

bool CursorScanTool::canceled(Game* game) {
    if (current_stage == ScanStages::EXTENDING ||
        current_stage == ScanStages::EXTENDED) {
        current_stage = ScanStages::SELECTED;
    }
    else if (current_stage == ScanStages::SETTING_LOWER) {
        return false;
    }
    else {
        current_stage = ScanStages::SETTING_LOWER;
    }
    return true;
}

bool CursorScanTool::handle_movement(ivec3 dir) {
    // TODO
    BlockOrientation player_orientation = get_player_direction();
    fvec3 player_dir = get_direction_from_orientation(player_orientation);
    
    
    return true;
}

void CursorScanTool::step(Game* game) {
    if (current_stage == ScanStages::SETTING_LOWER ||
        current_stage == ScanStages::SETTING_UPPER ||
        current_stage == ScanStages::EXTENDING) {
        if (BlockTracing::show_item) {
            show_item = true;
            if (current_stage == ScanStages::SETTING_LOWER) {
                lower = BlockTracing::pointed_rounded_pos;
                box.lower = lower;
                box.upper = ivec3(lower.x + 1,
                                  lower.y + 1,
                                  lower.z + 1);
            }
            else if (current_stage == ScanStages::SETTING_UPPER) {
                upper = BlockTracing::pointed_rounded_pos;
                box.lower = lower;
                box.upper = upper;
                box.refit();
                box.upper += 1;
            }
            else if (current_stage == ScanStages::EXTENDING) {
                // TODO calculate extending box
                extended = BlockTracing::pointed_rounded_pos;
                extending_box.lower = lower;
                extending_box.upper = upper;
                extending_box.refit();
                extending_box.expand(extended);
                extending_box.upper += 1;
            }
            player_orientation = get_player_direction();
        }
        else {
            show_item = false;
            if (current_stage == ScanStages::SETTING_UPPER) {
                box.upper = ivec3(lower.x + 1,
                                  lower.y + 1,
                                  lower.z + 1);
            }
        }
    }
}

void CursorScanTool::render_item() {
    // TODO
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::UV);
    //UIHelper::draw_square_color(SELECTED_COLOR);
    UIHelper::draw_square_voxel(255);
}

void render_colored_box(fmat4* transform, int_bounding_box box, fvec3 color, float offset=0.125f) {
    
    GLfloat box_texture[36][3];
    for (int i = 0; i < 36; i++) {
        box_texture[i][0] = color.x;
        box_texture[i][1] = color.y;
        box_texture[i][2] = color.z;
    }
    
    // apply rest of xformations
    // move box such that center pos will be in right place
    fvec3 center_pos = box.get_center_pos();
    fmat4 view = glm::translate(fmat4(1), center_pos);
    // scale box to dimensions
    ivec3 dimensions = box.get_dimensions();
    // add offset so our box "floats" above the actual blocks
    view = glm::scale(view, fvec3(dimensions.x / 2.0f + offset,
                                  dimensions.y / 2.0f + offset,
                                  dimensions.z / 2.0f + offset));
    
    view = *transform * view;
    
    OGLAttr::current_shader->set_transform_matrix(&view);
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR_TRANSPARENT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof scan_tool_box, scan_tool_box, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CursorScanTool::render_in_world(fmat4* transform) {
    if (current_stage == ScanStages::SELECTED) {
        // render the box normally
        render_colored_box(transform, box, SELECTED_COLOR);
    }
    else if (current_stage == ScanStages::SETTING_LOWER) {
        // render a single box that follows just that lower corner, if show_item is true
        if (show_item) {
            render_colored_box(transform, box, SELECTING_COLOR);
        }
    }
    else if (current_stage == ScanStages::SETTING_UPPER) {
        // if show_item, render like SELECTED
        // else, render like SETTING_LOWER
        render_colored_box(transform, box, SELECTING_COLOR);
    }
    else if (current_stage == ScanStages::EXTENDING) {
        // render the current selected box
        // then if show_selected, render a slightly bigger box showing the extended box
        //render_colored_box(transform, box, SELECTED_COLOR);
        if (show_item) {
            render_colored_box(transform, extending_box, EXTENDED_COLOR);
        }
    }
    else if (current_stage == ScanStages::EXTENDED) {
        //render_colored_box(transform, box, SELECTED_COLOR);
        render_colored_box(transform, extending_box, EXTENDED_COLOR);
    }
}

cursor_item_identifier CursorScanTool::get_identifier() {
    // TODO
    cursor_item_identifier identifier;
    identifier.vid = 0;
    identifier.pid = 0;
    
    return identifier;
}