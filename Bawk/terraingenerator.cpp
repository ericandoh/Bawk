//
//  terraingenerator.cpp
//  Bawk
//
//  Created by Eric Oh on 1/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "terraingenerator.h"
#include "basic_types.h"
#include <glm/gtc/noise.hpp>
#include <vector>

float TerrainGenerator::noise_2d(float x, float z, float magnitude, float freq) {
    // magnitude: how big the changes in slope are, in unit blocks
    // frequency: how often we have said changes in slope, roughly in unit blocks
    
    // setting magnitude = 1/2 * freq will make the average slope 1.0
    // doubling magnitude will make the average slope 2.0, as will halving the scale
    
    return magnitude * glm::simplex(fvec2((x+off_x) / freq, (z+off_z) / freq));
}

void TerrainGenerator::set_seed(int s, int si) {
    seed = s;
    unique_seed = si;
    srand(seed);
    off_x = rand() % (0x1 << 16);
    off_z = rand() % (0x1 << 16);
}

void TerrainGenerator::set_params(float mag, float freq) {
    magnitude = mag;
    frequency = freq;
}

float TerrainGenerator::get_val(float x, float z) {
    return noise_2d(x, z, magnitude, frequency);
}

class DefaultTerrainGenerator: public TerrainGenerator {
    float get_val(float x, float z) override;
};

float DefaultTerrainGenerator::get_val(float x, float z) {
    return 1.0f;
}

class PlateauTerrainGenerator: public TerrainGenerator {
    int levels;
    float range_per_level;
    std::vector<int> cutoffs;
public:
    void set_seed(int s, int si) override;
    void set_levels(float mag, float freq, int l);
    float get_val(float x, float z) override;
};

void PlateauTerrainGenerator::set_seed(int s, int si) {
    // partition all numbers between -mag to mag to some number
    srand(seed);
    srand(rand() + unique_seed);
    if (cutoffs.size() < levels) {
        cutoffs.resize(levels);
    }
    for (int i = 0; i < levels; i++) {
        cutoffs[i] = range_per_level * i + range_per_level * choose_random_one();
    }
}

void PlateauTerrainGenerator::set_levels(float mag, float freq, int l) {
    set_params(mag, freq);
    levels = l;
    float range = magnitude * 2.0f;
    range_per_level = range / levels;
}

float PlateauTerrainGenerator::get_val(float x, float z) {
    float val = TerrainGenerator::get_val(x, z);
    int level = (int)((val + magnitude) / range_per_level);
    if (level >= 0 && level < levels) {
        return cutoffs[level];
    }
    return 0;
}

class MultiTerrainGenerator: public TerrainGenerator {
    std::vector<TerrainGenerator*> generators;
public:
    ~MultiTerrainGenerator();
    void add_generator(TerrainGenerator* gen);
    void set_seed(int s, int si) override;
    float get_val(float x, float z) override;
};

MultiTerrainGenerator::~MultiTerrainGenerator() {
    for (auto &generator: generators) {
        delete generator;
    }
    generators.clear();
}

void MultiTerrainGenerator::add_generator(TerrainGenerator* gen) {
    generators.push_back(gen);
}

void MultiTerrainGenerator::set_seed(int s, int si) {
    TerrainGenerator::set_seed(s, si);
    for (int i = 0; i < generators.size(); i++) {
        generators[i]->set_seed(seed, si + i);
    }
}

float MultiTerrainGenerator::get_val(float x, float z) {
    float total_val = 0.0f;
    for (auto &generator: generators) {
        total_val += generator->get_val(x, z);
    }
    return total_val;
}

class StandardMultiTerrainGenerator: public MultiTerrainGenerator {
public:
    void set_multiparams(float mag, float freq, float persistence, int octaves);
};

void StandardMultiTerrainGenerator::set_multiparams(float mag, float freq, float persistence, int octaves) {
    float total_mag = 0.0f;
    float current_mag = mag;
    for (int i = 0; i < octaves; i++) {
        total_mag += current_mag;
        current_mag *= persistence;
    }
    current_mag = mag;
    float current_freq = freq;
    for (int i = 0; i < octaves; i++) {
        TerrainGenerator* generator = new TerrainGenerator();
        generator->set_params(current_mag * mag / total_mag, current_freq);
        add_generator(generator);
        current_mag *= persistence;
        current_freq /= 2.0f;
    }
}

#define TERRAIN_NOISE_SAMPLE 400

void test_terrain_generator_on(TerrainGenerator* generator) {
    generator->set_seed(100, 0);
    float sample_height_map[TERRAIN_NOISE_SAMPLE];
    float min_height = FLT_MAX;
    float max_height = FLT_MIN;
    float total_difference = 0;
    for (int i = 0; i < TERRAIN_NOISE_SAMPLE; i++) {
        float height = generator->get_val(i, 0);
        sample_height_map[i] = height;
        if (height < min_height) {
            min_height = height;
        }
        if (height > max_height) {
            max_height = height;
        }
        if (i > 0) {
            total_difference += fabsf(sample_height_map[i] - sample_height_map[i - 1]);
        }
    }
    for (int i = 0; i < TERRAIN_NOISE_SAMPLE; i++) {
        int spaces = (int)(roundf(sample_height_map[i]) - min_height + 1.0f);
        if (spaces > 50) {
            spaces = 50;
        }
        for (int j = 0; j < spaces; j++) {
            printf(" ");
        }
        printf("|\n");
    }
    printf("Average change in slope: %f\n", total_difference / (TERRAIN_NOISE_SAMPLE - 1.0f));
    printf("Max: %f // Min: %f\n", max_height, min_height);
}

