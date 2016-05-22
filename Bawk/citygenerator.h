//
//  citygenerator.h
//  Bawk
//
//  Created by Eric Oh on 2/20/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__citygenerator__
#define __Bawk__citygenerator__

#include <stdio.h>
#include <string>
#include "basic_types.h"

class SectorGenerationInfo;
class BiomeGenerationInfo;

class CityGenerator {
    uint16_t city_id;
    std::string name;
    
    float avg_city_block_length;
    float city_radius;
    
    CityGenerator();
    virtual void add_city(SectorGenerationInfo* sector_info,
                          BiomeGenerationInfo* biome_info);
};

#endif /* defined(__Bawk__citygenerator__) */
