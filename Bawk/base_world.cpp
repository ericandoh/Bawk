//
//  base_world.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "base_world.h"

#define GROUND_DEPTH 4

BaseWorld::BaseWorld() {
    memset(air, 0, sizeof(air[0][0][0])*CX*CY*CZ);
    memset(ground, 0, sizeof(ground[0][0][0])*CX*CY*CZ);
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            for (int y = 0; y < GROUND_DEPTH; y++) {
                ground[x][y][z] = 2;
            }
        }
    }
}

int BaseWorld::get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) {
    if (y == 0) {
        printf("Loading ground\n");
        memcpy(&to_arr[0][0][0], &ground[0][0][0], sizeof(uint16_t)*CX*CY*CZ);
        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 10 <=1)
                    to_arr[x][GROUND_DEPTH][z] = rand() % 4 + 2;
            }
        }
    }
    else {
        printf("Loading air\n");
        memcpy(&to_arr[0][0][0], &air[0][0][0], sizeof(uint16_t)*CX*CY*CZ);
        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 100 <=1)
                    to_arr[x][rand() % 16][z] = rand() % 4 + 7;
            }
        }
    }
    return 0;
}

void BaseWorld::save_chunk(int x, int y, int z) {
    // this world doesn't save (yet)!
    // shit
}

bool BaseWorld::within_dimensions(int x, int y, int z) {
    // all the world is within dimensions technically
    return true;
}