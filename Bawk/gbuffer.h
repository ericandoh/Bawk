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
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_COLOR,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_NUM_TEXTURES
    };
    GBuffer();
    ~GBuffer();
    bool init(unsigned int wwidth, unsigned int wheight, unsigned int to);
    void bind_for_write();
    void bind_for_read();
    void bind_for_read_color_map_only();
    void bind_for_readg();
    void set_read_buffer(GBUFFER_TEXTURE_TYPE type);
private:
    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
    unsigned int texture_offset;
};

#endif /* defined(__Bawk__gbuffer__) */
