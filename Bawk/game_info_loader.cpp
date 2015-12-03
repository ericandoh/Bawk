//
//  game_info_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 10/22/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "game_info_loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <map>
#include "json/json.h"
#include "block_loader.h"
#include "cursorsuperobject.h"
#include "world_generator.h"
#include "world_generator_structures.h"
#include "modelrender.h"
#include "lightrender.h"

// add this to every recipe ID
// +1, so 0 doesn't map to 0 in the actual block
const uint16_t recipe_mask = 0x1 << 15;

// TODO deprecate parts of this
// none of these structs have a uint16_t type
// because those are the indices into our vectors
struct block_game_info {
    std::string name;
    bool is_model;
    int texture;
    int textures[6];
    int resistance;
    int transparency;
    int weight;
    int vehicle;
    RenderableLight light;
    block_game_info() {
        name = "";
        is_model = false;
        texture = 0;
        resistance = 1;
        transparency = 0;
        weight = 1;
        vehicle = 0;
        light = RenderableLight();
    }
};

// TODO deprecate this
struct model_game_info {
    // we will use this to bind to texture which is loaded in by SOIL
    std::string texture;
    std::vector<fvec3> vertices;
    std::vector<fvec3> uvs;
    std::vector<fvec3> normals;
};

struct recipe_game_info {
    std::string name;
    bool from_file;
    std::string file_path;
    std::vector<ivec3> positions;
    ivec3 upper;    // the bounds of this object, after it has been zero'd (which will happen automatically)
    std::vector<uint16_t> blks;
    std::vector<BlockOrientation> orientations;
};

struct ui_block_callback_info {
    block_mouse_callback_func mouse_callback;
    block_keyboard_callback_func keyboard_callback;
    std::vector<Action> key_bindings;
};

class GameInfoDataObject {
    int read_blocks(Json::Value root);
    int read_models(Json::Value root);
    int read_recipes(Json::Value root);
    int read_world_gen(Json::Value root);
public:
    int version;
    std::vector<block_game_info> block_info;
    std::vector<RenderableModel> model_info;
    std::vector<block_game_info> recipe_block_info;
    std::map<uint16_t, model_game_info> block_model_info;
    std::map<uint16_t, ui_block_callback_info> block_callback_info;
    std::vector<recipe_game_info> recipe_info;
    std::vector<biome_game_info> biome_info;
    std::vector<world_gen_mode_info> world_gen_modes;
    
    GameInfoDataObject();
    ~GameInfoDataObject();
    int read_values();
    // accessor methods
    
};

// store the current game data object as a blobal
GameInfoDataObject* game_data_object;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    // stolen shamelessly (i'm a good thief) from http://stackoverflow.com/questions/236129/split-a-string-in-c
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void read_obj_file(RenderableModel* dst, std::string obj_filename) {
    // taken shamelessly from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
    
    //obj_filename = "/Users/Eric/Documents/dev/BawkAssets/simplebox.obj";
    
    std::vector<fvec3> temp_vertices;
    std::vector<fvec3> temp_uvs;
    std::vector<fvec3> temp_normals;
    
    std::ifstream file(get_path_to_game_folder() + "/" + obj_filename);
    //std::ifstream file(obj_filename);
    
    if (!file.is_open()) {
        printf("File could not be found %s\n", (get_path_to_game_folder() + "/" + obj_filename).c_str());
    }
    
    std::string line;
    while (std::getline(file, line)) {
        
        std::istringstream iss(line);
        std::string identifier;
        if (!(iss >> identifier)) { break; }
        if (identifier.compare("v") == 0) {
            fvec3 vertex;
            if (!(iss >> vertex.x >> vertex.y >> vertex.z)) { break; }
            temp_vertices.push_back(vertex);
        }
        else if (identifier.compare("vt") == 0) {
            fvec3 uv;
            uv.z = 0;
            if (!(iss >> uv.x >> uv.y)) { break; }
            temp_uvs.push_back(uv);
        }
        else if (identifier.compare("vn") == 0) {
            fvec3 normal;
            if (!(iss >> normal.x >> normal.y >> normal.z)) { break; }
            temp_normals.push_back(normal);
        }
        else if (identifier.compare("f") == 0) {
            // dude did you seriously 1-index
            std::string v1, v2, v3;
            if (!(iss >> v1 >> v2 >> v3)) { break; }
            
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            std::vector<std::string> elems;
            
            split(v1, '/', elems);
            if (elems.size() < 3) {
                printf("Obj file incorrectly formatted; aborting\n");
                return;
            }
            vertexIndex[0] = std::stoi(elems[0]);
            uvIndex[0] = std::stoi(elems[1]);
            normalIndex[0] = std::stoi(elems[2]);
            
            elems.clear();
            split(v2, '/', elems);
            if (elems.size() < 3) {
                printf("Obj file incorrectly formatted; aborting\n");
                return;
            }
            vertexIndex[1] = std::stoi(elems[0]);
            uvIndex[1] = std::stoi(elems[1]);
            normalIndex[1] = std::stoi(elems[2]);
            
            elems.clear();
            split(v3, '/', elems);
            if (elems.size() < 3) {
                printf("Obj file incorrectly formatted; aborting\n");
                return;
            }
            vertexIndex[2] = std::stoi(elems[0]);
            uvIndex[2] = std::stoi(elems[1]);
            normalIndex[2] = std::stoi(elems[2]);
            
            dst->model_vertices.push_back(temp_vertices[vertexIndex[0] - 1]);
            dst->model_vertices.push_back(temp_vertices[vertexIndex[1] - 1]);
            dst->model_vertices.push_back(temp_vertices[vertexIndex[2] - 1]);
            
            dst->model_uvs.push_back(temp_uvs[uvIndex[0] - 1]);
            dst->model_uvs.push_back(temp_uvs[uvIndex[1] - 1]);
            dst->model_uvs.push_back(temp_uvs[uvIndex[2] - 1]);
            
            dst->model_normals.push_back(temp_normals[normalIndex[0] - 1]);
            dst->model_normals.push_back(temp_normals[normalIndex[1] - 1]);
            dst->model_normals.push_back(temp_normals[normalIndex[2] - 1]);
        }
    }
    file.close();
}

