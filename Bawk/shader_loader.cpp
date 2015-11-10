//
//  shader_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <iostream>
#include "includeglfw.h"
#include "shader_loader.h"
#include "worldrender.h"

// VERSION 1.1
// TODO references to old shaders here

const std::string GEOMETRY_VERTEX_SHADER = "vertex_shader.glsl";
const std::string GEOMETRY_FRAG_SHADER = "frag_shader.glsl";
const std::string LIGHTING_VERTEX_SHADER = "light_vertex_shader.glsl";
const std::string LIGHTING_FRAG_SHADER = "light_frag_shader.glsl";

long get_file_length(std::string file_name) {
    // TODO (handle if file not found)
    // right now will return -1 => can't reserve string lol
    std::ifstream t(file_name, std::ifstream::ate | std::ifstream::binary);
    return t.tellg();
}

// Loads an entire file text into a string
void load_file(std::string& str, std::string file_name) {
    std::ifstream t(file_name, std::ifstream::ate | std::ifstream::binary);
    t.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
}

int compile_program(GLuint* program,
                    const std::string vertex_shader_file,
                    const std::string frag_shader_file) {
    std::string vertexSource;
    std::string fragmentSource;
    
    long vertex_shader_length = get_file_length(vertex_shader_file);
    long frag_shader_length = get_file_length(frag_shader_file);
    
    vertexSource.reserve(vertex_shader_length);
    fragmentSource.reserve(frag_shader_length);
    
    //Read our shaders into the appropriate buffers
    load_file(vertexSource, vertex_shader_file); //Get source code for vertex shader.
    load_file(fragmentSource, frag_shader_file); //Get source code for fragment shader.
    
    //Create an empty vertex shader handle
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    //Send the vertex shader source code to GL
    //Note that std::string's .c_str is NULL character terminated.
    const GLchar *source = (const GLchar *)vertexSource.c_str();
    glShaderSource(vertexShader, 1, &source, 0);
    
    //Compile the vertex shader
    glCompileShader(vertexShader);
    
    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        //The maxLength includes the NULL character
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar *info_log;
        info_log = (GLchar*)malloc(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, info_log);
        fprintf(stderr, "Shader compilation failed: %*s\n", maxLength, info_log);
        free(info_log);
        
        //We don't need the shader anymore.
        glDeleteShader(vertexShader);
        return 1;
    }
    
    //Create an empty fragment shader handle
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    //Send the fragment shader source code to GL
    //Note that std::string's .c_str is NULL character terminated.
    source = (const GLchar *)fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);
    
    //Compile the fragment shader
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        //The maxLength includes the NULL character
        GLchar *info_log;
        info_log = (GLchar*)malloc(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, info_log);
        fprintf(stderr, "Shader compilation failed: %*s\n", maxLength, info_log);
        free(info_log);
        
        //We don't need the shader anymore.
        glDeleteShader(fragmentShader);
        //Either of them. Don't leak shaders.
        glDeleteShader(vertexShader);
        return 1;
    }
    
    //Vertex and fragment shaders are successfully compiled.
    //Now time to link them together into a program.
    //Get a program object.
    *program = glCreateProgram();
    
    //Attach our shaders to our program
    glAttachShader(*program, vertexShader);
    glAttachShader(*program, fragmentShader);
    
    //Link our program
    glLinkProgram(*program);
    
    //Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, (int *)&isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &maxLength);
        
        //The maxLength includes the NULL character
        GLchar *info_log;
        info_log = (GLchar*)malloc(maxLength);
        glGetProgramInfoLog(*program, maxLength, &maxLength, info_log);
        fprintf(stderr, "Program compilation failed: %*s\n", maxLength, info_log);
        free(info_log);
        
        //We don't need the program anymore.
        glDeleteProgram(*program);
        //Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 1;
    }
    //We don't need these anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    return 0;
}

int get_attrib_location(GLuint* dst, const std::string &attribute_name, GLuint program) {
    *dst = glGetAttribLocation(program, attribute_name.c_str());
    if (*dst == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name.c_str());
        return 1;
    }
    return 0;
}

int get_uniform_location(GLuint* dst, const std::string &attribute_name, GLuint program) {
    *dst = glGetUniformLocation(program, attribute_name.c_str());
    if (*dst == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", attribute_name.c_str());
        return 1;
    }
    return 0;
}

// call this to bind attribute names 
int bind_geometry_shader_attributes(GLuint program) {
    int errors = 0;
    
    errors += get_attrib_location(&geometry_coord, "g_coord", program);
    errors += get_attrib_location(&geometry_texture_coord, "g_texturecoord", program);
    
    errors += get_uniform_location(&geometry_mvp, "g_mvp", program);
    errors += get_uniform_location(&geometry_world_transform, "g_worldtransform", program);
    errors += get_uniform_location(&geometry_draw_mode, "g_drawmode", program);
    errors += get_uniform_location(&geometry_intensity, "g_intensity", program);
    errors += get_uniform_location(&geometry_tile_texture, "tile_texture", program);
    
    glEnableVertexAttribArray(geometry_coord);
    glEnableVertexAttribArray(geometry_texture_coord);
    
    if (errors) {
        printf("Could not bind one of the above variables. Aborting\n");
        return errors;
    }
    printf("Done loading geometry shaders: %d\n", glGetError());
    return 0;
}

// call this to bind attribute names
int bind_light_shader_attributes(GLuint program) {
    int errors = 0;
    
    errors += get_attrib_location(&lighting_coord, "l_coord", program);
    errors += get_uniform_location(&lighting_mvp, "l_mvp", program);
    
    errors += get_uniform_location(&lighting_position_map, "g_position_map", program);
    errors += get_uniform_location(&lighting_color_map, "g_color_map", program);
    errors += get_uniform_location(&lighting_color_t_map, "g_color_t_map", program);
    //errors += get_uniform_location(&lighting_normal_map, "g_normal_map", program);
    
    errors += get_uniform_location(&lighting_screen_size, "l_screen_size", program);
    //errors += get_uniform_location(&lighting_val, "l_val", program);
    errors += get_uniform_location(&lighting_draw_mode, "l_draw_mode", program);
    
    glEnableVertexAttribArray(lighting_coord);
    
    if (errors) {
        printf("Could not bind one of the above variables. Aborting\n");
        return errors;
    }
    printf("Done loading lighting shaders: %d\n", glGetError());
    return 0;
}

int set_shaders() {
    if (compile_program(&geometry_program, GEOMETRY_VERTEX_SHADER, GEOMETRY_FRAG_SHADER))
        return 1;
    if (bind_geometry_shader_attributes(geometry_program))
        return 1;
    
    if (compile_program(&lighting_program, LIGHTING_VERTEX_SHADER, LIGHTING_FRAG_SHADER))
        return 1;
    if (bind_light_shader_attributes(lighting_program))
        return 1;
    return 0;
}