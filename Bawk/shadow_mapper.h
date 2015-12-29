//
//  shadow_mapper.h
//  Bawk
//
//  Created by Eric Oh on 12/20/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__shadow_mapper__
#define __Bawk__shadow_mapper__

#include <stdio.h>
#include "includeglfw.h"

class ShadowMapper {
private:
    GLuint m_fbo;
    GLuint m_shadow_map;
    unsigned int texture_offset;
public:
    ShadowMapper();
    ~ShadowMapper();
    // to = texture_offset
    bool init(unsigned int wwidth, unsigned int wheight);
    void bind_for_write();
    void bind_for_read();
};

#endif /* defined(__Bawk__shadow_mapper__) */
