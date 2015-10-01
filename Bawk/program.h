//
//  Overarching wrapper for the whole program
//  - Multiplexes between the main game, loading screens, starting menus
//  -
//  -
//
//  Used by:
//  - main.cpp
//
//  Uses:
//  - display.h
//  - displayable.h
//  - game.h
//
//  Notes
//  - Has logic so that entire rendering loop of underlying displayable is executed before
//    switching to next displayable
//  - Has logic for clean exits
//
//  Created by Eric Oh on 9/30/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__program__
#define __Bawk__program__

#include <stdio.h>

enum Display {
    GAME, EXIT
};

void switch_display(Display toDisplay);
void exit_game();
int run_game();

#endif /* defined(__Bawk__program__) */
