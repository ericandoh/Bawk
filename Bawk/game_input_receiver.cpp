//
//  game_input_receiver.cpp
//  Bawk
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "game_input_receiver.h"
#include "client_accessor.h"
#include "common_accessor.h"
#include "gametemplate.h"
#include "cursorsuperobject.h"
#include "program.h"
#include "display.h"
#include "debug_action.h"

float GameInputReceiver::get_player_speed(int ms) {
    return get_player()->weight * 5.0f * convert_milli_to_sec(ms);
}

bool GameInputReceiver::key_callback(Action do_this, int ms) {
    if (do_this == MOVE_UP) {
        get_player()->move_up_flat(get_player_speed(ms));
    }
    else if (do_this == MOVE_DOWN) {
        get_player()->move_down_flat(get_player_speed(ms));
    }
    else if (do_this == MOVE_LEFT) {
        get_player()->move_left(get_player_speed(ms));
    }
    else if (do_this == MOVE_RIGHT) {
        get_player()->move_right(get_player_speed(ms));
    }
    else if (do_this == MOVE_FORWARD) {
        get_player()->move_forward_flat(get_player_speed(ms));
    }
    else if (do_this == MOVE_BACKWARD) {
        get_player()->move_backward_flat(get_player_speed(ms));
    }
    else if (do_this == CONFIRM) {
        if (get_player()->game_template) {
            printf("Exporting template to current cursor!\n");
            if (get_client()->bar->can_set_current()) {
                GameTemplate* gtemplate = get_player()->game_template;
                get_client()->bar->set_current(gtemplate->create_from_template());
                gtemplate->remove_self();
                gtemplate = 0;
            }
        }
        else {
            printf("Making new template\n");
            // create a new place_info
            get_player()->game_template = new GameTemplate();
            get_world()->add_entity(get_player()->game_template);
        }
    }
    else if (do_this == CANCEL) {
        if (get_player()->game_template) {
            printf("Cancelling current template\n");
            get_player()->game_template->remove_self();
            get_player()->game_template = 0;
        }
        else if (get_client()->default_item->canceled()) {
            ;
        }
        else {
            printf("Escape pressed with no context. Exitting\n");
            exit_game();
        }
    }
    else if (do_this == OPEN_INV) {
        get_client()->story->toggle_child(get_client()->inventory_ui);
        if (get_client()->story->count_children() > 1) {
            get_client()->inventory_ui->refresh();
            display_enable_cursor();
        }
        else {
            display_disable_cursor();
        }
    }
    else if (do_this == DEBUG_ACTION) {
        // use this for debugging only
        debug_action(get_client());
    }
    else if (do_this == TOGGLE_VIEWPOINT) {
        get_player()->set_viewpoint(!get_player()->viewpoint);
    }
    else if (do_this == DELETE) {
        get_client()->default_item->pushed(do_this);
    }
    else {
        return false;
    }
    return true;
}

bool GameInputReceiver::mouse_move_callback(double xdiff, double ydiff) {
    get_client()->toggle_player_movement(xdiff, ydiff);
    return true;
}

bool GameInputReceiver::mouse_clicked_callback(Action do_this) {
    // block mouse callback has been disabled for now
    // we can still have model blocks but they should not be actionable
    // (what about storage blocks???)
    /*
    if (BlockTracing::show_item) {
        Entity* src = BlockTracing::selected->find_top_level_parent();
        if (BlockTracing::selected->block_mouse_callback(get_player(), do_this, src)) {
            return;
        }
    }
    */
    return get_client()->default_item->clicked(do_this);
}

bool GameInputReceiver::mouse_clicking_callback(Action do_this, int ms) {
    return get_client()->default_item->clicking(do_this, ms);
}

bool GameInputReceiver::scroll_callback(double xdiff, double ydiff) {
    return false;
}