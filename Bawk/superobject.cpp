//
//  Superobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "superobject.h"
#include "block_loader.h"
#include "game_info_loader.h"
#include "game.h"
#include "blocktracer.h"

SuperObject::SuperObject() {
    // this constructor should be only called to construct a world
    vid = 0;
    pid = 0;
    entity_class = 4;
    block_counter = 0;
}

SuperObject::SuperObject(uint32_t p, uint32_t v) {
    // this constructor should be called for world-less, cursor objects
    vid = v;
    pid = p;
    entity_class = 4;
    can_rotate = true;
    block_counter = 0;
}

void SuperObject::handle_block_addition(float x, float y, float z, block_type type) {
    // set key bindings if it has one
    if (has_block_keyboard_bindings(type)) {
        std::vector<Action> bindings = get_block_default_keyboard_bindings(type);
        fvec3 oac;
        transform_into_my_coordinates(&oac, x, y, z);
        ivec3 rounded_oac = get_floor_from_fvec3(oac);
        reverse_key_mapping[rounded_oac] = std::vector<Action>(bindings.size());
        for (int i = 0; i < bindings.size(); i++) {
            if (!key_mapping.count(bindings[i])) {
                key_mapping[bindings[i]] = std::vector<key_mapping_info>(1);
            }
            key_mapping_info info;
            info.position = rounded_oac;
            info.blk = type;
            info.action = bindings[i];
            key_mapping[bindings[i]].push_back(info);
            reverse_key_mapping[rounded_oac].push_back(bindings[i]);
        }
    }
    weight += get_block_weight(type.type);
    health += type.life;
    block_counter++;
}

void SuperObject::handle_block_removal(float x, float y, float z, block_type type) {
    fvec3 oac;
    transform_into_my_coordinates(&oac, x, y, z);
    ivec3 rounded_oac = get_floor_from_fvec3(oac);
    // see if we're removing a block that has a key binding
    if (reverse_key_mapping.count(rounded_oac)) {
        // we do have such a key binding, remove it
        for (int i = 0; i < reverse_key_mapping[rounded_oac].size(); i++) {
            Action keybinding = reverse_key_mapping[rounded_oac][i];
            for (int j = (int)key_mapping[keybinding].size() - 1; j >= 0; j--) {
                if (key_mapping[keybinding][j].position == rounded_oac) {
                    // assume we only mapped one key to
                    key_mapping[keybinding][j] = key_mapping[keybinding].back();
                    key_mapping[keybinding].pop_back();
                    break;
                }
            }
        }
        reverse_key_mapping.erase(rounded_oac);
    }
    // remove stats for current block
    // center pos is set outside separately
    weight -= get_block_weight(type.type);
    health -= type.life;
    block_counter--;
}

void SuperObject::step() {
    if (stable) {
        float max_movement = 0.05f;
        float max_angle_movement = 0.05f;
        if (floorf(pos.x) != pos.x) {
            float off = roundf(pos.x) - pos.x;
            if (off < -max_movement)
                off = -max_movement;
            if (off > max_movement)
                off = max_movement;
            velocity.x += off;
            stable = false;
        }
        if (floorf(pos.y) != pos.y) {
            float off = roundf(pos.y) - pos.y;
            if (off < -max_movement)
                off = -max_movement;
            if (off > max_movement)
                off = max_movement;
            velocity.y += off;
            stable = false;
        }
        if (floorf(pos.z) != pos.z) {
            float off = roundf(pos.z) - pos.z;
            if (off < -max_movement)
                off = -max_movement;
            if (off > max_movement)
                off = max_movement;
            velocity.z += off;
            stable = false;
        }
        angle.inch_toward_normalization();
        /*
        if ((int)(angle.angle.x/HALF_PI)*HALF_PI != angle.angle.x) {
            float off = roundf(angle.angle.x/HALF_PI)*HALF_PI - angle.angle.x;
            if (off < -max_angle_movement)
                off = -max_angle_movement;
            else if (off > max_angle_movement)
                off = max_angle_movement;
            else
                off = 0;
            angular_velocity.x += off;
        }
        if ((int)(angle.angle.y/HALF_PI)*HALF_PI != angle.angle.y) {
            float off = roundf(angle.angle.y/HALF_PI)*HALF_PI - angle.angle.y;
            if (off < -max_angle_movement)
                off = -max_angle_movement;
            else if (off > max_angle_movement)
                off = max_angle_movement;
            else
                off = 0;
            angular_velocity.y += off;
        }
        if ((int)(angle.angle.z/HALF_PI)*HALF_PI != angle.angle.z) {
            float off = roundf(angle.angle.z/HALF_PI)*HALF_PI - angle.angle.z;
            if (off < -max_angle_movement)
                off = -max_angle_movement;
            else if (off > max_angle_movement)
                off = max_angle_movement;
            else
                off = 0;
            angular_velocity.z += off;
        }*/
    }
}

