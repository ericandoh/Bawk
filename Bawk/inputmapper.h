//
//  inputmapper.h
//  Bawk
//
//  Created by Eric Oh on 5/22/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__inputmapper__
#define __Bawk__inputmapper__

#include <stdio.h>
#include "game_actions.h"
#include "displayable.h"

void initInputMapping();

Action getKeyActionFrom(int key);
Action getMouseActionFrom(int key);

bool isKeyToggleable(int key);
bool isMouseToggleable(int key);

void toggleKey(int key, bool pressed);
void toggleMouse(int key, bool pressed);

void stepInput(Displayable* displayable, int ms);

#endif /* defined(__Bawk__inputmapper__) */