// TODO DERPECATE BELOW
void read_obj_file(model_game_info* dst, std::string obj_filename) {
    // taken shamelessly from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
    
    //obj_filename = "/Users/Eric/Documents/dev/BawkAssets/simplebox.obj";
    
    std::vector<fvec3> temp_vertices;
    std::vector<fvec3> temp_uvs;
    std::vector<fvec3> temp_normals;
    
    std::ifstream file(get_path_to_game_folder() + "/" + obj_filename);
    //std::ifstream file(obj_filename);
    
    if (!file.is_open()) {
        printf("File could not be found %s\n", (get_path_to_game_folder() + "/" + obj_filename).c_str());
    }
    
    std::string line;
    while (std::getline(file, line)) {
        
        std::istringstream iss(line);
        std::string identifier;
        if (!(iss >> identifier)) { break; }
        if (identifier.compare("v") == 0) {
            fvec3 vertex;
            if (!(iss >> vertex.x >> vertex.y >> vertex.z)) { break; }
            temp_vertices.push_back(vertex);
        }
        else if (identifier.compare("vt") == 0) {
            fvec3 uv;
            uv.z = 0;
            if (!(iss >> uv.x >> uv.y)) { break; }
            temp_uvs.push_back(uv);
        }
        else if (identifier.compare("vn") == 0) {
            fvec3 normal;
            if (!(iss >> normal.x >> normal.y >> normal.z)) { break; }
            temp_normals.push_back(normal);
        }
        else if (identifier.compare("f") == 0) {
            // dude did you seriously 1-index
            std::string v1, v2, v3;
            if (!(iss >> v1 >> v2 >> v3)) { break; }
            
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            std::vector<std::string> elems;
            
            split(v1, '/', elems);
            if (elems.size() < 3) {
                printf("Obj file incorrectly formatted; aborting\n");
                return;
            }
            vertexIndex[0] = std::stoi(elems[0]);
            uvIndex[0] = std::stoi(elems[1]);
            normalIndex[0] = std::stoi(elems[2]);
            
            elems.clear();
            split(v2, '/', elems);
            if (elems.size() < 3) {
                printf("Obj file incorrectly formatted; aborting\n");
                return;
            }
            vertexIndex[1] = std::stoi(elems[0]);
            uvIndex[1] = std::stoi(elems[1]);
            normalIndex[1] = std::stoi(elems[2]);
            
            elems.clear();
            split(v3, '/', elems);
            if (elems.size() < 3) {
                printf("Obj file incorrectly formatted; aborting\n");
                return;
            }
            vertexIndex[2] = std::stoi(elems[0]);
            uvIndex[2] = std::stoi(elems[1]);
            normalIndex[2] = std::stoi(elems[2]);
            
            dst->vertices.push_back(temp_vertices[vertexIndex[0] - 1]);
            dst->vertices.push_back(temp_vertices[vertexIndex[1] - 1]);
            dst->vertices.push_back(temp_vertices[vertexIndex[2] - 1]);
            
            dst->uvs.push_back(temp_uvs[uvIndex[0] - 1]);
            dst->uvs.push_back(temp_uvs[uvIndex[1] - 1]);
            dst->uvs.push_back(temp_uvs[uvIndex[2] - 1]);
            
            dst->normals.push_back(temp_normals[normalIndex[0] - 1]);
            dst->normals.push_back(temp_normals[normalIndex[1] - 1]);
            dst->normals.push_back(temp_normals[normalIndex[2] - 1]);
        }
    }
    file.close();
}

GameInfoDataObject::GameInfoDataObject() {
    
}

GameInfoDataObject::~GameInfoDataObject() {
    
}

