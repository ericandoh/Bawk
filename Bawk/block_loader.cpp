//
//  block_loader.cpp
//  Bawk
//https://buddypanda.com/?p=10
//
//  Created by Eric Oh on 10/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "block_loader.h"


const std::string BLOCK_DATA_PATH = "/Users/Eric/w_data";
const std::string WORLD_SUBDIRECTORY_PATH = "world";

// ----- ROOT SAVE PATH -----
std::string get_root_save_path() {
    // if windows, return something different
    return "/Users/Eric/Bawk";
}

// ----- MAIN SUBDIRECTORY SAVE PATHS -----
std::string get_world_save_path() {
    return get_root_save_path() + "/worlds";
}

std::string get_superobject_save_path() {
    return get_root_save_path() + "/superobjects";
}

std::string get_player_save_path() {
    return get_root_save_path() + "/players";
}

std::string get_template_save_path() {
    return get_root_save_path() + "/templates";
}

std::string get_game_save_path() {
    return get_root_save_path() + "/info";
}

// ----- BLOCK DATA SAVE PATH -----
std::string get_chunk_path(std::string prefix_path,
                           ivec3* chunk_pos) {
    // name the chunk data path by the absolutes of the chunk position
    // and by the positive/negative of the position coordinates
    
    // pos for positive, not position!
    unsigned int xposi = chunk_pos->x > 0;
    unsigned int yposi = chunk_pos->y > 0;
    unsigned int zposi = chunk_pos->z > 0;
    
    unsigned int pos_bits = (xposi << 0x2) + (yposi << 0x1) + zposi;
    
    // for now put all chunk data in same object folder
    // later separate different x/y/z to different folders,
    // or hash locally (using same hash funct as for the maps)!
    
    return prefix_path + "/bl" + std::to_string(pos_bits) + "_"
                       + std::to_string(abs(chunk_pos->x)) + "_"
                       + std::to_string(abs(chunk_pos->y)) + "_"
                       + std::to_string(abs(chunk_pos->z));
}

// ----- ID OBJECT SAVE PATH -----
std::string get_id_path(std::string prefix_path, uint32_t id) {
    return prefix_path + "/x" + std::to_string(id);
}

// ----- IDxID OBJECT SAVE PATH -----
std::string get_idid_path(std::string prefix_path, uint32_t id0, uint32_t id1) {
    return prefix_path + "/x" + std::to_string(id0) + "_"
                       + std::to_string(id1);
}

// ----- NAMED DATA SAVE PATH -----
std::string get_named_object_path(std::string base_path, std::string name) {
    return base_path + "/" + name;
}

// ----- METADATA SAVE PATH -----
std::string get_metadata_path(std::string base_path) {
    return base_path + "/meta.inf";
}

// ----- VARIABLE METADATA SAVE PATH -----
std::string get_variable_metadata_path(std::string base_path, std::string name) {
    return base_path + "/vmeta.inf";
}

// ----- GET FILE LENGTH -----
long get_file_length_c(std::string file_name) {
    // this is copied from shader_loader.cpp
    std::ifstream t(file_name, std::ifstream::ate | std::ifstream::binary);
    return t.tellg();
}



/*
// ----- GENERIC WRITE METHOD -----
// the below is UNSAFE and should be only called INTERNALLY!!!
int save_data_to_file(std::string path, char* data, int length) {
    std::ofstream out;
    out.open(path,
             std::ios::out | std::ios::trunc | std::ios::binary);
    
    if (!out.is_open()) {
        return 1;
    }
    
    // TOFU use any encoding function here
    out.write(data, sizeof(char)*length);
    // TOFU check for errors (by looking at error flags)
    out.close();
    return 0;
}

// ----- GENERIC READ METHOD -----
// the below is UNSAFE and should be only called INTERNALLY!!!
int read_data_from_file(std::string path, char** data, int length) {
    std::ifstream in;
    in.open(path,
            std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        return -1;
    }
    
    // TOFU use any decoding function here
    //in.seekg(0, std::ios::beg);
    long file_length = get_file_length_c(path);
    if (*data == 0 || file_length > length) {
        printf("Data longer than expected. (%ld vs %d) Bit memory inefficient but w/e\n", file_length, length);
        if (*data)
            delete *data;
        *data = new char[file_length];
    }
    in.read(*data, file_length);
    in.close();
    return 0;
}*/

