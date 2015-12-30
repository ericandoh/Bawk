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
    entity_class = EntityType::SUPEROBJECT;
    block_counter = 0;
}

SuperObject::SuperObject(uint32_t p, uint32_t v) {
    // this constructor should be called for world-less, cursor objects
    vid = v;
    pid = p;
    entity_class = EntityType::SUPEROBJECT;
    can_rotate = true;
    block_counter = 0;
    velocity_decay = 0.9f;
}

// --- SuperObject ---
void SuperObject::add_entity(Entity* entity) {
    if (entity->parent) {
        ((SuperObject*)entity->parent)->remove_entity(entity);
    }
    // transform entity rotation/pos into this object's frame
    fvec3 true_center = entity->pos + entity->center_pos;
    fvec3 oac;
    transform_into_my_coordinates(&oac, true_center.x, true_center.y, true_center.z);
    entity->pos = oac - entity->center_pos;
    // transform rotation into my frame
    angle.transform_into_my_rotation(&(entity->angle), entity->angle);
    entity->parent = this;
    entity->recalculate_transform();
    entities.push_back(entity);
}

void SuperObject::remove_entity(Entity* entity) {
    bool removed = false;
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities.at(i) == entity) {
            entities.erase(entities.begin() + i);
            removed = true;
            break;
        }
    }
    if (!removed) {
        return;
    }
    entity->parent = 0;
    // transform entity back into RWC
    fvec3 oac_center = entity->pos + entity->center_pos;
    fvec3 rwc;
    transform_into_world_coordinates(&rwc, oac_center.x, oac_center.y, oac_center.z);
    entity->pos = rwc - entity->center_pos;
    angle.transform_into_world_rotation(&(entity->angle), entity->angle);
    entity->recalculate_transform();
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
            // transform entity rotation/pos into this object's frame
            fvec3 true_center = copy->pos + copy->center_pos;
            fvec3 rwc;
            transform_into_world_coordinates(&rwc, true_center.x, true_center.y, true_center.z);
            copy->pos = rwc - copy->center_pos;
            angle.transform_into_world_rotation(&(copy->angle), copy->angle);
            copy->recalculate_transform();
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
                        ivec3 world_coord;
                        ivec3 chunk_pos = i.first;
                        transform_into_world_integer_coordinates(&world_coord,
                                                                 (int)(chunk_pos.x*CX+x),
                                                                 (int)(chunk_pos.y*CY+y),
                                                                 (int)(chunk_pos.z*CZ+z));
                        // TODO change block orientation too?
                        printf("Setting %d %d %d\n", world_coord.x, world_coord.y, world_coord.z);
                        block.owner = player->getID();
                        target->set_block_integral(world_coord.x, world_coord.y, world_coord.z, block);
                    }
                }
            }
        }
    }
    printf("Copied %d blocks and %d entities\n", counter, entity_counter);
}

// --- RenderableSuperObject
void SuperObject::handle_block_addition(int x, int y, int z, block_type type) {
    // set key bindings if it has one
    if (has_block_keyboard_action(type)) {
        std::map<Action, block_callback_func> bindings;
        get_block_keyboard_callback_from(type, bindings);
        ivec3 rounded_oac(x, y, z);
        reverse_key_mapping[rounded_oac] = std::vector<Action>();
        reverse_key_mapping[rounded_oac].reserve(bindings.size());
        pos_to_active_block_mapping[rounded_oac] = type;
        for (auto &i: bindings) {
            Action act = i.first;
            
            if (!key_mapping.count(act)) {
                key_mapping[act] = std::vector<key_mapping_info>();
                key_mapping[act].reserve(1);
            }
            
            key_mapping_info info;
            info.position = rounded_oac;
            info.blk = type;
            info.func = i.second;
            key_mapping[act].push_back(info);
            reverse_key_mapping[rounded_oac].push_back(act);
        }
    }
    weight += get_block_weight(type.type);
    health += type.life;
    block_counter++;
}

void SuperObject::handle_block_removal(int x, int y, int z, block_type type) {
    ivec3 rounded_oac(x, y, z);
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
                }
            }
        }
        reverse_key_mapping.erase(rounded_oac);
        pos_to_active_block_mapping.erase(rounded_oac);
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
void SuperObject::recalculate_transform() {
    RenderableSuperObject::recalculate_transform();
    for (Entity* ent: entities) {
        ent->recalculate_transform();
    }
}

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
        delete_entity_from_memory(at);
        return true;
    }
    return false;
}

void SuperObject::get_hurt(float x, float y, float z, float dmg) {
    Entity* at = poke(x, y, z);
    if (at == this) {
        // hit block with the damage, if it is over threshold remove it
        block_type* blk = get_block(x, y, z);
        if (blk) {
            int resistance = get_block_resistance(blk->type);
            int actual_dmg = (int)((10.0f / (10.0f + resistance)) * dmg);
            blk->life += actual_dmg;
            if (blk->life >= MAX_HEALTH) {
                // TODO don't actually kill the block, unless the block belongs to the world
                // disable it instead
                if (!blk->owner)
                    kill_block(x, y, z);
            }
        }
    }
    else if (at) {
        at->get_hurt(x, y, z, dmg);
    }
}