int GameInfoDataObject::read_blocks(Json::Value root) {
    if (root.type() != Json::objectValue)
        return 1;
    
    Json::Value block;
    block_game_info* info;
    
    Json::Value::Members members(root.getMemberNames());
    for (Json::Value::Members::iterator it = members.end() - 1;
         it >= members.begin();
         --it) {
        const std::string& name = *it;
        uint16_t block_id = std::stoi(name);
        if (block_id >= block_info.size()) {
            block_info.resize(block_id + 1);
        }
        info = &(block_info[block_id]);
        block = root[name];
        if (block.isMember("name") && block["name"].type() == Json::stringValue) {
            info->name.assign(block["name"].asString().c_str());
        }
        if (block.isMember("resistance") && block["resistance"].type() == Json::intValue) {
            info->resistance = block["resistance"].asInt();
        }
        if (block.isMember("texture")) {
            if (block["texture"].type() == Json::intValue) {
                info->texture = block["texture"].asInt();
            }
            else if (block["texture"].type() == Json::stringValue) {
                block_model_info[block_id] = model_game_info();
                read_obj_file(&(block_model_info[block_id]), block["texture"].asString());
                info->is_model = true;
            }
            else if (block["texture"].type() == Json::arrayValue) {
                int size = block["texture"].size();
                for (int i = 0; i < size; i++) {
                    info->textures[i] = block["texture"][i].asInt();
                }
                info->texture = -1;
            }
        }
        else {
            info->texture = block_id;
        }
        if (block.isMember("transparency") && block["transparency"].type() == Json::intValue) {
            info->transparency = block["transparency"].asInt();
        }
        if (block.isMember("weight") &&block["weight"].type() == Json::intValue) {
            info->weight = block["weight"].asInt();
        }
        if (block.isMember("vehicle") && block["vehicle"].type() == Json::intValue) {
            info->vehicle = block["vehicle"].asInt();
        }
        if (block.isMember("action") and block["action"].type() == Json::stringValue) {
            ui_block_callback_info callback_info;
            callback_info.mouse_callback = get_block_mouse_callback_for(block["action"].asString());
            callback_info.keyboard_callback = get_block_keyboard_callback_for(block["action"].asString(), callback_info.key_bindings);
            block_callback_info[block_id] = callback_info;
        }
        if (block.isMember("light") && block["light"].type() == Json::arrayValue) {
            int size = block["light"].size();
            if (size >= 2) {
                info->light.light_radius = block["light"][0].asFloat();
                info->light.light_intensity = block["light"][1].asFloat();
            }
        }
    }
    return 0;
}

int GameInfoDataObject::read_models(Json::Value root) {
    if (root.type() != Json::objectValue)
        return 1;
    
    Json::Value model;

    RenderableModel* info;
    
    Json::Value::Members members(root.getMemberNames());
    for (Json::Value::Members::iterator it = members.end() - 1;
         it >= members.begin();
         --it) {
        const std::string& name = *it;
        uint16_t model_id = std::stoi(name);
        if (model_id >= model_info.size()) {
            model_info.resize(model_id + 1);
        }
        info = &(model_info[model_id]);
        model = root[name];
        if (model.isMember("name") && model["name"].type() == Json::stringValue) {
            info->name.assign(model["name"].asString().c_str());
        }
        if (model.isMember("resistance") && model["resistance"].type() == Json::intValue) {
            info->resistance = model["resistance"].asInt();
        }
        if (model.isMember("texture")) {
            read_obj_file(info, model["texture"].asString());
        }
        if (model.isMember("weight") && model["weight"].type() == Json::intValue) {
            info->weight = model["weight"].asInt();
        }
        if (model.isMember("vehicle") && model["vehicle"].type() == Json::intValue) {
            info->vehicle = model["vehicle"].asInt();
        }
        if (model.isMember("action") and model["action"].type() == Json::stringValue) {
            ui_block_callback_info callback_info;
            info->mouse_callback = get_block_mouse_callback_for(model["action"].asString());
            info->keyboard_callback = get_model_keyboard_callback_for(model["action"].asString(), info->key_bindings);
        }
        if (model.isMember("light") && model["light"].type() == Json::arrayValue) {
            int size = model["light"].size();
            if (size >= 2) {
                info->light.light_radius = model["light"][0].asFloat();
                info->light.light_intensity = model["light"][1].asFloat();
            }
        }
    }
    // calculate auxiliary data
    for (auto&i: model_info) {
        i.refresh();
    }
    return 0;
}

uint16_t get_block_id_from_name(Json::Value node) {
    if (node.type() == Json::stringValue) {
        for (uint16_t i = 0; i < game_data_object->block_info.size(); i++) {
            if (game_data_object->block_info[i].name.compare(node.asString()) == 0) {
                return i;
            }
        }
        for (uint16_t i = 0; i < game_data_object->recipe_info.size(); i++) {
            if (game_data_object->recipe_info[i].name.compare(node.asString()) == 0) {
                return i + recipe_mask;
            }
        }
        return 0;
    }
    else {
        return node.asInt();
    }
}

