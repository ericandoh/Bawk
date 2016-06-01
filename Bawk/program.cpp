//
//  program.cpp
//  Bawk
//
//  Created by Eric Oh on 9/30/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "program.h"

#include "display.h"
#include "displayable.h"

#include "client.h"

#include "client_accessor.h"
#include "engine_accessor.h"

// variables for current state of program
Display current;


void failed_init() {
    printf("Failed to initialize resources for a game");
    throw 1;
}

// sets up then renders a display
void start_display() {
    switch (current) {
        case TITLE: {
            /*
            TitleScreen title_screen;   // TitleScreen implements Displayable
            set_current_displayable(&title_screen);
            */
            break;
        }
        case LOADING: {
            /*
             LoadingScreen loading_screen;  // LoadingScreen implements Displayable
             set_current_displayable(&loading_screen);
             */
            break;
        }
        case SP_GAME: {
            Client client = Client();
            Engine engine = Engine();
            set_engine(&engine);
            set_client(&client);
            setup_for_singleplayer();
            if (engine.start("testworld")) {
                failed_init();
            }
            uint32_t player_id = 1;
            
            register_player(player_id);
            client.init();
            set_displayable(&client);
            // loop to run the game
            display_run();
            deregister_current_player();
            break;
        }
        case MP_GAME: {
            /*
            GameClient game;
            game.set_parameters("server_name", etc);
            Client client;
            client.link_to_game(&game);
            set_current_displayable(&client);
            display_run();
             
            // and on server side
            Engine game_engine;
            if (game_engine.start_mp("testworld")) {
                failed_init();
            }
            */
            break;
        }
        case EXIT: {
            break;
        }
        default: {
            break;
        }
    }
}

// called externally by other functions to trigger a display switch
void switch_display(Display toDisplay) {
    if (current == toDisplay and current != EXIT) {
        return;
    }
    close_render_loop();
    current = toDisplay;
}

void exit_game() {
    switch_display(EXIT);
}

int run_game() {
    // initializes the display for the game
    if (init_display())
        return 1;
    
    // the starting display of the game
    current = SP_GAME;
    
    // if simply switching, display_run will return 0
    // if exitting game, return something else which will trigger program to end
    //while (current != EXIT) {
    start_display();
    //}
    display_close();
    return 0;
}