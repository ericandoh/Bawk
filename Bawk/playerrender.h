//
//  Render for player
//  - Sets + Readies camera for rendering
//  -
//  -
//
//  Used by:
//  - game.cpp
//
//  Uses:
//  - OpenGL
//  -
//  -
//
//  Notes
//  - Does NOT render the player! Only sets the camera transformation
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__playerrender__
#define __Bawk__playerrender__

#include <stdio.h>
#include "importopengl.h"
#include "entity.h"
#include "math.h"

class World;

class RenderablePlayer: public Entity {
    fmat4 mvp;
    fmat4 view, projection;
    // temp variables related to viewpoint
    bool viewpoint;
public:
    RenderablePlayer();
    
    // --- RenderablePlayer ---
    virtual bounding_box get_bounds_for_viewing();
    void set_camera(bool viewpoint);
    void query_depth(World* world);
    
    fvec3 get_viewpoint();
    // render the player, and renders cursor/some UI elements which shouldnt be here....
    // TOFU fix the above
    void render();
};

#endif /* defined(__Bawk__playerrender__) */