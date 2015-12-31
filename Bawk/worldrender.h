//
//  Contains and loads/frees assets related to rendering the world
//  - OpenGL functions here
//  - Has assets global to rendering (not specific world rendering items)
//  - Actual world rendering is done at chunk-level, which will use assets from this class
//    (this is to preserve hierarchy)
//  - Loads/Requests resources at beginning, and frees them at end
//
//  Used by:
//  - world.cpp
//
//  Uses:
//  - OpenGL library
//
//  Notes
//  - If we end up switching to another rendering program or using a voxel engine we have to
//    change this class only
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__worldrender__
#define __Bawk__worldrender__

#include <stdio.h>
#include "importopengl.h"
#include "basic_types.h"
#include "superobject.h"
#include "block.h"

// --- Some Globals ---

class ShaderProgram {
public:
    GLuint program;
    
    GLuint coord;
    GLuint mvp;
    
    virtual void set_transform_matrix(fmat4* view);
};

enum BlockDrawMode {
    UV = 0, VOXEL = 1, COLOR = 2
};

// NONE = render scene without any lighting correction
// BASE = light with directional/shadows/ambient
// POINT = point light
// LIGHT_TEST = render texcoord of shadowmap directly
enum LightDrawMode {
    NONE = 0, BASE = 1, POINT = 2, LIGHT_TEST = 3
};

class FirstPassShaderProgram: public ShaderProgram {
public:
    void set_coord_attribute(GLenum type);
    virtual void set_texture_coord_attribute(GLenum type) { };
    virtual void set_block_draw_mode(BlockDrawMode v) { };
    virtual void set_shader_intensity(float m) { };
    virtual void set_alpha_cutoff(float a) { };
    virtual void set_enable_shadows(bool v) { };
};

// stage 1 of the deferred rendering
// geometry pass
class GeometryShaderProgram: public FirstPassShaderProgram {
public:
    GLuint texture_coord;
    GLuint world_transform;
    GLuint draw_mode;
    GLuint intensity;
    GLuint alphacutoff;
    GLuint enable_shadows;
    GLuint tile_texture;
    
    void set_transform_matrix(fmat4* view) override;
    void set_texture_coord_attribute(GLenum type) override;
    void set_block_draw_mode(BlockDrawMode v) override;
    void set_shader_intensity(float m) override;
    void set_alpha_cutoff(float a) override;
    void set_enable_shadows(bool v) override;
};

// stage 2 of the deferred rendering
// light pass
class LightingShaderProgram: public ShaderProgram {
public:
    GLuint position_map;
    GLuint color_map;
    GLuint color_t_map;
    GLuint normal_map;
    
    GLuint shadow_map;
    GLuint shadow_mvp;
    
    GLuint screen_size;
    GLuint val;
    GLuint properties;
    GLuint draw_mode;
};

// depth buffer write from POV of light
// shadow mapping
class ShadowShaderProgram: public FirstPassShaderProgram {
public:
};

namespace OGLAttr {
    // vertex array object (for everythings!)
    extern GLuint vao;
    
    // handle to texture
    extern GLuint tile_texture;
    extern GLuint active_tile_texture;
    
    extern GLuint common_vertex_vbo;
    extern GLuint common_texture_vbo;
    
    extern GeometryShaderProgram geometry_shader;
    extern LightingShaderProgram lighting_shader;
    extern ShadowShaderProgram shadow_shader;
    
    // current first pass shader we're using
    extern FirstPassShaderProgram* current_shader;
}

int world_load_resources();
void world_free_resources();

void set_geometry_as_current_shader();
void set_shadow_as_current_shader();

void set_camera_transform_matrix(fmat4* camera);
void set_unitary_transform_matrix();

// calls glVertexAttributePointer assuming the data's been loaded into a VBO
void set_coord_attribute(GLenum type);
void set_texture_coord_attribute(GLenum type);

fvec4 apply_mvp_matrix(fmat4* view, fvec4 a);

void set_lighting_block_draw_mode(LightDrawMode v);
void set_lighting_val(fvec3 val);
void set_lighting_properties(float light_radius, float light_intensity);
void set_ambient_lighting_properties(float light_intensity);
void set_directional_lighting_transform_matrix(fmat4* light_mvp);
void set_lighting_transform_matrix(fmat4* view);
void set_unitary_lighting_transform_matrix();
void set_lighting_screen_size(float width, float height);

#endif /* defined(__Bawk__worldrender__) */