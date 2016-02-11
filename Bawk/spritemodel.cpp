//
//  spritemodel.cpp
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "spritemodel.h"
#include "worldrender.h"
#include "obj_data_loader.h"
#include "json_reader_helper.h"
#include "json/json.h"

SpriteModel::SpriteModel() {
    model = new ModelData();
}

void SpriteModel::render(fmat4* transform, SpriteRender* render) {
    fmat4 view;
    render->get_mvp(&view);
    view = *transform * view;
    OGLAttr::current_shader->set_transform_matrix(&view);
    model->render();
}

SpriteMultiplexer* get_sprite_model(Json::Value node) {
    SpriteModel* model = new SpriteModel();
    if (node.isMember("texture")) {
        std::string texture_src = json_read_string_or_empty(node["texture"]);
        read_obj_file(model->model, texture_src);
    }
    return model;
}