// ----- DIRECTORY PATH CHECK METHOD -----
// forces creation of a directory. this fails if dir already exists
// TOFU make this less hacky cmon man
int validate_subdirectory_path(std::string path) {
    std::string command = "mkdir " + path;
    std::system(command.c_str());   // this might fail...TOFU handle
    return 0;
}

// makes any directories not in the path
int validate_read_write_path(std::string path) {
    std::string root = get_root_save_path();
    int delims_in_root = 0;
    size_t pos = 0;
    while ((pos = root.find("/", pos)) != std::string::npos) {
        delims_in_root++;
        pos++;
    }
    
    pos = 0;
    int delim_counter = 0;
    while ((pos = path.find("/", pos)) != std::string::npos) {
        delim_counter++;
        if (delim_counter > delims_in_root) {
            std::string subdir = path.substr(0, pos);
            if (validate_subdirectory_path(subdir)) {
                // we had a nonzero error
                return 1;
            }
        }
        pos++;
    }
    return 0;
}

// deletes object at path. CAREFUL
void delete_at_path(std::string path) {
    std::string command = "rm -rf " + path;
    std::system(command.c_str());   // this might fail...TOFU handle
    return;
}

std::string get_path_to_world_folder(std::string world_name) {
    return get_named_object_path(get_world_save_path(), world_name);
}

// read/write methods
std::string get_path_to_world(std::string world_name) {
    return get_metadata_path(get_path_to_world_folder(world_name));
}

std::string get_path_to_world_chunk(std::string world_name, ivec3* chunk_pos) {
    return get_chunk_path(
                               get_path_to_world_folder(world_name),
                               chunk_pos
                          );
}

std::string get_path_to_superobj_folder(uint32_t pid, uint32_t vid) {
    return get_idid_path(get_superobject_save_path(), pid, vid);
}

std::string get_path_to_superobj(uint32_t pid, uint32_t vid) {
    return get_metadata_path(get_path_to_superobj_folder(pid, vid));
}

std::string get_path_to_superobj_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos) {
    return get_chunk_path(
                               get_path_to_superobj_folder(pid, vid),
                               chunk_pos
                          );
}

std::string get_path_to_template_folder(uint32_t pid, uint32_t vid) {
    return get_idid_path(get_template_save_path(), pid, vid);
}

std::string get_path_to_template(uint32_t pid, uint32_t vid) {
    return get_metadata_path(get_path_to_template_folder(pid, vid));
}

std::string get_path_to_template_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos) {
    return get_chunk_path(
                               get_path_to_template_folder(pid, vid),
                               chunk_pos
                               );
}

std::string get_path_to_player_folder(uint32_t pid) {
    return get_id_path(get_player_save_path(), pid);
}

std::string get_path_to_player(uint32_t pid) {
    return get_metadata_path(get_path_to_player_folder(pid));
}

std::string get_path_to_game_folder() {
    return get_game_save_path();
}

std::string get_path_to_game() {
    return get_metadata_path(get_path_to_game_folder());
}

std::string get_path_to_game_json() {
    // the game info json should be located inside where the game is running
    return "./gameinfo.json";
}




IODataObject::IODataObject(std::string p) {
    i = 0;
    mode = SaveMode::READ;
    succeeded = false;
    read_data = 0;
    path = p;
}

IODataObject::~IODataObject() {
    if (read_data)
        delete read_data;
}

void IODataObject::close() {
    if (mode == SaveMode::READ) {
        delete read_data;
        read_data = 0;
    }
    else if (mode == SaveMode::WRITE) {
        size_t written = outfile.tellp();
        if (written > 1000000) {
            printf("We wrote more than 1MB to disk. WHY?\n");
        }
        outfile.close();
    }
}

int IODataObject::save(bool needs_validation) {
    if (needs_validation)
        validate_read_write_path(path);
    outfile.open(path,
                 std::ios::out | std::ios::trunc | std::ios::binary);
    
    if (!outfile.is_open()) {
        return 1;
    }
    i = 0;
    mode = SaveMode::WRITE;
    return 0;
}

int IODataObject::read(bool needs_validation) {
    if (needs_validation)
        validate_read_write_path(path);
    
    size = get_file_length_c(path);
    if (size <= 0)
        return 1;
    read_data = new char[size];
    
    infile.open(path,
                std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        return 1;
    }
    infile.read(read_data, size);
    infile.close();
    i = 0;
    mode = SaveMode::READ;
    
    return 0;
}


