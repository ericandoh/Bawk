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
#include "game.h"

// variables for current state of program
Display current;

void failed_init() {
    printf("Failed to initialize resources for a game");
    throw 1;
}

// sets up then renders a display
void start_display() {
    switch (current) {
        case GAME: {
            Game game;
            game.set_parameters("testworld", 0);
            if (game.init()) {
                failed_init();
            }
            set_current_game(&game);
            // loop to run the game
            display_run();
            break;
        }
        case EXIT:
            break;
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
    init_display();
    
    // the starting display of the game
    current = GAME;
    
    // if simply switching, display_run will return 0
    // if exitting game, return something else which will trigger program to end
    while (current != EXIT) {
        start_display();
    }
    display_close();
    return 0;
}