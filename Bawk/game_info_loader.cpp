//
//  game_info_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 10/22/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "game_info_loader.h"

#include "obj_data_loader.h"
#include "block_loader.h"

#include "json/json.h"

#include "modeldata.h"

#include "cursorsuperobject.h"
#include "modelrender.h"

#include "spriterender.h"
#include "spritegetter.h"

#include "modelaction.h"

#include "terraingenerator.h"

#include "json_reader_helper.h"

struct recipe_game_info {
    std::string name;
    bool from_file;
    std::string file_path;
    std::vector<ivec3> positions;
    ivec3 upper;    // the bounds of this object, after it has been zero'd (which will happen automatically)
    std::vector<uint16_t> blks;
    std::vector<BlockOrientation> orientations;
};

struct sprite_game_info {
    std::string name;
    SpriteRender sprite;
};

class GameInfoDataObject {
    int read_blocks(Json::Value root);
    int read_models(Json::Value root);
    int read_sprites(Json::Value root);
    int read_recipes(Json::Value root);
    int read_world_gen(Json::Value root);
public:
    int version;
    std::vector<BlockInfo> block_info;
    std::vector<RenderableModel> model_info;
    std::map<uint16_t, ModelData> block_model_info;
    std::vector<sprite_game_info> sprite_info;
    std::vector<recipe_game_info> recipe_info;
    std::vector<BiomeGenerator> biome_info;
    std::vector<SectorGenerator> world_gen_modes;
    
    GameInfoDataObject();
    ~GameInfoDataObject();
    int read_values();
    // accessor methods
    
};

// store the current game data object as a blobal
GameInfoDataObject* game_data_object;

GameInfoDataObject::GameInfoDataObject() {
    
}

GameInfoDataObject::~GameInfoDataObject() {
    
}