int GameInfoDataObject::read_recipes(Json::Value root) {
    if (root.type() != Json::objectValue)
        return 1;
    
    Json::Value recipe;
    recipe_game_info* info;
    block_game_info* block_info;
    
    Json::Value::Members members(root.getMemberNames());
    for (Json::Value::Members::iterator it = members.end() - 1;
         it >= members.begin();
         --it) {
        const std::string& name = *it;
        uint16_t recipe_id = std::stoi(name);
        if (recipe_id >= recipe_info.size()) {
            recipe_info.resize(recipe_id + 1);
            recipe_block_info.resize(recipe_id + 1);
        }
        info = &(recipe_info[recipe_id]);
        block_info = &(recipe_block_info[recipe_id]);
        recipe = root[name];
        
        if (recipe.isMember("name") && recipe["name"].type() == Json::stringValue) {
            info->name = recipe["name"].asString();
        }
        if (recipe.isMember("blocks") && recipe["blocks"].type() == Json::arrayValue) {
            int size = recipe["blocks"].size();
            Json::Value submember;
            ivec3 left_corner;
            for (int i = 0; i < size; i++) {
                submember = recipe["blocks"][i];
                if(submember.type() == Json::objectValue) {
                    if ((!submember.isMember("id") || submember["id"].type() == Json::intValue || submember["id"].type() == Json::stringValue) && (!submember.isMember("orientation") || submember["orientation"].type() == Json::intValue || submember["orientation"].type() == Json::stringValue) && submember["position"].type() == Json::arrayValue) {
                        ivec3 position(0, 0, 0);
                        int subsize = submember["position"].size();
                        if (subsize >= 3) {
                            position.x = submember["position"][0].asInt();
                            position.y = submember["position"][1].asInt();
                            position.z = submember["position"][2].asInt();
                            left_corner = get_ivec3_minimum(left_corner, position);
                        }
                        
                        uint16_t bid = recipe_id + recipe_mask;
                        if (submember.isMember("id")) {
                            bid = get_block_id_from_name(submember["id"]);
                        }
                        info->blks.push_back(bid);
                        info->positions.push_back(position);
                        
                        if (submember.isMember("orientation")) {
                            if (submember["orientation"].type() == Json::stringValue) {
                                if (submember["orientation"].asString().compare("front") == 0) {
                                    info->orientations.push_back(BlockOrientation::FRONT);
                                }
                                else if (submember["orientation"].asString().compare("back") == 0) {
                                    info->orientations.push_back(BlockOrientation::BACK);
                                }
                                else if (submember["orientation"].asString().compare("left") == 0) {
                                    info->orientations.push_back(BlockOrientation::LEFT);
                                }
                                else if (submember["orientation"].asString().compare("right") == 0) {
                                    info->orientations.push_back(BlockOrientation::RIGHT);
                                }
                                else if (submember["orientation"].asString().compare("top") == 0) {
                                    info->orientations.push_back(BlockOrientation::TOP);
                                }
                                else if (submember["orientation"].asString().compare("bottom") == 0) {
                                    info->orientations.push_back(BlockOrientation::BOTTOM);
                                }
                                else {
                                    info->orientations.push_back(DEFAULT_BLOCK_ORIENTATION);
                                }
                            }
                            else if (submember["orientation"].type() == Json::intValue) {
                                info->orientations.push_back(static_cast<BlockOrientation>(submember["orientation"].asInt()));
                            }
                            else {
                                info->orientations.push_back(DEFAULT_BLOCK_ORIENTATION);
                            }
                        }
                        else {
                            info->orientations.push_back(DEFAULT_BLOCK_ORIENTATION);
                        }
                    }
                    
                }
            }
            ivec3 max_pos(0,0,0);
            for (int i = 0; i < info->positions.size(); i++) {
                ivec3 real_pos = ivec3(info->positions[i].x - left_corner.x,
                                       info->positions[i].y - left_corner.y,
                                       info->positions[i].z - left_corner.z);
                info->positions[i] = real_pos;
                max_pos = get_ivec3_maximum(max_pos, real_pos);
            }
            info->upper = ivec3(max_pos.x + 1, max_pos.y + 1, max_pos.z + 1);
            info->from_file = false;
        }
        if (recipe.isMember("blocks") && recipe["blocks"].type() == Json::stringValue) {
            if (recipe["blockfile"].asString().compare("auto") == 0) {
                // using the texture block model obj data, automatically fill in blocks where the model has points
                // then assign a center block as the center of the object
                printf("frog\n");
            }
            else {
                // this is the name of a file.
                info->from_file = true;
                info->file_path = get_path_to_game_folder() + "/" +  recipe["blockfile"].asString();
            }
        }
        if (recipe.isMember("texture") && recipe["texture"].type() == Json::stringValue) {
            block_model_info[recipe_id + recipe_mask] = model_game_info();
            read_obj_file(&(block_model_info[recipe_id + recipe_mask]), recipe["texture"].asString());
            block_info->is_model = true;
        }
        else {
            // the recipe (should) not have recipe blocks, and if it does default to the corresponding block id texture
            block_info->texture = recipe_id;
        }
        if (recipe.isMember("resistance") && recipe["resistance"].type() == Json::intValue) {
            block_info->resistance = recipe["resistance"].asInt();
        }
        if (recipe.isMember("weight") && recipe["weight"].type() == Json::intValue) {
            block_info->weight = recipe["weight"].asInt();
        }
        if (recipe.isMember("action") and recipe["action"].type() == Json::stringValue) {
            ui_block_callback_info callback_info;
            callback_info.mouse_callback = get_block_mouse_callback_for(recipe["action"].asString());
            callback_info.keyboard_callback = get_block_keyboard_callback_for(recipe["action"].asString(), callback_info.key_bindings);
            block_callback_info[recipe_id + recipe_mask] = callback_info;
        }
    }
    return 0;
}

