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
#include "entity_loader.h"

SuperObject::SuperObject() {
    // this constructor should be only called to construct a world
    vid = 0;
    pid = 0;
    entity_class = 3;
    block_counter = 0;
}

SuperObject::SuperObject(uint32_t p, uint32_t v) {
    // this constructor should be called for world-less, cursor objects
    vid = v;
    pid = p;
    entity_class = 3;
    can_rotate = true;
    block_counter = 0;
}

// --- SuperObject ---
void SuperObject::add_entity(Entity* entity) {
    // transform entity rotation/pos into this object's frame
    //fvec3 oac;
    //transform_into_my_coordinates(&oac, entity->pos.x, entity->pos.y, entity->pos.z);
    //entity->pos = oac;
    // transform rotation into my frame
    //angle.transform_into_my_rotation(entity->angle, entity->angle);
    entities.push_back(entity);
}

void SuperObject::remove_entity(Entity* entity) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities.at(i) == entity) {
            entities.erase(entities.begin() + i);
            break;
        }
    }
    delete_entity_from_memory(entity);
}

std::string SuperObject::get_chunk_save_path(ivec3* pos) {
    return get_path_to_superobj_chunk(pid, vid, pos);
}

void SuperObject::copy_into(Player* player, SuperObject* target) {
    // copy entities over
    int entity_counter = 0;
    for (Entity* ent: entities) {
        // copy entity over, but if the entity is not a player
        Entity* copy = copy_entity(player, ent);
        if (copy) {
            target->add_entity(copy);
            entity_counter++;
        }
    }
    
    // copy blocks over
    int counter = 0;
    for (auto &i: chunk_bounds) {
        RenderableChunk* chunk = 0;
        if (chunks.count(i.first)) {
            chunk = chunks[i.first];
        }
        else {
            if (load_chunk(i.first.x, i.first.y, i.first.z)) {
                // chunk could not be loaded
                continue;
            }
            chunk = chunks[i.first];
        }
        for (int x = i.second.lower_bound.x; x <= i.second.upper_bound.x; x++) {
            for (int y = i.second.lower_bound.y; y <= i.second.upper_bound.y; y++) {
                for (int z = i.second.lower_bound.z; z <= i.second.upper_bound.z; z++) {
                    block_type block = chunk->blk[x][y][z];
                    if (block.type) {
                        counter++;
                        fvec3 world_coord;
                        ivec3 chunk_pos = i.first;
                        transform_into_world_coordinates(&world_coord,
                                                         chunk_pos.x*CX+x,
                                                         chunk_pos.y*CY+y,
                                                         chunk_pos.z*CZ+z);
                        ivec3 block_pos = ivec3(int(world_coord.x),
                                                int(world_coord.y),
                                                int(world_coord.z));
                        block.owner = player->getID();
                        target->set_block(block_pos.x, block_pos.y, block_pos.z, block);
                    }
                }
            }
        }
    }
    printf("Copied %d blocks and %d entities\n", counter, entity_counter);
}

