//
//  modelrender.h
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modelrender__
#define __Bawk__modelrender__

#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include "basic_types.h"
#include "blockaction.h"
#include "lightrender.h"

class Game;

class RenderableModel {
public:
    std::string name;
    int resistance;
    int weight;
    int vehicle;
    RenderableLight light;
    
    bounding_box bounds;
    fvec3 center_pos;
    std::vector<fvec3> model_vertices;
    std::vector<fvec3> model_normals;
    std::vector<fvec3> model_uvs;
    
    std::map<Action, model_callback_func> mouse_callback;
    std::map<Action, model_callback_func> keyboard_callback;
    
    RenderableModel();
    
    // --- RenderableModel ---
    void render();
    void refresh();
    
    bool model_keyboard_callback(Game* game, Entity* owner, Entity* ent, Action key);
    bool model_mouse_callback(Game* game, Entity* owner, Entity* ent, Action key);
};

#endif /* defined(__Bawk__modelrender__) */
