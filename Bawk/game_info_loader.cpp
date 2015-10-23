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
#include "block_loader.h"

// add this to every recipe ID
const uint16_t recipe_mask = 0x1 << 15;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    // stolen shamelessly (i'm a good thief) from http://stackoverflow.com/questions/236129/split-a-string-in-c
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void read_obj_file(model_game_info* dst, std::string obj_filename) {
    // taken shamelessly from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
    
    //obj_filename = "/Users/Eric/Documents/dev/BawkAssets/simplebox.obj";
    
    std::vector<fvec3> temp_vertices;
    std::vector<fvec2> temp_uvs;
    std::vector<fvec3> temp_normals;
    
    std::ifstream file(get_path_to_game_folder() + "/" + obj_filename);
    //std::ifstream file(obj_filename);
    
    if (!file.is_open()) {
        printf("File could not be found\n");
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
            fvec2 uv;
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
    }
    return 0;
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
            for (int i = 0; i < size; i++) {
                submember = recipe["blocks"][i];
                if(submember.type() == Json::objectValue) {
                    if ((!submember.isMember("id") || submember["id"].type() == Json::intValue) && (!submember.isMember("orientation") || submember["orientation"].type() == Json::intValue || submember["orientation"].type() == Json::stringValue) && submember["position"].type() == Json::arrayValue) {
                        ivec3 position;
                        int subsize = submember["position"].size();
                        if (subsize >= 3) {
                            position.x = submember["position"][0].asInt();
                            position.y = submember["position"][1].asInt();
                            position.z = submember["position"][2].asInt();
                        }
                        
                        uint16_t bid = recipe_id + recipe_mask;
                        if (submember.isMember("id")) {
                            bid = submember["id"].asInt();
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
                                    info->orientations.push_back(BlockOrientation::FRONT);
                                }
                            }
                            else if (submember["orientation"].type() == Json::intValue) {
                                info->orientations.push_back(static_cast<BlockOrientation>(submember["orientation"].asInt()));
                            }
                            else {
                                info->orientations.push_back(BlockOrientation::FRONT);
                            }
                        }
                        else {
                            info->orientations.push_back(BlockOrientation::FRONT);
                        }
                    }
                    
                }
            }
        }
        if (recipe.isMember("blockfile") && recipe["blockfile"].type() == Json::stringValue) {
            printf("Currently not supported. Sorry!");
        }
        if (recipe.isMember("texture") && recipe["texture"].type() == Json::stringValue) {
            block_model_info[recipe_id + recipe_mask] = model_game_info();
            read_obj_file(&(block_model_info[recipe_id + recipe_mask]), recipe["texture"].asString());
            block_info->is_model = true;
        }
        if (recipe.isMember("resistance") && recipe["resistance"].type() == Json::intValue) {
            block_info->resistance = recipe["resistance"].asInt();
        }
        if (recipe.isMember("weight") && recipe["weight"].type() == Json::intValue) {
            block_info->weight = recipe["weight"].asInt();
        }
        
    }
    
    return 0;
}

int GameInfoDataObject::read_world_gen(Json::Value root) {
    // not supported currently
    printf("reading in world generation files currently not supported!\n");
    printf("frog\n");
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
    if (root.isMember("recipes"))
        read_recipes(root["recipes"]);
    if (root.isMember("world_gen"))
        read_world_gen(root["world_gen"]);
    
    return 0;
}