uint16_t get_biome_id_from_name(const std::string &name) {
    for (uint16_t i = 0; i < game_data_object->biome_info.size(); i++) {
        if (game_data_object->biome_info[i].name.compare(name) == 0) {
            return i;
        }
    }
    return 0;
}

int GameInfoDataObject::read_world_gen(Json::Value root) {
    // read biome information
    if (root.isMember("biomes") && root["biomes"].type() == Json::arrayValue) {
        int size = root["biomes"].size();
        for (int i = 0; i < size; i++){
            Json::Value biome_node = root["biomes"][i];
            if (biome_node.type() != Json::objectValue) {
                continue;
            }
            biome_game_info binfo;
            if (biome_node.isMember("name") && biome_node["name"].type() == Json::stringValue) {
                binfo.name = biome_node["name"].asString();
            }
            if (biome_node.isMember("strength") && (biome_node["strength"].type() == Json::realValue || biome_node["strength"].type() == Json::intValue)) {
                binfo.strength = biome_node["strength"].asFloat();
            }
            if (biome_node.isMember("persistence") && (biome_node["persistence"].type() == Json::realValue || biome_node["persistence"].type() == Json::intValue)) {
                binfo.persistence = biome_node["persistence"].asFloat();
            }
            if (biome_node.isMember("blocks") && biome_node["blocks"].type() == Json::arrayValue) {
                int block_size = biome_node["blocks"].size();
                for (int j = 0; j < block_size; j++) {
                    Json::Value block_root = biome_node["blocks"][j];
                    if (block_root.type() != Json::objectValue)
                        continue;
                    block_layer_info blk_layer;
                    if (block_root.isMember("id")) {
                        blk_layer.type = get_block_id_from_name(block_root["id"]);
                    }
                    if (block_root.isMember("lower") && block_root["lower"].type() == Json::intValue) {
                        blk_layer.lower = block_root["lower"].asInt();
                    }
                    if (block_root.isMember("upper") && block_root["upper"].type() == Json::intValue) {
                        blk_layer.upper = block_root["upper"].asInt();
                    }
                    if (block_root.isMember("frequency") && (block_root["frequency"].type() == Json::realValue || block_root["frequency"].type() == Json::intValue) ) {
                        blk_layer.frequency = block_root["frequency"].asFloat();
                    }
                    binfo.blocks.push_back(blk_layer);
                }
            }
            if (biome_node.isMember("structures") && biome_node["structures"].type() == Json::arrayValue) {
                int structure_size = biome_node["structures"].size();
                for (int j = 0; j < structure_size; j++) {
                    Json::Value structure_root = biome_node["structures"][j];
                    if (structure_root.type() != Json::objectValue)
                        continue;
                    block_layer_info struct_layer;
                    if (structure_root.isMember("id")) {
                        struct_layer.type = get_block_id_from_name(structure_root["id"]);
                    }
                    if (structure_root.isMember("lower") && structure_root["lower"].type() == Json::intValue) {
                        struct_layer.lower = structure_root["lower"].asInt();
                    }
                    if (structure_root.isMember("upper") && structure_root["upper"].type() == Json::intValue) {
                        struct_layer.upper = structure_root["upper"].asInt();
                    }
                    if (structure_root.isMember("frequency") && (structure_root["frequency"].type() == Json::realValue || structure_root["frequency"].type() == Json::intValue) ) {
                        struct_layer.frequency = structure_root["frequency"].asFloat();
                    }
                    binfo.structures.push_back(struct_layer);
                }
            }
            if (biome_node.isMember("events") && biome_node["events"].type() == Json::arrayValue) {
                int event_size = biome_node["events"].size();
                for (int j = 0; j < event_size; j++) {
                    Json::Value event_root = biome_node["events"][j];
                    if (event_root.type() != Json::objectValue)
                        continue;
                    
                    event_layer_info evt_layer;
                    block_layer_info* evt_blk_layer = &(evt_layer.layer);
                    Json::Value::Members members(event_root.getMemberNames());
                    for (Json::Value::Members::iterator it = members.end() - 1;
                         it >= members.begin();
                         --it) {
                        const std::string& name = *it;
                        if (name.compare("lower") == 0 && event_root[name].type() == Json::intValue) {
                            evt_blk_layer->lower = event_root[name].asInt();
                        }
                        else if (name.compare("upper") == 0 && event_root[name].type() == Json::intValue) {
                            evt_blk_layer->upper = event_root[name].asInt();
                        }
                        else if (name.compare("frequency") == 0 && (event_root[name].type() == Json::realValue || event_root[name].type() == Json::intValue)) {
                            evt_blk_layer->frequency = event_root[name].asFloat();
                        }
                        else if (name.compare("id") == 0 && event_root[name].type() == Json::stringValue) {
                            evt_layer.name = event_root[name].asString();
                        }
                        else if (event_root[name].type() == Json::intValue || event_root[name].type() == Json::realValue) {
                            // save into mapping directly
                            evt_layer.event_parameters[name] = event_root[name].asFloat();
                        }
                        else if (event_root[name].type() == Json::stringValue) {
                            // this must be a string referring to a block ID
                            uint16_t bid = get_block_id_from_name(event_root[name]);
                            evt_layer.event_parameters[name] = bid;
                        }
                    }
                    binfo.events.push_back(evt_layer);
                }
            }
            game_data_object->biome_info.push_back(binfo);
        }
    }
    
    // read modes
    if (root.isMember("modes") && root["modes"].type() == Json::arrayValue) {
        int size = root["modes"].size();
        for (int i = 0; i < size; i++){
            Json::Value mode_node = root["modes"][i];
            if (mode_node.type() != Json::objectValue) {
                continue;
            }
            world_gen_mode_info winfo;
            if (mode_node.isMember("name") && mode_node["name"].type() == Json::stringValue) {
                winfo.name = mode_node["name"].asString();
            }
            if (mode_node.isMember("biomes") && mode_node["biomes"].type() == Json::objectValue) {
                Json::Value::Members members(mode_node["biomes"].getMemberNames());
                for (Json::Value::Members::iterator it = members.begin();
                     it != members.end();
                     ++it) {
                    const std::string& biome_name = *it;
                    uint16_t biome_id =  get_biome_id_from_name(biome_name);
                    winfo.biome_frequencies[biome_id] = mode_node["biomes"][biome_name].asFloat();
                }
            }
            if (mode_node.isMember("sector") && mode_node["sector"].type() == Json::arrayValue && mode_node["sector"].size() == 2) {
                if (mode_node["sector"][0].type() == Json::intValue &&
                    mode_node["sector"][1].type() == Json::intValue) {
                    winfo.xsectorsize = mode_node["sector"][0].asInt();
                    winfo.zsectorsize = mode_node["sector"][1].asInt();
                }
            }
            if (mode_node.isMember("biomecount") && mode_node["biomecount"].type() == Json::arrayValue) {
                if (mode_node["biomecount"].size() >= 1 && mode_node["biomecount"][0].type() == Json::intValue) {
                    winfo.biomepointavg = mode_node["biomecount"][0].asInt();
                }
                if (mode_node["biomecount"].size() >= 2 && mode_node["biomecount"][1].type() == Json::intValue) {
                    winfo.biomepointvar = mode_node["biomecount"][1].asInt();
                }
            }
            if (mode_node.isMember("islandcount") && mode_node["islandcount"].type() == Json::arrayValue) {
                if (mode_node["islandcount"].size() >= 1 && mode_node["islandcount"][0].type() == Json::intValue) {
                    winfo.islandpointavg = mode_node["islandcount"][0].asInt();
                }
                if (mode_node["islandcount"].size() >= 2 && mode_node["islandcount"][1].type() == Json::intValue) {
                    winfo.islandpointvar = mode_node["islandcount"][1].asInt();
                }
            }
            if (mode_node.isMember("separation") && (mode_node["separation"].type() == Json::realValue || mode_node["separation"].type() == Json::intValue)) {
                winfo.separation = mode_node["separation"].asFloat();
            }
            if (mode_node.isMember("fatness") && (mode_node["fatness"].type() == Json::realValue || mode_node["fatness"].type() == Json::intValue)) {
                winfo.fatness = mode_node["fatness"].asFloat();
            }
            if (mode_node.isMember("melt") && (mode_node["melt"].type() == Json::realValue || mode_node["melt"].type() == Json::intValue)) {
                winfo.melt_distance = mode_node["melt"].asFloat();
            }
            if (mode_node.isMember("octaves") && mode_node["octaves"].type() == Json::intValue) {
                winfo.octaves = mode_node["octaves"].asInt();
            }
            if (mode_node.isMember("lowerstrength") && (mode_node["lowerstrength"].type() == Json::realValue || mode_node["lowerstrength"].type() == Json::intValue)) {
                winfo.lower_strength = mode_node["lowerstrength"].asFloat();
            }
            if (mode_node.isMember("lowerpersistence") && (mode_node["lowerpersistence"].type() == Json::realValue || mode_node["lowerpersistence"].type() == Json::intValue)) {
                winfo.lower_persistence = mode_node["lowerpersistence"].asFloat();
            }
            game_data_object->world_gen_modes.push_back(winfo);
        }
    }
    
    setup_world_generator(&(game_data_object->world_gen_modes[0]), 101);
    
    return 0;
}