int GameInfoDataObject::read_blocks(Json::Value root) {
    if (root.type() != Json::objectValue)
        return 1;
    
    Json::Value block;
    BlockInfo* info;
    
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
                block_model_info[block_id] = ModelData();
                read_obj_file(&(block_model_info[block_id]),
                              block["texture"].asString(),
                              get_texturename_from_string(json_read_string_or_empty(block["resource"])));
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
            get_block_mouse_callback_for(block["action"].asString(), info->mouse_callbacks);
            get_block_keyboard_callback_for(block["action"].asString(), info->keyboard_callbacks);
            info->set_callback_checks();
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
    
    // read in all names first, in case we have some property that recursively asks for an ID given a name
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
    }
    
    for (Json::Value::Members::iterator it = members.end() - 1;
         it >= members.begin();
         --it) {
        const std::string& name = *it;
        uint16_t model_id = std::stoi(name);
        info = &(model_info[model_id]);
        model = root[name];
        if (model.isMember("resistance") && model["resistance"].type() == Json::intValue) {
            info->resistance = model["resistance"].asInt();
        }
        if (model.isMember("texture")) {
            read_obj_file(info,
                          model["texture"].asString(),
                          get_texturename_from_string(json_read_string_or_empty(model["resource"])));
        }
        if (model.isMember("weight") && model["weight"].type() == Json::intValue) {
            info->weight = model["weight"].asInt();
        }
        if (model.isMember("vehicle") && model["vehicle"].type() == Json::intValue) {
            info->vehicle = model["vehicle"].asInt();
        }
        if (model.isMember("action")) {
            info->multiplexer = get_model_action_multiplexer_from(model["action"]);
        }
        else {
            info->multiplexer = 0;
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

uint16_t get_sprite_id_from_name(const std::string &name) {
    for (uint16_t i = 0; i < game_data_object->sprite_info.size(); i++) {
        if (game_data_object->sprite_info[i].name.compare(name) == 0) {
            return i;
        }
    }
    return 0;
}

int GameInfoDataObject::read_sprites(Json::Value root) {
    if (root.type() != Json::objectValue)
        return 1;
    
    Json::Value sprite;
    sprite_game_info* info;
    
    Json::Value::Members members(root.getMemberNames());
    for (Json::Value::Members::iterator it = members.end() - 1;
         it >= members.begin();
         --it) {
        const std::string& name = *it;
        uint16_t sprite_id = std::stoi(name);
        if (sprite_id >= sprite_info.size()) {
            sprite_info.resize(sprite_id + 1);
        }
        info = &(sprite_info[sprite_id]);
        sprite = root[name];
        if (sprite.isMember("name") && sprite["name"].isString()) {
            info->name.assign(sprite["name"].asString().c_str());
        }
        if (sprite.isMember("properties")) {
            set_sprite_properties(&(info->sprite), sprite["properties"]);
        }
        if (sprite.isMember("duration") && sprite["duration"].isInt()) {
            info->sprite.duration = sprite["duration"].asInt();
        }
        if (sprite.isMember("light") && sprite["light"].type() == Json::arrayValue) {
            int size = sprite["light"].size();
            if (size >= 2) {
                info->sprite.light.light_radius = sprite["light"][0].asFloat();
                info->sprite.light.light_intensity = sprite["light"][1].asFloat();
            }
        }
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
        return 0;
    }
    else {
        return node.asInt();
    }
}

uint16_t get_model_id_from_name(Json::Value node) {
    if (node.type() == Json::stringValue) {
        for (uint16_t i = 0; i < game_data_object->block_info.size(); i++) {
            if (game_data_object->model_info[i].name.compare(node.asString()) == 0) {
                return i;
            }
        }
        return 0;
    }
    else {
        return node.asInt();
    }
}

uint16_t get_recipe_id_from_name(Json::Value node) {
    if (node.type() == Json::stringValue) {
        for (uint16_t i = 0; i < game_data_object->recipe_info.size(); i++) {
            if (game_data_object->recipe_info[i].name.compare(node.asString()) == 0) {
                return i;
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
    
    Json::Value::Members members(root.getMemberNames());
    for (Json::Value::Members::iterator it = members.end() - 1;
         it >= members.begin();
         --it) {
        const std::string& name = *it;
        uint16_t recipe_id = std::stoi(name);
        if (recipe_id >= recipe_info.size()) {
            recipe_info.resize(recipe_id + 1);
        }
        info = &(recipe_info[recipe_id]);
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
                        
                        uint16_t bid = recipe_id;
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
                // TOFU using the texture block model obj data, automatically fill in blocks where the model has points
                // then assign a center block as the center of the object
                printf("this feature is so far out from development so don't even trip bro\n");
            }
            else {
                // this is the name of a file.
                info->from_file = true;
                info->file_path = get_path_to_game_folder() + "/" +  recipe["blockfile"].asString();
            }
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
            BiomeGenerator binfo = BiomeGenerator();
            binfo.biome_id = i;
            if (biome_node.isMember("name") && biome_node["name"].type() == Json::stringValue) {
                binfo.name = biome_node["name"].asString();
            }
            if (biome_node.isMember("strength") && (biome_node["strength"].type() == Json::realValue || biome_node["strength"].type() == Json::intValue)) {
                binfo.strength = biome_node["strength"].asFloat();
            }
            if (biome_node.isMember("persistence") && (biome_node["persistence"].type() == Json::realValue || biome_node["persistence"].type() == Json::intValue)) {
                binfo.persistence = biome_node["persistence"].asFloat();
            }
            if (biome_node.isMember("terrain") && biome_node["terrain"].type() == Json::arrayValue) {
                int terrain_size = biome_node["terrain"].size();
                for (int j = 0; j < terrain_size; j++) {
                    Json::Value terrain_root = biome_node["terrain"][j];
                    if (terrain_root.type() == Json::stringValue) {
                        TerrainGenerators::TerrainGeneratorTypes terrain_type = get_terrain_generator_type_from(terrain_root.asString());
                        TerrainGenerator* generator = get_terrain_generator(terrain_type);
                        if (generator) {
                            binfo.valid_generators.push_back(generator);
                        }
                    }
                }
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
                        struct_layer.type = get_recipe_id_from_name(structure_root["id"]);
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
            /*if (biome_node.isMember("events") && biome_node["events"].type() == Json::arrayValue) {
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
            }*/
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
            SectorGenerator winfo;
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
                    winfo.sector_xwidth = mode_node["sector"][0].asInt();
                    winfo.sector_zwidth = mode_node["sector"][1].asInt();
                }
            }
            if (mode_node.isMember("biomecount") && mode_node["biomecount"].type() == Json::arrayValue) {
                if (mode_node["biomecount"].size() >= 1 && mode_node["biomecount"][0].type() == Json::intValue) {
                    winfo.biome_count_avg = mode_node["biomecount"][0].asInt();
                }
                if (mode_node["biomecount"].size() >= 2 && mode_node["biomecount"][1].type() == Json::intValue) {
                    winfo.biome_count_var = mode_node["biomecount"][1].asInt();
                }
            }
            if (mode_node.isMember("islandcount") && mode_node["islandcount"].type() == Json::arrayValue) {
                if (mode_node["islandcount"].size() >= 1 && mode_node["islandcount"][0].type() == Json::intValue) {
                    winfo.island_count_avg = mode_node["islandcount"][0].asInt();
                }
                if (mode_node["islandcount"].size() >= 2 && mode_node["islandcount"][1].type() == Json::intValue) {
                    winfo.island_count_var = mode_node["islandcount"][1].asInt();
                }
            }
            if (mode_node.isMember("fillcount") && mode_node["fillcount"].type() == Json::arrayValue) {
                if (mode_node["fillcount"].size() >= 1) {
                    winfo.fill_perc_avg = json_read_float_or_zero(mode_node["fillcount"][0]);
                }
                if (mode_node["fillcount"].size() >= 2) {
                    winfo.fill_perc_var = json_read_float_or_zero(mode_node["fillcount"][1]);
                }
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
            winfo.seed = 101;
            game_data_object->world_gen_modes.push_back(winfo);
        }
    }
    // TODO change this so this takes by name instead ("default")
    
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
    if (root.isMember("sprites"))
        read_sprites(root["sprites"]);
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

BlockInfo* get_block_info(uint16_t block_id) {
    if (block_id >= game_data_object->block_info.size()) {
        // TODO return invalid block
        return &(game_data_object->block_info[0]);
    }
    return &(game_data_object->block_info[block_id]);
}

RenderableModel* get_game_model(uint16_t model_id) {
    return &(game_data_object->model_info[model_id]);
}

bool call_block_mouse_callback_from(block_type* block_id, Player* player, Entity* ent, ivec3 pos, Action a) {
    block_callback_func func = game_data_object->block_info[block_id->type].mouse_callbacks[a];
    return (*func)(player, ent, block_id, pos);
}

void get_block_keyboard_callback_from(block_type block_id, std::map<Action, block_callback_func> &funcs) {
    for (auto &i: game_data_object->block_info[block_id.type].keyboard_callbacks) {
        funcs[i.first] = i.second;
    }
}

bool has_block_mouse_action(block_type block_id) {
    return game_data_object->block_info[block_id.type].has_mouse_callback;
}

bool has_block_keyboard_action(block_type block_id) {
    // TODO this has been disabled and this is a zombie function
    return game_data_object->block_info[block_id.type].has_keyboard_callback;
}

SpriteRender* get_sprite_renderable(uint16_t sid) {
    return &(game_data_object->sprite_info[sid].sprite);
}

CursorSuperObject* get_recipe_cursoritem_from(uint16_t vid) {
    CursorItemInfo* info = new CursorItemInfo(vid, CursorType::CURSOR_SUPEROBJECT);
    CursorSuperObject* object = new CursorSuperObject(info);
    if (game_data_object->recipe_info[vid].from_file) {
        // make a superobject loaded from file path
        std::string path = game_data_object->recipe_info[vid].file_path;
        object->init(path);
    }
    else {
        // construct a CursorSuperObject holding recipe contents
        for (int i = 0; i < game_data_object->recipe_info[vid].blks.size(); i++) {
            ivec3 pos = game_data_object->recipe_info[vid].positions[i];
            uint16_t blk = game_data_object->recipe_info[vid].blks[i];
            BlockOrientation orientation = game_data_object->recipe_info[vid].orientations[i];
            block_type block(blk, orientation, 0);
            object->set_block_integral(pos.x, pos.y, pos.z, block);
        }
    }
    return object;
}

void fill_game_models(std::vector<fvec3> &model_vertices,
                      std::vector<fvec3> &model_normals,
                      std::vector<fvec3> &model_uvs,
                      block_type block,
                      int x, int y, int z) {
    uint16_t block_id = block.type;
    std::vector<fvec3> vertices = game_data_object->block_model_info[block_id].model_vertices;
    std::vector<fvec3> normals = game_data_object->block_model_info[block_id].model_normals;
    std::vector<fvec3> uvs = game_data_object->block_model_info[block_id].model_uvs;
    
    model_vertices.reserve(model_vertices.size() + vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        fvec3 translated(vertices[i].x + x, vertices[i].y + y, vertices[i].z + z + 1);
        model_vertices.push_back(translated);
    };
    
    // normals and uvs dont need to add xyz
    model_normals.insert(model_normals.end(), normals.begin(), normals.end());
    model_uvs.insert(model_uvs.end(), uvs.begin(), uvs.end());
}

BiomeGenerator* get_biome(uint16_t biome) {
    return &(game_data_object->biome_info[biome]);
}

SectorGenerator* get_sector_generator(uint16_t sid) {
    return &(game_data_object->world_gen_modes[sid]);
}
