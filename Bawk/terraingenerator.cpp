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

class TerrainGenerator {
    // used for terrain generation
    int off_x, off_z;
protected:
    // params
    int seed;
    float magnitude, frequency;
    float noise_2d(float x, float z, float magnitude, float freq);
public:
    void set_seed(int s);
    void set_params(float mag, float freq);
    virtual float get_val(float x, float z);
};

float TerrainGenerator::noise_2d(float x, float z, float magnitude, float freq) {
    // magnitude: how big the changes in slope are, in unit blocks
    // frequency: how often we have said changes in slope, roughly in unit blocks
    
    // setting magnitude = 1/2 * freq will make the average slope 1.0
    // doubling magnitude will make the average slope 2.0, as will halving the scale
    
    return magnitude * glm::simplex(fvec2((x+off_x) / freq, (z+off_z) / freq));
}

void TerrainGenerator::set_seed(int s) {
    seed = s;
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

class PlateauTerrainGenerator: public TerrainGenerator {
    int levels;
    float range_per_level;
    std::vector<int> cutoffs;
public:
    void set_levels(float mag, float freq, int l);
    float get_val(float x, float z) override;
};

void PlateauTerrainGenerator::set_levels(float mag, float freq, int l) {
    set_params(mag, freq);
    levels = l;
    // partition all numbers between -mag to mag to some number
    float range = magnitude * 2.0f;
    range_per_level = range / levels;
    cutoffs.resize(levels);
    for (int i = 0; i < levels; i++) {
        cutoffs[i] = range_per_level * i + range_per_level * choose_random_one();
    }
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
        generator->set_seed(seed + i);
        generator->set_params(current_mag * mag / total_mag, current_freq);
        add_generator(generator);
        current_mag *= persistence;
        current_freq /= 2.0f;
    }
}


static float noise_standard2d(float x, float z, int seed, int octaves, float persistence, float strength) {
    srand(seed);
    // any more and we start losing precision
    // 2^20 is about a million, so about a million x million possible worlds. I think we good
    int add_x = rand() % (0x1 << 16);
    int add_z = rand() % (0x1 << 16);
    
    float sum = 0;
    float scale = 1.0;
    float total_strength = 0.0f;
    float base_strength = strength;
    
    for(int i = 0; i < octaves; i++) {
        sum += strength * glm::simplex(fvec2((x+add_x) / 1.0f * scale, (z+add_z) / 1.0f * scale));
        total_strength += strength;
        scale *= 2.0;
        strength *= persistence;
    }
    return std::abs(sum / total_strength * base_strength);
}

#define TERRAIN_NOISE_SAMPLE 400

void test_terrain_generator_on(TerrainGenerator* generator) {
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

void test_terrain_generator() {
    TerrainGenerator generator = TerrainGenerator();
    generator.set_seed(100);
    generator.set_params(32.0f, 64.0f);
    test_terrain_generator_on(&generator);
    
    PlateauTerrainGenerator* plateau_generator = new PlateauTerrainGenerator();
    plateau_generator->set_seed(100);
    plateau_generator->set_levels(24.0f, 64.0f, 5);
    test_terrain_generator_on(plateau_generator);
    
    StandardMultiTerrainGenerator* multi_generator = new StandardMultiTerrainGenerator();
    multi_generator->set_seed(100);
    multi_generator->set_multiparams(8.0f, 128.0f, 0.2f, 5);
    test_terrain_generator_on(multi_generator);
    
    MultiTerrainGenerator plateau_multi_generator = MultiTerrainGenerator();
    plateau_multi_generator.set_seed(100);
    plateau_multi_generator.add_generator(plateau_generator);
    plateau_multi_generator.add_generator(multi_generator);
    test_terrain_generator_on(&plateau_multi_generator);
}