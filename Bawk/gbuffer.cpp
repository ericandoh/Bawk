//
//  gbuffer.cpp
//  Bawk
//
//  Created by Eric Oh on 11/5/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "gbuffer.h"
#include "worldrender.h"
#include "texture_allocator.h"

#include "opengl_debug.h"

GBuffer::GBuffer()
{
    m_fbo = 0;
    m_depthTexture = 0;
    for (int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
        m_textures[i] = 0;
    }
    texture_offset = 0;
}

GBuffer::~GBuffer()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }
    
    if (m_textures[0] != 0) {
        glDeleteTextures(GBUFFER_NUM_TEXTURES, m_textures);
    }
    
    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
    check_for_error();
}

bool GBuffer::init(unsigned int wwidth, unsigned int wheight) {
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    check_for_error();
    
    // Create the gbuffer textures
    glGenTextures(GBUFFER_NUM_TEXTURES, m_textures);
    glGenTextures(1, &m_depthTexture);
    check_for_error();
    
    // Create the G-buffer textures
    for (unsigned int i = 0 ; i < GBUFFER_NUM_TEXTURES ; i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, wwidth, wheight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }
    check_for_error();
    
    // Create the depth texture
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, wwidth, wheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    check_for_error();
    
    GLenum draw_buffers[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(4, draw_buffers);
    check_for_error();
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer error, status: 0x%x\n", status);
        return false;
    }
    
    // Restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    check_for_error();
    // Reserve some active textures for our FBO
    texture_offset = reserve_n_active_textures(GBUFFER_NUM_TEXTURES);
    return true;
}

void GBuffer::bind_for_write() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void GBuffer::bind_for_read() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    // only bind color and position
    int num_textures = 2;
    for (unsigned int i = 0; i < num_textures; i++) {
        glActiveTexture(GL_TEXTURE0 + i + texture_offset);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
    }
    glUniform1i(OGLAttr::lighting_shader.position_map, texture_offset);
    glUniform1i(OGLAttr::lighting_shader.color_map, texture_offset + 1);
    //glUniform1i(OGLAttr::lighting_normal_map, texture_offset + 2);
}

void GBuffer::bind_for_read_color_map_only() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glActiveTexture(GL_TEXTURE0 + 1 + texture_offset);
    glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    glUniform1i(OGLAttr::lighting_shader.color_t_map, texture_offset + 1);
}

void GBuffer::bind_for_readg() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void GBuffer::set_read_buffer(GBUFFER_TEXTURE_TYPE type) {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + type);
}