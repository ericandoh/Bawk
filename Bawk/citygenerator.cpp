//
//  citygenerator.cpp
//  Bawk
//
//  Created by Eric Oh on 2/20/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "citygenerator.h"
#include "sectorgenerator.h"
#include "biomegenerator.h"

CityGenerator::CityGenerator() {
    city_id = 0;
    name = "";
    avg_city_block_length = 10.0f;
    city_radius = 100.0f;
}

void CityGenerator::add_city(SectorGenerationInfo* sector_info,
                             BiomeGenerationInfo* biome_info) {
    
    ivec3 start_point = biome_info->position;
    
    
    
}