//
//  gbuffer.cpp
//  Bawk
//
//  Created by Eric Oh on 11/5/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "gbuffer.h"
#include "worldrender.h"

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
}

bool GBuffer::init(unsigned int wwidth, unsigned int wheight, unsigned int to) {
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    
    printf("A %d\n",glGetError());
    
    // Create the gbuffer textures
    glGenTextures(GBUFFER_NUM_TEXTURES, m_textures);
    glGenTextures(1, &m_depthTexture);
    
    printf("B %d\n",glGetError());
    
    for (unsigned int i = 0 ; i < GBUFFER_NUM_TEXTURES ; i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, wwidth, wheight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }
    
    printf("C %d\n",glGetError());
    
    // depth
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, wwidth, wheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    
    printf("DE %d\n",glGetError());
    
    /*GLenum DrawBuffers[GBUFFER_NUM_TEXTURES];
    
    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
        DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }*/
    //GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, DrawBuffers);
    
    printf("D %d\n",glGetError());
    
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
    
    printf("E %d\n",glGetError());
    
    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    texture_offset = to;
    
    return true;
}

void GBuffer::bind_for_write() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void GBuffer::bind_for_read() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
        glActiveTexture(GL_TEXTURE0 + i + texture_offset);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
    }
    glUniform1i(lighting_position_map, texture_offset);
    glUniform1i(lighting_color_map, texture_offset + 1);
    glUniform1i(lighting_color_t_map, texture_offset + 2);
    //glUniform1i(lighting_normal_map, texture_offset + 3);
}

void GBuffer::bind_for_readg() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void GBuffer::set_read_buffer(GBUFFER_TEXTURE_TYPE type) {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + type);
}