namespace TerrainGenerators {
    DefaultTerrainGenerator* default_generator;
    StandardMultiTerrainGenerator* brittle_generator;
    StandardMultiTerrainGenerator* flat_generator;
    StandardMultiTerrainGenerator* hilly_generator;
    StandardMultiTerrainGenerator* mountainous_generator;
    MultiTerrainGenerator* plateau_generator;
    std::vector<std::string> terrain_generator_names;
    std::vector<TerrainGenerator*> terrain_generators;
    bool inited = false;
}

using namespace TerrainGenerators;

void init_terrain_generators() {
    if (inited) {
        return;
    }
    
    // DEFAULT
    default_generator = new DefaultTerrainGenerator();
    
    // BRITTLE
    brittle_generator = new StandardMultiTerrainGenerator();
    brittle_generator->set_multiparams(8.0f, 32.0f, 0.9f, 5);
    
    // FLAT
    flat_generator = new StandardMultiTerrainGenerator();
    flat_generator->set_multiparams(14.0f, 256.0f, 0.2f, 4);
    
    // HILLY
    hilly_generator = new StandardMultiTerrainGenerator();
    hilly_generator->set_multiparams(16.0f, 64.0f, 0.5f, 5);
    
    // MOUNTAINOUS
    mountainous_generator = new StandardMultiTerrainGenerator();
    mountainous_generator->set_multiparams(32.0f, 64.0f, 0.7f, 5);
    
    // CANYON
    PlateauTerrainGenerator* canyon_base = new PlateauTerrainGenerator();
    canyon_base->set_levels(32.0f, 256.0f, 5);

    StandardMultiTerrainGenerator* canyon_detail = new StandardMultiTerrainGenerator();
    canyon_detail->set_multiparams(8.0f, 64.0f, 0.3f, 3);
    
    plateau_generator = new MultiTerrainGenerator();
    plateau_generator->add_generator(canyon_base);
    plateau_generator->add_generator(canyon_detail);
    
    terrain_generators.resize(TerrainGeneratorTypes::TERRAIN_GENERATOR_SIZE);
    terrain_generators[TerrainGeneratorTypes::DEFAULT] = default_generator;
    terrain_generators[TerrainGeneratorTypes::BRITTLE] = brittle_generator;
    terrain_generators[TerrainGeneratorTypes::FLAT] = flat_generator;
    terrain_generators[TerrainGeneratorTypes::HILLY] = hilly_generator;
    terrain_generators[TerrainGeneratorTypes::MOUNTAINOUS] = mountainous_generator;
    terrain_generators[TerrainGeneratorTypes::CANYON] = plateau_generator;
    
    terrain_generator_names.resize(TerrainGeneratorTypes::TERRAIN_GENERATOR_SIZE);
    terrain_generator_names[TerrainGeneratorTypes::DEFAULT] = "default";
    terrain_generator_names[TerrainGeneratorTypes::BRITTLE] = "brittle";
    terrain_generator_names[TerrainGeneratorTypes::FLAT] = "flat";
    terrain_generator_names[TerrainGeneratorTypes::HILLY] = "hilly";
    terrain_generator_names[TerrainGeneratorTypes::MOUNTAINOUS] = "mountainous";
    terrain_generator_names[TerrainGeneratorTypes::CANYON] = "canyon";
    
    inited = true;
}

TerrainGeneratorTypes get_terrain_generator_type_from(std::string name) {
    if (!inited) {
        init_terrain_generators();
    }
    for (int i = 0; i < TerrainGeneratorTypes::TERRAIN_GENERATOR_SIZE; i++) {
        if (name.compare(terrain_generator_names[i]) == 0) {
            return static_cast<TerrainGeneratorTypes>(i);
        }
    }
    return TerrainGeneratorTypes::FLAT;
}

TerrainGenerator* get_terrain_generator(TerrainGeneratorTypes type) {
    if (!inited) {
        init_terrain_generators();
    }
    return terrain_generators[type];
}

void free_terrain_generators() {
    // TODO call this
    if (!inited) {
        return;
    }
    for (auto &generator: terrain_generators) {
        delete generator;
    }
    terrain_generators.clear();
    inited = false;
}

void test_terrain_generator() {
    init_terrain_generators();
    for (int i = 0; i < TerrainGeneratorTypes::TERRAIN_GENERATOR_SIZE; i++) {
        printf("%s\n", terrain_generator_names[i].c_str());
        test_terrain_generator_on(terrain_generators[i]);
    }
    free_terrain_generators();
    
    /*
    TerrainGenerator generator = TerrainGenerator();
    generator.set_params(32.0f, 64.0f);
    generator.set_seed(100, 0);
    test_terrain_generator_on(&generator);
    
    PlateauTerrainGenerator* plateau_generator = new PlateauTerrainGenerator();
    plateau_generator->set_levels(24.0f, 64.0f, 5);
    plateau_generator->set_seed(100, 0);
    test_terrain_generator_on(plateau_generator);
    
    StandardMultiTerrainGenerator* multi_generator = new StandardMultiTerrainGenerator();
    multi_generator->set_multiparams(8.0f, 128.0f, 0.2f, 5);
    multi_generator->set_seed(100, 0);
    test_terrain_generator_on(multi_generator);
    
    MultiTerrainGenerator plateau_multi_generator = MultiTerrainGenerator();
    plateau_multi_generator.add_generator(plateau_generator);
    plateau_multi_generator.add_generator(multi_generator);
    plateau_multi_generator.set_seed(100, 0);
    test_terrain_generator_on(&plateau_multi_generator);*/
}