int GameInfoDataObject::read_values() {
    // first, read the whole file into a string
    std::string raw_json;
    
    std::ifstream file(get_path_to_game_json());
    
    file.seekg(0, std::ios::end);
    raw_json.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    
    raw_json.assign((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    
    file.close();
    
    Json::Value root;
    Json::Reader reader;
    
    bool success = reader.parse(raw_json, root);
    if (!success) {
        printf("Error encountered while parsing gameinfo.json\n");
        printf("A helpful place to validate that JSON is at http://jsonlint.com/");
        return 1;
    }
    
    if (root.isMember("version")) {
        version = root["version"].asInt();
    }
    else {
        printf("Version missing from game files\n");
        return 1;
    }
    
    if (root.isMember("blocks"))
        read_blocks(root["blocks"]);
    if (root.isMember("models"))
        read_models(root["models"]);
    if (root.isMember("recipes"))
        read_recipes(root["recipes"]);
    if (root.isMember("world_gen"))
        read_world_gen(root["world_gen"]);
    
    return 0;
}

int load_game_info() {
    game_data_object = new GameInfoDataObject();
    return game_data_object->read_values();
}

void free_game_info() {
    delete game_data_object;
}

uint16_t get_block_texture(block_type blk, BlockOrientation face) {
    uint16_t block_id = blk.type;
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        int texture = game_data_object->recipe_block_info[block_id].texture;
        if (texture < 0) {
            // use the array instead
            BlockOrientation actual_face = get_translated_orientation(blk.orientation, face);
            return game_data_object->recipe_block_info[block_id].textures[actual_face];
        }
        else {
            return texture;
        }

    }
    else {
        if (block_id >= game_data_object->block_info.size()) {
            return block_id;
        }
        int texture = game_data_object->block_info[block_id].texture;
        if (texture < 0) {
            // use the array instead
            BlockOrientation actual_face = get_translated_orientation(blk.orientation, face);
            return game_data_object->block_info[block_id].textures[actual_face];
        }
        else {
            return texture;
        }
    }
    return 0;
}

bool get_block_is_model(block_type blk) {
    uint16_t block_id = blk.type;
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        if (game_data_object->recipe_block_info[block_id].is_model) {
            // only render the model if you are the center
            return blk.relx == 0 && blk.rely == 0 && blk.relz == 0;
        }
    }
    else {
        return game_data_object->block_info[block_id].is_model;
    }
    return false;
}

