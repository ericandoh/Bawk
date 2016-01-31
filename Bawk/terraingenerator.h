//
//  terraingenerator.h
//  Bawk
//
//  Created by Eric Oh on 1/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__terraingenerator__
#define __Bawk__terraingenerator__

#include <stdio.h>
#include <string>

namespace TerrainGenerators {
    enum TerrainGeneratorTypes: int {
        DEFAULT, BRITTLE, FLAT, HILLY, MOUNTAINOUS, CANYON, TERRAIN_GENERATOR_SIZE
    };
}

class TerrainGenerator {
    // used for terrain generation
    int off_x, off_z;
protected:
    // params
    int seed;           // shared between all biomes
    int unique_seed;    // unique per biome
    float magnitude, frequency;
    float noise_2d(float x, float z, float magnitude, float freq);
public:
    virtual void set_seed(int s, int si);
    void set_params(float mag, float freq);
    virtual float get_val(float x, float z);
};

TerrainGenerators::TerrainGeneratorTypes get_terrain_generator_type_from(std::string name);
TerrainGenerator* get_terrain_generator(TerrainGenerators::TerrainGeneratorTypes type);
void free_terrain_generators();

void test_terrain_generator();

#endif /* defined(__Bawk__terraingenerator__) */
