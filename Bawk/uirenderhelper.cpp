//
//  uirenderhelper.cpp
//  Bawk
//
//  Created by Eric Oh on 1/5/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "uirenderhelper.h"
#include "worldrender.h"

using namespace UIHelper;

namespace UIHelper {
    
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    
    void draw_square_color(fvec3 color) {
        OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
        float texture[6][3];
        for (int p = 0; p < 6; p++) {
            texture[p][0] = color.x;
            texture[p][1] = color.y;
            texture[p][2] = color.z;
        }
        
        set_unitary_transform_matrix();
        
        glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
        OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
        
        glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
        OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    void draw_square_voxel(int voxel) {
        // we will NOT use the voxel shader
        // instead, bind UV directly
        
        int xtex = voxel % TILES_PER_TEXTURE;
        int ytex = voxel / TILES_PER_TEXTURE;
        
        float xl = xtex * 1.0f / TILES_PER_TEXTURE;
        float yl = 1.0f - ytex * 1.0f / TILES_PER_TEXTURE;
        float xu = (xtex + 1.0f) / TILES_PER_TEXTURE;
        float yu = 1.0f - (ytex + 1.0f) / TILES_PER_TEXTURE;
        
        fvec3 uv_coords[4];
        uv_coords[0] = fvec3(xl,yl,0.0f);
        uv_coords[1] = fvec3(xu,yl,0.0f);
        uv_coords[2] = fvec3(xl,yu,0.0f);
        uv_coords[3] = fvec3(xu,yu,0.0f);
        
        draw_square_texture(uv_coords);
    }
    
    void draw_square_texture(fvec3 uv_coords[4]) {
        fvec3 texture[6];
        
        texture[0] = uv_coords[0];
        texture[1] = uv_coords[1];
        texture[2] = uv_coords[2];
        texture[3] = uv_coords[2];
        texture[4] = uv_coords[1];
        texture[5] = uv_coords[3];
        
        set_unitary_transform_matrix();
        OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::UV);
        
        glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
        OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
        
        glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof texture, &(texture[0]), GL_DYNAMIC_DRAW);
        OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}