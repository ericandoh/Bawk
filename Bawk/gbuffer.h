//
//  gbuffer.h
//  Bawk
//
//  Taken shamelessly from http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html
//
//  Created by Eric Oh on 11/5/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__gbuffer__
#define __Bawk__gbuffer__

#include <stdio.h>
#include "includeglfw.h"

class GBuffer {
public:
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION = 0,
        GBUFFER_TEXTURE_TYPE_DIFFUSE = 1,
        GBUFFER_TEXTURE_TYPE_NORMAL = 2,
        GBUFFER_TEXTURE_TYPE_TEXCOORD = 3,
        GBUFFER_NUM_TEXTURES = 4
    };
    GBuffer();
    ~GBuffer();
    bool init(unsigned int wwidth, unsigned int wheight);
    void bind_for_write();
    void bind_for_read();
    void set_read_buffer(GBUFFER_TEXTURE_TYPE type);
private:
    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
};

#endif /* defined(__Bawk__gbuffer__) */
