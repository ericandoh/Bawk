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
    fvec3 viewpoint_pos;
    Rotation viewpoint_angle;
    void update_viewpoint();
public:
    bool viewpoint;
    RenderablePlayer();
    
    // --- RenderablePlayer ---
    virtual bounding_box get_bounds_for_viewing();
    void set_camera();
    void set_viewpoint(bool v);
    void query_depth(World* world);
    
    fvec3 get_viewpoint();
    Rotation* get_viewpoint_angle();
    // render the player, and renders cursor/some UI elements which shouldnt be here....
    // TOFU fix the above
    void render();
};

#endif /* defined(__Bawk__playerrender__) */