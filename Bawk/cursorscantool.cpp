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

#include "cursorsuperobject.h"
#include "client_accessor.h"
#include "basicrender.h"

const fvec3 SELECTING_COLOR = fvec3(0.5f, 0.0f, 0.0f);
const fvec3 SELECTED_COLOR = fvec3(0.0f, 0.5f, 0.0f);
const fvec3 EXTENDED_COLOR = fvec3(0.0f, 0.0f, 0.5f);

CursorScanTool::CursorScanTool(CursorItemInfo* i): CursorItem(i) {
    current_stage = ScanStages::SETTING_LOWER;
    show_item = false;
}

void CursorScanTool::reset() {
    current_stage = ScanStages::SETTING_LOWER;
    show_item = false;
}

bool CursorScanTool::clicked(Action mouse) {
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

bool CursorScanTool::pushed(Action key) {
    if (key == Action::MOUNTING) {
        if (current_stage == ScanStages::SELECTED) {
            current_stage = ScanStages::EXTENDING;
            extending_box = box;
            return true;
        }
    }
    return false;
}

bool CursorScanTool::confirmed() {
    if (current_stage == ScanStages::SELECTED) {
        // copies the area selected into the first cursorobject that is available
        // TODO copy entities as well
        CursorItemInfo* info = new CursorItemInfo((uint32_t)get_player()->getID(),
                                                  (uint32_t)get_player()->assignID(),
                                                  CursorType::CURSOR_SUPEROBJECT);
        CursorSuperObject* object = new CursorSuperObject(info);// all templates are made on the bar
        // TODO giraffes (center chunk and set position to box.lower + box.upper)
        object->set_pos(box.lower);
        for (int x = box.lower.x; x < box.upper.x; x++) {
            for (int y = box.lower.y; y < box.upper.y; y++) {
                for (int z = box.lower.z; z < box.upper.z; z++) {
                    block_type* blk = target->get_block_integral(x, y, z);
                    object->set_block_integral(x, y, z, *blk);
                }
            }
        }
        // TOFU what if inventory is open and this gets executed...?
        get_player()->inventory->add_custom(info);
        get_client()->set_first_available(object);
        current_stage = ScanStages::SETTING_LOWER;
    }
    else if (current_stage == ScanStages::EXTENDED) {
        // fills in the extra blocks using the original as a template, over a repeating pattern
        // TODO copy over models in the area as well with equal frequency
        for (int x = extending_box.lower.x; x < extending_box.upper.x; x++) {
            for (int y = extending_box.lower.y; y < extending_box.upper.y; y++) {
                for (int z = extending_box.lower.z; z < extending_box.upper.z; z++) {
                    if (box.hits(ivec3(x,y,z)))
                        continue;
                    int xm = get_positive_mod((x - box.lower.x), (box.upper.x - box.lower.x)) + box.lower.x;
                    int ym = get_positive_mod((y - box.lower.y), (box.upper.y - box.lower.y)) + box.lower.y;
                    int zm = get_positive_mod((z - box.lower.z), (box.upper.z - box.lower.z)) + box.lower.z;
                    
                    block_type* blk = target->get_block_integral(xm, ym, zm);
                    target->set_block_integral(x, y, z, *blk);
                }
            }
        }
        
        current_stage = ScanStages::SELECTED;
    }
    current_stage = ScanStages::SETTING_LOWER;
    return true;
}

bool CursorScanTool::canceled() {
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
    int_bounding_box current_box;
    if (current_stage == ScanStages::EXTENDED) {
        current_box = extending_box;
    }
    else if (current_stage == ScanStages::SELECTED) {
        current_box = box;
    }
    else {
        return false;
    }
    
    // TODO
    BlockOrientation player_orientation = get_player_direction();
    fvec3 player_dir = get_direction_from_orientation(player_orientation);
    
    if (player_dir.x != 0) {
        // use x
        if (player_dir.x < 0) {
            // apply the movement to the FRONT face
            current_box.upper = add_ivec3(dir, current_box.upper);
        }
        else {
            // apply the movement to the BACK face
            current_box.lower = add_ivec3(dir, current_box.lower);
        }
    }
    else if (player_dir.y != 0) {
        // use y
        if (player_dir.y < 0) {
            // apply the movement to the TOP face
            current_box.upper = add_ivec3(dir, current_box.upper);
        }
        else {
            // apply the movement to the BOTTOM face
            current_box.lower = add_ivec3(dir, current_box.lower);
        }
    }
    else {
        // use z
        if (player_dir.z < 0) {
            // apply the movement to the LEFT face
            current_box.upper = add_ivec3(dir, current_box.upper);
        }
        else {
            // apply the movement to the RIGHT face
            current_box.lower = add_ivec3(dir, current_box.lower);
        }
    }
    
    return true;
}

void CursorScanTool::step(int ms) {
    if (current_stage == ScanStages::SETTING_LOWER ||
        current_stage == ScanStages::SETTING_UPPER ||
        current_stage == ScanStages::EXTENDING) {
        if (BlockTracing::show_item) {
            show_item = true;
            
            Entity* at = BlockTracing::selected;
            if (at->entity_class == EntityType::BASEWORLD ||
                at->entity_class == EntityType::SUPEROBJECT ||
                at->entity_class == EntityType::GAMETEMPLATE) {
                target = (SuperObject*)at;
            }
            else {
                target = at->parent;
            }
            
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
    //UIHelper::draw_square_color(SELECTED_COLOR);
    UIHelper::draw_square_voxel(255);
}

void render_colored_box(fmat4* transform, int_bounding_box box, fvec3 color, float offset=0.0625f) {
    bounding_box box_with_offsets;
    box_with_offsets.lower = get_fvec3_from_ivec3(box.lower) + offset;
    box_with_offsets.upper = get_fvec3_from_ivec3(box.upper) + offset;
    draw_colored_cube(transform, &box_with_offsets, color, true);
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