// --- RenderableSuperObject
void SuperObject::handle_block_addition(float x, float y, float z, block_type type) {
    // set key bindings if it has one
    if (has_block_keyboard_bindings(type)) {
        std::vector<Action> bindings = get_block_default_keyboard_bindings(type);
        fvec3 oac;
        transform_into_my_coordinates(&oac, x, y, z);
        ivec3 rounded_oac = get_floor_from_fvec3(oac);
        reverse_key_mapping[rounded_oac] = std::vector<Action>();
        reverse_key_mapping[rounded_oac].reserve(bindings.size());
        for (int i = 0; i < bindings.size(); i++) {
            if (!key_mapping.count(bindings[i])) {
                key_mapping[bindings[i]] = std::vector<key_mapping_info>();
                key_mapping[bindings[i]].reserve(1);
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

// --- Entity ---
Entity* SuperObject::poke(float x, float y, float z) {
    if (RenderableSuperObject::poke(x, y, z))
        return this;
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities[i]->poke(x, y, z)) {
            return entities[i];
        }
    }
    return 0;
}

bool SuperObject::break_block(float x, float y, float z) {
    Entity* at = poke(x, y, z);
    if (at == this) {
        // remove block from this
        kill_block(x, y, z);
    }
    else if (at) {
        // break away the entity at from this object
        remove_entity(at);
        return true;
    }
    return false;
}

bool SuperObject::block_keyboard_callback(Game* game, Action key) {
    for (Entity* ent: entities) {
        ent->block_keyboard_callback(game, key);
    }
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
        return any;
    }
    return false;
}

bool SuperObject::block_mouse_callback(Game* game, Action button) {
    fvec4 lookingat;
    if (get_look_at_vehicle(&lookingat)) {
        Entity* at_cursor = poke(lookingat.x, lookingat.y, lookingat.z);
        if (at_cursor == this) {
            block_type blk = get_block(lookingat.x, lookingat.y, lookingat.z);
            block_mouse_callback_func callback = get_block_mouse_callback_from(blk.type);
            if (callback) {
                return (*callback)(game,
                                   this,
                                   &blk,
                                   fvec3(lookingat.x, lookingat.y, lookingat.z),
                                   button);
            }

        }
        else {
            return at_cursor->block_mouse_callback(game, button);
        }
    }
    return false;
}

void SuperObject::step() {
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->step();
    }
    if (stable) {
        float max_movement = 0.05f;
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
    }
}

void SuperObject::render(fmat4* transform) {
    RenderableSuperObject::render(transform);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render(transform);
    }
}

void SuperObject::update_chunks(fvec3* player_pos) {
    // TODO check if the bounds of this superobject even intersect our vision
    RenderableSuperObject::update_chunks(player_pos);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->update_chunks(player_pos);
    }
}

void SuperObject::calculate_moving_bounding_box() {
    RenderableSuperObject::calculate_moving_bounding_box();
    for (Entity* ent: entities) {
        ent->calculate_moving_bounding_box();
        moving_bounds.combine_with(ent->moving_bounds);
    }
}

int SuperObject::get_collision_level() {
    return 2;
}

// method for collision detection against base class entities ONLY
bool SuperObject::collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level) {
    if (my_collision_lvl == 0) {
        return Entity::collides_with(other, my_bounds, other_bounds, my_collision_lvl, other_collision_level);
    }
    else if (my_collision_lvl == 1) {
        return RenderableSuperObject::collides_with(other, my_bounds, other_bounds, my_collision_lvl, other_collision_level);
    }
    else {
        // first check if other collides with this superobject itself
        if (RenderableSuperObject::collides_with(other, my_bounds, other_bounds, my_collision_lvl - 1, other_collision_level)) {
            return true;
        }
        else {
            // check if other collides with any other entities i am holding
            for (Entity* ent: entities) {
                if (ent->collides_with(other)) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string SuperObject::get_save_path() {
    return get_path_to_superobj(pid, vid);
}

int SuperObject::load_self(IODataObject* obj) {
    if (RenderableSuperObject::load_self(obj))
        return 1;
    
    // load other entities here
    int entities_count = obj->read_value<int>();
    for (int i = 0; i < entities_count; i++) {
        // load in pid, vid, entity_class in that order
        uint32_t pid = obj->read_value<uint32_t>();
        uint32_t vid = obj->read_value<uint32_t>();
        int entity_class = obj->read_value<int>();
        Entity* entity = get_entity_from(pid, vid, entity_class);
        if (entity)
            entities.push_back(entity);
    }
    
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
    
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->remove_selfs();
    }
    int entities_count = (int)entities.size();
    // TODO handle that player/baseworld? is removed/saved here
    obj->save_value(entities_count);
    for (int i = 0; i < (int)entities.size(); i++) {
        if (entities[i]->entity_class == 2)
            continue;
        obj->save_value(entities[i]->pid);
        obj->save_value(entities[i]->vid);
        obj->save_value(entities[i]->entity_class);
    }
    
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