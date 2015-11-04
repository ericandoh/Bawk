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

const std::string VERTEX_SHADER = "vertex_shader.glsl";
const std::string FRAG_SHADER = "frag_shader.glsl";

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

int set_shaders() {
    printf("Setting shaders\n");
    
    std::string vertexSource;
    std::string fragmentSource;
    
    long vertex_shader_length = get_file_length(VERTEX_SHADER);
    long frag_shader_length = get_file_length(FRAG_SHADER);
    
    vertexSource.reserve(vertex_shader_length);
    fragmentSource.reserve(frag_shader_length);
    
    //Read our shaders into the appropriate buffers
    load_file(vertexSource, VERTEX_SHADER); //Get source code for vertex shader.
    load_file(fragmentSource, FRAG_SHADER); //Get source code for fragment shader.
    
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
        
        //Use the infoLog as you see fit.
        
        //In this simple program, we'll just leave
        return -1;
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
        
        //Use the infoLog as you see fit.
        
        //In this simple program, we'll just leave
        return -1;
    }
    
    
    
    //Vertex and fragment shaders are successfully compiled.
    //Now time to link them together into a program.
    //Get a program object.
    program = glCreateProgram();
    
    //Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    //Link our program
    glLinkProgram(program);
    
    //Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        
        //The maxLength includes the NULL character
        GLchar *info_log;
        info_log = (GLchar*)malloc(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, info_log);
        fprintf(stderr, "Program compilation failed: %*s\n", maxLength, info_log);
        free(info_log);
        
        //We don't need the program anymore.
        glDeleteProgram(program);
        //Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        //Use the infoLog as you see fit.
        
        //In this simple program, we'll just leave
        return -1;
    }
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    const char* attribute_name = "coord";
    block_attribute_coord = glGetAttribLocation(program, attribute_name);
    if (block_attribute_coord == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return -1;
    }
    
    const char* texture_attribute_name = "texture_coord";
    texture_attribute_coord = glGetAttribLocation(program, texture_attribute_name);
    if (texture_attribute_coord == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", texture_attribute_name);
        return -1;
    }
    
    const char* uniform_name = "mvp";
    block_uniform_mvp = glGetUniformLocation(program, uniform_name);
    if (block_uniform_mvp == -1) {
        fprintf(stderr, "Could not bind attribute %s\n",uniform_name);
        return false;
    }
    
    const char* uniform_draw_mode_name = "draw_mode";
    block_uniform_draw_mode = glGetUniformLocation(program, uniform_draw_mode_name);
    if (block_uniform_draw_mode == -1) {
        fprintf(stderr, "Could not bind attribute %s\n",uniform_draw_mode_name);
        return false;
    }
    
    const char* uniform_shade_intensity_name = "shade_intensity";
    block_shader_intensity = glGetUniformLocation(program, uniform_shade_intensity_name);
    if (block_shader_intensity == -1) {
        fprintf(stderr, "Could not bind attribute %s\n",uniform_shade_intensity_name);
        return false;
    }
    
    const char* uniform_alpha_cutoff_name = "alpha_cutoff";
    block_alpha_cutoff = glGetUniformLocation(program, uniform_alpha_cutoff_name);
    if (block_alpha_cutoff == -1) {
        fprintf(stderr, "Could not bind attribute %s\n",uniform_alpha_cutoff_name);
        return false;
    }
    
    const char* uniform_alpha_set_name = "alpha_set";
    block_alpha_set = glGetUniformLocation(program, uniform_alpha_set_name);
    if (block_alpha_set == -1) {
        fprintf(stderr, "Could not bind attribute %s\n",uniform_alpha_set_name);
        return false;
    }
    
    glEnableVertexAttribArray(block_attribute_coord);
    glEnableVertexAttribArray(texture_attribute_coord);
    
    printf("4 %d\n",glGetError());
    
    printf("Done loading shaders\n");
    return 0;
}