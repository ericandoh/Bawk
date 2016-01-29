//
//  textrender.h
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__textrender__
#define __Bawk__textrender__

#include <stdio.h>
#include <string>

/*
 
 B E C A U S E
 
 F U C K
 
 F O N T      L I B R A R I E S
 
 
 \   /
  O O
   V
 
 */

void init_string_resources();
// render a string with top left corner at x, y
void render_string(std::string text, int x, int y, int height=32);
// render a string within a box specified by xl, yl, xu, yu
void render_string(std::string text, int xl, int yl, int xu, int yu, int height=32);
void clean_string_resources();

#endif /* defined(__Bawk__textrender__) */
