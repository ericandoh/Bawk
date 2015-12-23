//
//  shadow_mapper.cpp
//  Bawk
//
//  heavily based off of http://ogldev.atspace.co.uk/www/tutorial23/tutorial23.html
//  thanks!
//
//  Created by Eric Oh on 12/20/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "shadow_mapper.h"

ShadowMapper::ShadowMapper() {
    m_fbo = 0;
    m_shadow_map = 0;
}

ShadowMapper::~ShadowMapper() {
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    
    if (m_shadow_map != 0) {
        glDeleteTextures(1, &m_shadow_map);
    }
}

bool ShadowMapper::init(unsigned int wwidth, unsigned int wheight, int to) {
    // make the FBO
    glGenFramebuffers(1, &m_fbo);
    
    // make the shadow map texture
    glGenTextures(1, &m_shadow_map);
    glBindTexture(GL_TEXTURE_2D, m_shadow_map);
    // this is a texture for depth only
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, wwidth, wheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // clamp because shadows are within [0,1] range
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // last 0 is mipmap level (no mipmapping)
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadow_map, 0);
    
    // for this framebuffer, don't write to color
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", status);
        return false;
    }
    
    texture_offset = to;
    
    return true;
}

void ShadowMapper::bind_for_write() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void ShadowMapper::bind_for_read() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    
    
    glActiveTexture(GL_TEXTURE0 + texture_offset);
    glBindTexture(GL_TEXTURE_2D, m_shadow_map);
    
    // glUniform1i(texture_map_variable, texture_offset);
}
