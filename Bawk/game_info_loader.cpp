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

// add this to every recipe ID
// +1, so 0 doesn't map to 0 in the actual block
const uint16_t recipe_mask = 0x1 << 15;

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
    block_game_info() {
        name = "";
        is_model = false;
        texture = 0;
        resistance = 0;
        transparency = 0;
        weight = 0;
        vehicle = 0;
    }
};

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
    std::vector<uint16_t> blks;
    std::vector<BlockOrientation> orientations;
};

struct block_layer_game_info {
    uint16_t type;
    int lower;
    int upper;
    int frequency;
};

struct structure_gen_game_info {
    std::vector<uint16_t> types;
    int lower;
    int upper;
    int spacing;
};

struct biome_game_info {
    std::string name;
    int frequency;
    int roughness;
    std::vector<block_layer_game_info> layers;
    std::vector<structure_gen_game_info> structures;
};

struct ui_block_callback_info {
    block_mouse_callback_func mouse_callback;
    block_keyboard_callback_func keyboard_callback;
    std::vector<int> key_bindings;
};

class GameInfoDataObject {
    int read_blocks(Json::Value root);
    int read_recipes(Json::Value root);
    int read_world_gen(Json::Value root);
public:
    int version;
    std::vector<block_game_info> block_info;
    std::vector<block_game_info> recipe_block_info;
    std::map<uint16_t, model_game_info> block_model_info;
    std::map<uint16_t, ui_block_callback_info> block_callback_info;
    std::vector<recipe_game_info> recipe_info;
    std::vector<biome_game_info> biome_info;
    
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
            printf("frog\n");
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
            printf("frog\n");
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

std::vector<int> get_block_default_keyboard_bindings(block_type block_id) {
    if (game_data_object->block_callback_info.count(block_id.type)) {
        return game_data_object->block_callback_info[block_id.type].key_bindings;
    }
    return std::vector<int>();
}

CursorItem* get_recipe_cursoritem_from(uint16_t vid) {
    if (game_data_object->recipe_info[vid].from_file) {
        // make a superobject loaded from file path
        // game_data_object->recipe_info[vid].file_path;
        return 0;
    }
    else {
        // construct a CursorSuperObject holding recipe contents
        CursorSuperObject* object = new CursorSuperObject(0, vid);
        ivec3 first_pos;
        for (int i = 0; i < game_data_object->recipe_info[vid].blks.size(); i++) {
            ivec3 pos = game_data_object->recipe_info[vid].positions[i];
            uint16_t blk = game_data_object->recipe_info[vid].blks[i];
            BlockOrientation orientation = game_data_object->recipe_info[vid].orientations[i];
            block_type block(blk, orientation, 0);
            if (i == 0) {
                block.relx = block.rely = block.relz = 0;
                first_pos = pos;
            }
            else {
                // calculate block relx/rely/relz from above
                block.relx = pos.x - first_pos.x;
                block.rely = pos.y - first_pos.y;
                block.relz = pos.z - first_pos.z;
            }
            object->set_block(pos.x, pos.y, pos.z, block);
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
