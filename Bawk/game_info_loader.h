//
//  Loads info about the game, like what block types have what definitions
//  - Unlike block_loader, loads/saves in plaintext format
//  - Later can make it compile a plaintext file into a data file to be used by the game
//  - (if we want to make the game files non-edittable, which is unlikely bc EDIT ALL THE THINGS)
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//  Created by Eric Oh on 10/22/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game_info_loader__
#define __Bawk__game_info_loader__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "basic_types.h"
#include "json/json.h"
#include "block.h"

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
    std::vector<fvec2> uvs;
    std::vector<fvec3> normals;
};

struct recipe_game_info {
    std::string name;
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

class GameInfoDataObject {
    int version;
    std::vector<block_game_info> block_info;
    std::vector<block_game_info> recipe_block_info;
    std::unordered_map<uint16_t, model_game_info> block_model_info;
    std::vector<recipe_game_info> recipe_info;
    std::vector<biome_game_info> biome_info;
    
    int read_blocks(Json::Value root);
    int read_recipes(Json::Value root);
    int read_world_gen(Json::Value root);
public:
    GameInfoDataObject();
    ~GameInfoDataObject();
    int read_values();
    // accessor methods
    
};

#endif /* defined(__Bawk__game_info_loader__) */
