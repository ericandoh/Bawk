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
#include "shader_loader.h"

const std::string VERTEX_SHADER = "vertex_shader";
const std::string FRAG_SHADER = "frag_shader";

const GLuint INVALID_SHADER = -1;

long get_file_length(std::string file_name) {
    // TODO (handle if file not found)
    // right now will return -1 => can't reserve string lol
    std::ifstream t(file_name);
    return t.tellg();
}

// Loads an entire file text into a string
void load_file(std::string str, std::string file_name) {
    std::ifstream t(file_name);
    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
}

GLuint set_shaders() {
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
        return INVALID_SHADER;
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
        return INVALID_SHADER;
    }
    
    //Vertex and fragment shaders are successfully compiled.
    //Now time to link them together into a program.
    //Get a program object.
    GLuint program = glCreateProgram();
    
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
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        
        //We don't need the program anymore.
        glDeleteProgram(program);
        //Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        //Use the infoLog as you see fit.
        
        //In this simple program, we'll just leave
        return INVALID_SHADER;
    }
    
    //Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    
    GLint attribute_coord;
    const char* attribute_name = "coord2d";
    attribute_coord = glGetAttribLocation(program, attribute_name);
    if (attribute_coord == -1) {
        fprintf(stderr, "Could not bind attribute %s", attribute_name);
        return INVALID_SHADER;
    }
    return attribute_coord;
}