bool get_block_is_light(block_type blk) {
    uint16_t block_id = blk.type;
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        return game_data_object->recipe_block_info[block_id].light.should_render();
    }
    else {
        return game_data_object->block_info[block_id].light.should_render();
    }
    return false;
}

int get_block_resistance(uint16_t block_id) {
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        return game_data_object->recipe_block_info[block_id].resistance;
    }
    else {
        return game_data_object->block_info[block_id].resistance;
    }
}

int get_block_transparency(uint16_t block_id) {
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        return game_data_object->recipe_block_info[block_id].transparency;
    }
    else {
        return game_data_object->block_info[block_id].transparency;
    }
}

int get_block_weight(uint16_t block_id) {
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        return game_data_object->recipe_block_info[block_id].weight;
    }
    else {
        return game_data_object->block_info[block_id].weight;
    }
}

int get_block_independence(uint16_t block_id) {
    if (block_id >= recipe_mask) {
        block_id -= recipe_mask;
        return game_data_object->recipe_block_info[block_id].vehicle;
    }
    else {
        return game_data_object->block_info[block_id].vehicle;
    }
}

RenderableModel* get_game_model(uint16_t model_id) {
    return &(game_data_object->model_info[model_id]);
}

block_mouse_callback_func get_block_mouse_callback_from(block_type block_id) {
    if (game_data_object->block_callback_info.count(block_id.type)) {
        return game_data_object->block_callback_info[block_id.type].mouse_callback;
    }
    return 0;
}

block_keyboard_callback_func get_block_keyboard_callback_from(block_type block_id) {
    if (game_data_object->block_callback_info.count(block_id.type)) {
        return game_data_object->block_callback_info[block_id.type].keyboard_callback;
    }
    return 0;
}

bool has_block_keyboard_bindings(block_type block_id) {
    if (game_data_object->block_callback_info.count(block_id.type)) {
        return true;
    }
    return false;
}

std::vector<Action> get_block_default_keyboard_bindings(block_type block_id) {
    return game_data_object->block_callback_info[block_id.type].key_bindings;
}

CursorItem* get_recipe_cursoritem_from(uint16_t vid) {
    if (game_data_object->recipe_info[vid].from_file) {
        // make a superobject loaded from file path
        // game_data_object->recipe_info[vid].file_path;
        // TODO
        return 0;
    }
    else {
        // construct a CursorSuperObject holding recipe contents
        CursorSuperObject* object = new CursorSuperObject(vid);
        for (int i = 0; i < game_data_object->recipe_info[vid].blks.size(); i++) {
            ivec3 pos = game_data_object->recipe_info[vid].positions[i];
            uint16_t blk = game_data_object->recipe_info[vid].blks[i];
            BlockOrientation orientation = game_data_object->recipe_info[vid].orientations[i];
            block_type block(blk, orientation, 0);
            object->set_block_integral(pos.x, pos.y, pos.z, block);
        }
        return object;
    }
}

