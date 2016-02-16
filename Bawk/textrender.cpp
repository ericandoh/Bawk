//
//  textrender.cpp
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "textrender.h"
#include <map>
#include "worldrender.h"

const std::string TEXT_TEXTURE_STRING = "0123456789abcdefghijklmnopqrstuvwxyz?!.,";
const int TEXT_TEXTURE_CHARS_PER_ROW = 10;
const int TEXT_TEXTURE_ROW_COUNT = 4;

// ratio of width / height PER character
const float TEXT_TEXTURE_RATIO = 0.5f;

class TextRenderResource {
    int alphabet_start;
    int number_start;
    std::map<char, int> char_to_index;
public:
    void init();
    void draw_char(char v);
    void clean();
};

void TextRenderResource::init() {
    for (int i = 0; i < TEXT_TEXTURE_STRING.length(); i++) {
        char character = TEXT_TEXTURE_STRING[i];
        char_to_index[character] = i;
        if (character == 'a') {
            alphabet_start = i;
        }
        else if (character == '0') {
            number_start = i;
        }
    }
}

bool is_alphabet(char x) {
    return x >= 'a' && x <= 'z';
}

bool is_numeric(char x) {
    return x >= '0' && x <= '9';
}

void TextRenderResource::draw_char(char v) {
    int index = 0;
    if (is_alphabet(v)) {
        index = alphabet_start + (v - 'a');
    }
    else if (is_numeric(v)) {
        index = number_start + (v - '0');
    }
    else if (v == ' ') {
        return;
    }
    else {
        if (char_to_index.count(v)) {
            index = char_to_index[v];
        }
        else {
            // use '?'
            index = char_to_index['?'];
        }
    }
    
    int xc = index % TEXT_TEXTURE_CHARS_PER_ROW;
    int yc = index / TEXT_TEXTURE_CHARS_PER_ROW;
    
    float lowerx = xc * 1.0f / TEXT_TEXTURE_CHARS_PER_ROW;
    float upperx = (xc + 1) * 1.0f / TEXT_TEXTURE_CHARS_PER_ROW;
    float lowery = 1.0f - (yc + 1) * 1.0f / TEXT_TEXTURE_ROW_COUNT;
    float uppery = 1.0f - yc * 1.0f / TEXT_TEXTURE_ROW_COUNT;
    
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::UV);
    OGLAttr::current_shader->bind_texture(Textures::TextureName::FONTS);
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    float texture[6][3] = {
        {lowerx, lowery, 0},
        {upperx, lowery, 0},
        {lowerx, uppery, 0},
        {lowerx, uppery, 0},
        {upperx, lowery, 0},
        {upperx, uppery, 0},
    };
    set_unitary_transform_matrix();
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TextRenderResource::clean() {
    char_to_index.clear();
}

namespace TextRender {
    TextRenderResource text_resource;
}

using namespace TextRender;

void init_string_resources() {
    text_resource.init();
}

void render_string(std::string text, int x, int y, int height) {
    int width = height * TEXT_TEXTURE_RATIO;
    for (int i = 0; i < text.length(); i++) {
        char v = text[i];
        glViewport(x + i * width, y - height, width, height);
        text_resource.draw_char(v);
    }
}

void render_string(std::string text, int xl, int yl, int xu, int yu, int height) {
    int width = height * TEXT_TEXTURE_RATIO;
    int row = 0;
    for (int i = 0; i < text.length(); i++) {
        if (xl + (i + 1) * width > xu) {
            // need a new line
            row += 1;
            if (yl + (row) * height > yu) {
                // wont render, break
                break;
            }
        }
        char v = text[i];
        glViewport(xl + i * width, yl + (row - 1) * height, width, height);
        text_resource.draw_char(v);
    }
}

void clean_string_resources() {
    text_resource.clean();
}