//
//  lightrender.cpp
//  Bawk
//
//  Created by Eric Oh on 12/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "lightrender.h"
#include "includeglfw.h"
#include <glm/gtc/matrix_transform.hpp>
#include "worldrender.h"

// Render a box around the block we are pointing at
/*GLbyte light_render_box[36][3] = {
    {-1,-1,-1},
    {-1,-1,1},
    {-1,1,-1},
    
    {-1,1,-1},
    {-1,-1,1},
    {-1,1,1},
    
    {1,-1,-1},
    {1,1,-1},
    {1,-1,1},
    
    {1,1,-1},
    {1,1,1},
    {1,-1,1},
    
    {-1,-1,-1},
    {1,-1,-1},
    {-1,-1,1},
    
    {-1,-1,1},
    {1,-1,-1},
    {1,-1,1},
    
    {-1,1,-1},
    {-1,1,1},
    {1,1,-1},
    
    {-1,1,1},
    {1,1,1},
    {1,1,-1},
    
    {-1,-1,-1},
    {1,-1,-1},
    {-1,1,-1},
    
    {-1,1,-1},
    {1,-1,-1},
    {1,1,-1},
    
    {-1,-1,1},
    {-1,1,1},
    {1,-1,1},
    
    {-1,1,1},
    {1,1,1},
    {1,-1,1},
};*/

GLbyte light_render_box[36][3] = {
    {-1,-1,-1},
     {-1,-1, 1},
     {-1, 1, 1},
     {1, 1,-1},
     {-1,-1,-1},
     {-1, 1,-1},
     {1,-1, 1},
     {-1,-1,-1},
     {1,-1,-1},
     {1, 1,-1},
     {1,-1,-1},
     {-1,-1,-1},
     {-1,-1,-1},
     {-1, 1, 1},
     {-1, 1,-1},
     {1,-1, 1},
     {-1,-1, 1},
     {-1,-1,-1},
     {-1, 1, 1},
     {-1,-1, 1},
     {1,-1, 1},
     {1, 1, 1},
     {1,-1,-1},
     {1, 1,-1},
     {1,-1,-1},
     {1, 1, 1},
     {1,-1, 1},
     {1, 1, 1},
     {1, 1,-1},
     {-1, 1,-1},
     {1, 1, 1},
     {-1, 1,-1},
     {-1, 1, 1},
     {1, 1, 1},
     {-1, 1, 1},
     {1,-1, 1}
};

RenderableLight::RenderableLight() {
    light_radius = 0.0f;
    light_intensity = 0.0f;
}

void RenderableLight::render_light(fvec3 rwc, fvec3 player_pos) {
    // renders a light at position p
    
    fvec3 player_diff = rwc - player_pos;
    bool cull_backface = true;
    if (fabsf(player_diff.x) >= light_radius ||
        fabsf(player_diff.y) >= light_radius ||
        fabsf(player_diff.z) >= light_radius) {
        //enable backface culling, which we think is done by default
    }
    else {
        cull_backface = false;
        glDisable(GL_CULL_FACE);
    }
    
    fmat4 view(1);
    view = glm::translate(view, rwc);
    view = glm::scale(view, fvec3(light_radius, light_radius, light_radius));
    set_lighting_transform_matrix(&view);
    set_lighting_block_draw_mode(1);
    set_lighting_val(rwc);
    set_lighting_properties(light_radius, light_intensity);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof light_render_box, light_render_box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    if (!cull_backface) {
        // re-enable cull backface
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
}

void RenderableLight::render_light(fmat4* transform, fvec3 p, fvec3 player_pos) {
    fvec4 result = *transform * fvec4(p.x,p.y,p.z,1.0f);
    render_light(fvec3(result.x,result.y,result.z), player_pos);
}

bool RenderableLight::should_render() {
    return light_intensity != 0.0f;
}