void fill_game_models(std::vector<fvec3> &model_vertices,
                      std::vector<fvec3> &model_normals,
                      std::vector<fvec3> &model_uvs,
                      block_type block,
                      int x, int y, int z) {
    uint16_t block_id = block.type;
    std::vector<fvec3> vertices = game_data_object->block_model_info[block_id].vertices;
    std::vector<fvec3> normals = game_data_object->block_model_info[block_id].normals;
    std::vector<fvec3> uvs = game_data_object->block_model_info[block_id].uvs;
    
    model_vertices.reserve(model_vertices.size() + vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        fvec3 translated(vertices[i].x + x, vertices[i].y + y, vertices[i].z + z + 1);
        model_vertices.push_back(translated);
    };
    
    // normals and uvs dont need to add xyz
    model_normals.insert(model_normals.end(), normals.begin(), normals.end());
    model_uvs.insert(model_uvs.end(), uvs.begin(), uvs.end());
}

float get_biome_strength(uint16_t biome) {
    // TOFU this could be unsafe array-size wise! we're being really trusting here
    return game_data_object->biome_info[biome].strength;
}

float get_biome_persistence(uint16_t biome) {
    // TOFU this could be unsafe array-size wise! we're being really trusting here
    return game_data_object->biome_info[biome].persistence;
}

uint16_t get_random_block_from_biome(uint16_t biome, int depth) {
    float total_frequency = 0;
    for (auto &i: game_data_object->biome_info[biome].blocks) {
        if (depth >= i.lower && depth <= i.upper) {
            total_frequency += i.frequency;
        }
    }
    
    int precision = 10000;
    float rv = (rand() % precision) * 1.0 / precision;
    float total = 0;
    for (auto &i: game_data_object->biome_info[biome].blocks) {
        if (depth >= i.lower && depth <= i.upper) {
            total += i.frequency;
            if (rv < total / total_frequency) {
                return i.type;
            }
        }
    }
    return 0;
    
}

void add_struct_in_biome_randomly(uint16_t biome, ivec3 pos,
                                  std::vector<uint16_t> &sids,
                                  std::vector<ivec3> &dimensions,
                                  std::vector<ivec3> &positions) {
    int precision = 10000;
    for (int i = 0; i < game_data_object->biome_info[biome].structures.size(); i++) {
        float rv = (rand() % precision) * 1.0 / precision;
        block_layer_info current = game_data_object->biome_info[biome].structures[i];
        if (rv < current.frequency) {
            sids.push_back(current.type);
            ivec3 pos_off;
            if (current.type >= recipe_mask) {
                // this is a recipe
                dimensions.push_back(game_data_object->recipe_info[current.type - recipe_mask].upper);
                pos_off.x = game_data_object->recipe_info[current.type - recipe_mask].upper.x / 2;
                pos_off.y = game_data_object->recipe_info[current.type - recipe_mask].upper.y / 2;
                pos_off.z = game_data_object->recipe_info[current.type - recipe_mask].upper.z / 2;
            }
            else {
                dimensions.push_back(ivec3(1, 1, 1));
                pos_off = ivec3(0,0,0);
            }
            int height = current.lower;
            if (current.upper > current.lower) {
                height = current.lower + (rand() % (current.upper - current.lower));
            }
            ivec3 struct_pos = ivec3(pos.x - pos_off.x, pos.y - height - 1, pos.z - pos_off.z);
            positions.push_back(struct_pos);
        }
    }
}

bool is_within_chunk(ivec3 pos) {
    return pos.x >= 0 && pos.x < CX
    && pos.y >= 0 && pos.y < CY
    && pos.z >= 0 && pos.z < CZ;
}

void add_recipe_block_to_chunk(block_type to_arr[CX][CY][CZ], uint16_t recipe,
                               ivec3 chunk_pos, ivec3 recipe_pos) {
    // get vector of blocks at the position
    // offset it by the position of the structure
    // then see if that position is within our chunk
    // if so, then scale it into CRC and then set it in our array
    if (recipe < recipe_mask) {
        ivec3 aligned_pos = ivec3(recipe_pos.x - chunk_pos.x*CX,
                                  recipe_pos.y - chunk_pos.y*CY,
                                  recipe_pos.z - chunk_pos.z*CZ);
        if (is_within_chunk(aligned_pos)) {
            to_arr[aligned_pos.x][aligned_pos.y][aligned_pos.z] = block_type(recipe);
        }
    }
    else {
        int recipe_block_count = (int)game_data_object->recipe_info[recipe - recipe_mask].blks.size();
        for (int i = 0; i < recipe_block_count; i++) {
            ivec3 offset = game_data_object->recipe_info[recipe - recipe_mask].positions[i];
            ivec3 aligned_pos = ivec3(recipe_pos.x - chunk_pos.x*CX + offset.x,
                                      recipe_pos.y - chunk_pos.y*CY + offset.y,
                                      recipe_pos.z - chunk_pos.z*CZ + offset.z);
            if (is_within_chunk(aligned_pos)) {
                uint16_t block = game_data_object->recipe_info[recipe - recipe_mask].blks[i];
                to_arr[aligned_pos.x][aligned_pos.y][aligned_pos.z] = block_type(block);
            }
        }
    }
}