bool SuperObject::block_keyboard_callback(Game* game, Action key) {
    if (key_mapping.count(key)) {
        bool any = false;
        for (int i = 0; i < key_mapping[key].size(); i++) {
            ivec3 position = key_mapping[key][i].position;
            fvec3 rwc;
            transform_into_world_coordinates(&rwc, position.x, position.y, position.z);
            block_keyboard_callback_func callback = get_block_keyboard_callback_from(key_mapping[key][i].blk.type);
            if (callback) {
                any = any || (*callback)(game,
                                         this,
                                         &(key_mapping[key][i].blk),
                                         rwc,
                                         key_mapping[key][i].action);
            }
        }
        // assume if its in our key mapping, even if the presentable action does NOT exist (due to bug)
        // that we consume the key
        return any;
    }
    return false;
}

bool SuperObject::block_mouse_callback(Game* game, int button) {
    fvec4 lookingat;
    if (get_look_at_vehicle(&lookingat)) {
        block_type blk = get_block(lookingat.x, lookingat.y, lookingat.z);
        // see if blk is an indirect block - that is, it's part of a recipe block
        if (blk.is_recipe == 2) {
            // find offset
            printf("frog\n");
            printf("ufkc off");
            // this does NOT account for the rotation of the entity and WILL give us a wrong result!!!
            //ivec3 rrel = get_translated_offset(blk.orientation, ivec3(blk.relx, blk.rely, blk.relz));
            //blk = get_block(lookingat.x + rrel.x, lookingat.y + rrel.y, lookingat.z + rrel.z);
        }
        block_mouse_callback_func callback = get_block_mouse_callback_from(blk.type);
        if (callback) {
            (*callback)(game,
                        this,
                        &blk,
                        fvec3(lookingat.x, lookingat.y, lookingat.z),
                        button);
            return true;
        }
    }
    return false;
}

std::string SuperObject::get_save_path() {
    return get_path_to_superobj(pid, vid);
}

std::string SuperObject::get_chunk_save_path(ivec3* pos) {
    return get_path_to_superobj_chunk(pid, vid, pos);
}

int SuperObject::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    IODataObject reader(get_chunk_save_path(&pos));
    if (reader.read(false))
        return 1;
    reader.read_pointer(&(to_arr[0][0][0]), sizeof(to_arr[0][0][0])*CX*CY*CZ);
    reader.close();
    return 0;
}

int SuperObject::save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    IODataObject writer(get_chunk_save_path(&pos));
    if (writer.save(false))
        return 1;
    writer.save_pointer(&(from_arr[0][0][0]), sizeof(from_arr[0][0][0])*CX*CY*CZ);
    writer.close();
    return 0;
}

int SuperObject::load_self(IODataObject* obj) {
    if (RenderableSuperObject::load_self(obj))
        return 1;
    
    // load key bindings!
    int keybinding_count = obj->read_value<int>();
    for (int i = 0; i < keybinding_count; i++) {
        Action key = obj->read_value<Action>();
        int keybinded_count = obj->read_value<int>();
        key_mapping[key] = std::vector<key_mapping_info>(keybinded_count);
        for (int j = 0; j < keybinded_count; j++) {
            key_mapping[key].push_back(obj->read_value<key_mapping_info>());
            // construct the reverse_key_mapping as well
            if (!reverse_key_mapping.count(key_mapping[key][j].position)) {
                reverse_key_mapping[key_mapping[key][j].position] = std::vector<Action>();
            }
            reverse_key_mapping[key_mapping[key][j].position].push_back(key);
        }
    }
    block_counter = obj->read_value<int>();
    
    return 0;
}

void SuperObject::remove_self(IODataObject* obj) {
    RenderableSuperObject::remove_self(obj);
    
    // save key bindings!
    // TOFU later make a SAVE_MAP and SAVE_VECTOR method in the block_loader, so we don't have to keep doing this hack
    int keybinding_count = (int)key_mapping.size();
    obj->save_value(keybinding_count);
    for (auto &i : key_mapping) {
        obj->save_value(i.first);   // save the key press
        int keybinded_count = (int)i.second.size();
        obj->save_value(keybinded_count);
        for (int j = 0; j < keybinded_count; j++) {
            obj->save_value(i.second[j]);
        }
    }
    obj->save_value(block_counter);
}