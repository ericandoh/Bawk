//
//  obj_data_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "obj_data_loader.h"
#include <vector>

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <sstream>

#include "modeldata.h"
#include "block_loader.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    // stolen shamelessly (i'm a good thief) from http://stackoverflow.com/questions/236129/split-a-string-in-c
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void read_obj_file(ModelData* dst, std::string obj_filename, Textures::TextureName tex) {
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
            
            dst->model_vertices.push_back(temp_vertices[vertexIndex[0] - 1]);
            dst->model_vertices.push_back(temp_vertices[vertexIndex[1] - 1]);
            dst->model_vertices.push_back(temp_vertices[vertexIndex[2] - 1]);
            
            dst->model_uvs.push_back(temp_uvs[uvIndex[0] - 1]);
            dst->model_uvs.push_back(temp_uvs[uvIndex[1] - 1]);
            dst->model_uvs.push_back(temp_uvs[uvIndex[2] - 1]);
            
            dst->model_normals.push_back(temp_normals[normalIndex[0] - 1]);
            dst->model_normals.push_back(temp_normals[normalIndex[1] - 1]);
            dst->model_normals.push_back(temp_normals[normalIndex[2] - 1]);
        }
    }
    file.close();
    if (tex == Textures::TextureName::TOTAL_TEXTURES) {
        tex = Textures::TextureName::TILES;
    }
    dst->texture = tex;
}