bool SuperObject::block_keyboard_callback(Game* game, Action key, Entity* ent) {
    bool any = false;
    for (Entity* entity: entities) {
        any = entity->block_keyboard_callback(game, key, ent) || any;
    }
    if (key_mapping.count(key)) {
        for (int i = 0; i < key_mapping[key].size(); i++) {
            ivec3 position = key_mapping[key][i].position;
            // transform into rwc
            ivec3 rwc;
            transform_into_world_integer_coordinates(&rwc, position.x, position.y, position.z);
            
            block_callback_func callback = key_mapping[key][i].func;
            if (callback) {
                any = any || (*callback)(game,
                                         ent,
                                         &(key_mapping[key][i].blk),
                                         rwc);
            }
        }
        return any;
    }
    return any;
}

bool SuperObject::block_mouse_callback(Game* game, Action button, Entity* ent) {
    fvec4 lookingat;
    if (get_look_at_vehicle(&lookingat)) {
        Entity* at_cursor = poke(lookingat.x, lookingat.y, lookingat.z);
        if (at_cursor == this) {
            block_type* blk = get_block(lookingat.x, lookingat.y, lookingat.z);
            if (blk && has_block_mouse_action(*blk)) {
                ivec3 oac = get_floor_from_fvec3(fvec3(lookingat.x, lookingat.y, lookingat.z));
                ivec3 rwc;
                transform_into_world_integer_coordinates(&rwc, oac.x, oac.y, oac.z);
                return call_block_mouse_callback_from(blk, game, ent, rwc, button);
            }
        }
        else {
            return at_cursor->block_mouse_callback(game, button, ent);
        }
    }
    return false;
}

void SuperObject::step(Game* game) {
    RenderableSuperObject::step(game);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->step(game);
    }
    /*if (stable) {
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
    }*/
}

void SuperObject::render(fmat4* transform) {
    RenderableSuperObject::render(transform);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render(transform);
    }
}

void SuperObject::render_lights(fmat4* transform, fvec3 player_pos) {
    RenderableSuperObject::render_lights(transform, player_pos);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render_lights(transform, player_pos);
    }
}

void SuperObject::update_chunks(fvec3* player_pos) {
    // TODO check if the bounds of this superobject even intersect our vision
    RenderableSuperObject::update_chunks(player_pos);
    // TODO clean trash like here
    //fvec3 oac;
    //transform_into_my_coordinates(&oac, player_pos->x, player_pos->y, player_pos->z);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->update_chunks(player_pos);
    }
}

bool SuperObject::collides_with(Entity* other) {
    if (Entity::collides_with(other)) {
        return true;
    }
    // else, go through each of my entities
    for (Entity* ent: entities) {
        if (ent->collides_with(other)) {
            return true;
        }
    }
    return false;
}

void SuperObject::calculate_moving_bounding_box() {
    RenderableSuperObject::calculate_moving_bounding_box();
    for (Entity* ent: entities) {
        ent->calculate_moving_bounding_box();
        bounding_box ent_box = ent->moving_bounds;
        //transform_into_world_coordinates(&ent_box.lower, ent_box.lower.x, ent_box.lower.y, ent_box.lower.z);
        //transform_into_world_coordinates(&ent_box.upper, ent_box.upper.x, ent_box.upper.y, ent_box.upper.z);
        //ent_box.refit_for_rotation();
        moving_bounds.combine_with(ent_box);
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
        EntityType entity_class = obj->read_value<EntityType>();
        Entity* entity = get_entity_from(pid, vid, entity_class);
        if (entity) {
            entity->parent = this;
            entity->recalculate_transform();
            entities.push_back(entity);
        }
    }
    
    int block_binding_count = obj->read_value<int>();
    for (int i = 0; i < block_binding_count; i++) {
        ivec3 pos = obj->read_value<ivec3>();
        block_type blk = obj->read_value<block_type>();
        pos_to_active_block_mapping[pos] = blk;
        
        std::map<Action, block_callback_func> bindings;
        get_block_keyboard_callback_from(blk, bindings);
        reverse_key_mapping[pos] = std::vector<Action>();
        reverse_key_mapping[pos].reserve(bindings.size());
        for (auto &i: bindings) {
            Action act = i.first;
            
            if (!key_mapping.count(act)) {
                key_mapping[act] = std::vector<key_mapping_info>();
                key_mapping[act].reserve(1);
            }
            
            key_mapping_info info;
            info.position = pos;
            info.blk = blk;
            info.func = i.second;
            key_mapping[act].push_back(info);
            reverse_key_mapping[pos].push_back(act);
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
        if (entities[i]->entity_class == EntityType::PLAYER)
            continue;
        obj->save_value(entities[i]->pid);
        obj->save_value(entities[i]->vid);
        obj->save_value(entities[i]->entity_class);
    }
    
    // save only the reverse key mapping
    int block_binding_count = (int)reverse_key_mapping.size();
    obj->save_value(block_binding_count);
    for (auto &i: reverse_key_mapping) {
        ivec3 pos = i.first;
        obj->save_value(pos);
        obj->save_value(pos_to_active_block_mapping[pos]);
    }
    
    obj->save_value(block_counter);
}