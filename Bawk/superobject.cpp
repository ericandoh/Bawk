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
        entity->parent->remove_entity(entity);
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

void SuperObject::get_entities_in_range(std::vector<Entity*> &children, bounding_box box, bool include_boundary) {
    if (include_boundary) {
        for (auto &i: entities) {
            if (box.hits(i->bounds)) {
                if (box.hits(i->bounds.lower) && box.hits(i->bounds.upper)) {
                    children.push_back(i);
                }
            }
        }
    }
    else {
        for (auto &i: entities) {
            if (box.hits(i->bounds)) {
                children.push_back(i);
            }
        }
    }
}

bool SuperObject::hits_chunk_bounds_in_range(bounding_box box) {
    // convert intersection_box into my coordinates
    bounding_box oac_box;
    transform_into_my_coordinates(&oac_box.lower,
                                  box.lower.x,
                                  box.lower.y,
                                  box.lower.z);
    transform_into_my_coordinates(&oac_box.upper,
                                  box.upper.x,
                                  box.upper.y,
                                  box.upper.z);
    oac_box.refit_for_rotation();
    
    // now transform into cac, crc
    ivec3 lower_cac, lower_crc;
    transform_into_chunk_bounds(&lower_cac, &lower_crc,
                                oac_box.lower.x,
                                oac_box.lower.y,
                                oac_box.lower.z);
    // now transform into cac, crc
    ivec3 upper_cac, upper_crc;
    transform_into_chunk_bounds(&upper_cac, &upper_crc,
                                ceilf(oac_box.upper.x),
                                ceilf(oac_box.upper.y),
                                ceilf(oac_box.upper.z));
    
    for (int x = lower_cac.x; x <= upper_cac.x; x++) {
        for (int y = lower_cac.y; y <= upper_cac.y; y++) {
            for (int z = lower_cac.z; z <= upper_cac.z; z++) {
                if (!within_dimensions_chunk(x, y, z)) {
                    continue;
                }
                ivec3 lower_corner(0, 0, 0);
                ivec3 upper_corner(CX, CY, CZ);
                if (x == lower_cac.x)
                    lower_corner.x = lower_crc.x;
                if (x == upper_cac.x)
                    upper_corner.x = upper_crc.x;
                if (y == lower_cac.y)
                    lower_corner.y = lower_crc.y;
                if (y == upper_cac.y)
                    upper_corner.y = upper_crc.y;
                if (z == lower_cac.z)
                    lower_corner.z = lower_crc.z;
                if (z == upper_cac.z)
                    upper_corner.z = upper_crc.z;
                ivec3 chunk_pos = ivec3(x, y, z);
                if (intersects_chunk(lower_corner, upper_corner, chunk_pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}

struct EntityPair {
    Entity* first;
    Entity* second;
    EntityPair(Entity* f, Entity* s) {
        first = f;
        second = s;
    }
    bool operator==(const EntityPair &other) const
    {
        return (first == other.first && second == other.second) ||
        (first == other.second && second == other.first);
    }
};

struct EntityPairHasher {
    std::size_t operator()(const EntityPair& k) const
    {
        using std::size_t;
        using std::hash;
        return hash<long>()((long)k.first) + hash<long>()((long)k.second);
    }
};

bool sort_on_x(Entity* a, Entity* b) {
    return a->moving_bounds.lower.x < b->moving_bounds.lower.x;
}
bool sort_on_y(Entity* a, Entity* b) {
    return a->moving_bounds.lower.y < b->moving_bounds.lower.y;
}
bool sort_on_z(Entity* a, Entity* b) {
    return a->moving_bounds.lower.z < b->moving_bounds.lower.z;
}

/*
 COLLISION DETECTION IS HERE!!!!!!!!!!
 
 Algorithm Summary: Sweep N' Prune, Then Iteratively Step
 
 Algorithm Steps:
 
 1. Calculate bounding box holding the entity before AND after it has moved <O(N)>
 2. Sort all entities by their lower corner of their movement bounding box (from above) <O(N log N)>, for each axis
 (3 lists total)
 3. For each axis list: <O(N)>
 a) See if the next element in my list intersects any opened boxes
 b) If so, add the colliding pair of elements to a hash
 i) If on the z-axis list we see that we've counted up to 3, this pair collides in all axises.
 ii) Find out which entity was the one that moved (the unstable one) and then add it to another mapping
 iii) The mapping should map from (unstable entity)=>(list of stable/unstable entities it collided with)
 c) If not, the opened box will not intersect any other future boxes, so close it (remove from open list)
 d) Add the box at our current index to the list of opened boxes (open the current box)
 4. Using mapping from (unstable entity)=>(list of all stable/unstable entities it collided with) O(M^2)
 a) Note that since pairs are unique, we will not have A=>(..B..) and B=>(..A..)
 b) Try rotating our unstable entity, and see if the rotation agrees with any of the stable/unstable entities
 i) If rotation works, apply it (prioritize rotations)
 ii) Else, do not apply the rotation
 c) Use the line tracing algorithm to step from start_pos to end_pos. At each integral position:
 i) Check against each of the stable/unstable entities it thinks it could have collided with
 ii) If collision, break and return that final position
 d) Get last valid position from the line tracing algorithm, set position of this unstable object to that
 i) Note: If we have a collision between 2 unstable objects this will make a priority order - one object
 will always move fully, then the rest will move until they can - this is slightly inaccurate but
 for practice will work well enough
 
 */
void SuperObject::check_collision_detection_children(Game* game) {
    // step on each entity, updating velocity/stable/etc if needed
    // step should also add the velocities to the position/angle/dir
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->calculate_moving_bounding_box();
    }
    // arrays of sorted lower bounds of each entity
    // an entity should be more precisely checked for collision if they intersect on all axises
    std::vector<Entity*> xbounding(entities);
    std::vector<Entity*> ybounding(entities);
    std::vector<Entity*> zbounding(entities);
    std::sort(xbounding.begin(), xbounding.end(), sort_on_x);
    std::sort(ybounding.begin(), ybounding.end(), sort_on_y);
    std::sort(zbounding.begin(), zbounding.end(), sort_on_z);
    
    // if the int value here ever gets to 3 we have a collision in all 3 axises = require closer look
    std::unordered_map<EntityPair,int,EntityPairHasher> collisionPairs;
    std::unordered_map<Entity*, std::vector<Entity*>> collisionList;
    
    for (auto& ent: entities) {
        // if not stable, always check entity against the world
        // if we add gravity, add it here later
        if (!ent->stable && ent->can_collide) {
            if (hits_chunk_bounds_in_range(ent->moving_bounds)) {
                collisionList[ent].push_back(this);
            }
        }
    }
    
    // now run collision detection - see if anything collides
    std::vector<Entity*> opened;
    for (int i = 0; i < xbounding.size(); i++) {
        if (xbounding[i]->bounds.lower.x == FLT_MAX) {
            continue;
        }
        for (int j = (int)opened.size() - 1; j >= 0; j--) {
            // if current's lower greater than opened's upper
            if (opened[j]->moving_bounds.upper.x <= xbounding[i]->moving_bounds.lower.x) {
                opened[j] = opened.back();
                opened.pop_back();
            }
            else {
                // report a (possible) collision
                EntityPair love_buddies(xbounding[i], opened[j]);
                collisionPairs[love_buddies] = 1;
            }
        }
        // add this item to the active list
        opened.push_back(xbounding[i]);
    }
    // now run collision detection - see if anything collides
    opened.clear();
    for (int i = 0; i < ybounding.size(); i++) {
        if (ybounding[i]->bounds.lower.y == FLT_MAX) {
            continue;
        }
        for (int j = (int)opened.size() - 1; j >= 0; j--) {
            // if current's lower greater than opened's upper
            if (opened[j]->moving_bounds.upper.y <= ybounding[i]->moving_bounds.lower.y) {
                opened[j] = opened.back();
                opened.pop_back();
            }
            else {
                // report a (possible) collision
                EntityPair love_buddies(ybounding[i], opened[j]);
                if (collisionPairs.count(love_buddies)) {
                    collisionPairs[love_buddies] += 1;
                }
            }
        }
        // add this item to the active list
        opened.push_back(ybounding[i]);
    }
    // now run collision detection - see if anything collides
    opened.clear();
    for (int i = 0; i < zbounding.size(); i++) {
        if (zbounding[i]->bounds.lower.z == FLT_MAX) {
            continue;
        }
        for (int j = (int)opened.size() - 1; j >= 0; j--) {
            // if current's lower greater than opened's upper
            if (opened[j]->moving_bounds.upper.z <= zbounding[i]->moving_bounds.lower.z) {
                opened[j] = opened.back();
                opened.pop_back();
            }
            else {
                // report a (possible) collision
                EntityPair love_buddies(zbounding[i], opened[j]);
                if (collisionPairs.count(love_buddies)) {
                    collisionPairs[love_buddies] += 1;
                    if (collisionPairs[love_buddies] == 3) {
                        // we have a collision
                        Entity* first = love_buddies.first;
                        Entity* second = love_buddies.second;
                        if (!first->can_collide || !second->can_collide) {
                            continue;
                        }
                        if (!first->stable) {
                            // first is the perpetrator of the action
                            if (collisionList.count(first) == 0) {
                                collisionList[first] = std::vector<Entity*>();
                            }
                            collisionList[first].push_back(second);
                        }
                        if (!second->stable) {
                            // second is the perpetrator of the action
                            if (collisionList.count(second) == 0) {
                                collisionList[second] = std::vector<Entity*>();
                            }
                            collisionList[second].push_back(first);
                        }
                    }
                }
            }
        }
        // add this item to the active list
        opened.push_back(zbounding[i]);
    }
    
    // if anything collided, revert back their velocities, then do step-by-step
    for (auto&i : collisionList) {
        Entity* moved_entity = i.first;
        std::vector<Entity*> checking_against = i.second;
        // first, try rotating moved_entity and see if it can rotate
        moved_entity->pos -= moved_entity->velocity;
        moved_entity->recalculate_transform();
        if (moved_entity->angular_velocity.x != 0 || moved_entity->angular_velocity.y != 0) {
            bool collides = false;
            for (auto &j : checking_against) {
                if (j->collides_with(moved_entity)) {
                    collides = true;
                    break;
                }
            }
            if (collides) {
                moved_entity->angle.reset_rotation();
                //moved_entity->angle -= moved_entity->angular_velocity;
                //moved_entity->recalculate_dir();
                moved_entity->recalculate_transform();
            }
        }
        if (moved_entity->velocity.x != 0 || moved_entity->velocity.y != 0 || moved_entity->velocity.z != 0) {
            // now try moving the entity slowly in the direction until final destination
            fvec3 start_pos = moved_entity->pos;
            fvec3 current_pos = start_pos;
            fvec3 entity_velocity = moved_entity->velocity;
            // step in increments of 1 first
            fvec3 step_velocity = glm::normalize(moved_entity->velocity);
            int steps = 0;
            if (step_velocity.x == 0) {
                if (step_velocity.y == 0) {
                    steps = (int)(entity_velocity.z / step_velocity.z);
                }
                else {
                    steps = (int)(entity_velocity.y / step_velocity.y);
                }
            } else {
                steps = (int)(entity_velocity.x / step_velocity.x);
            }
            // the last step will be against the end position
            steps += 1;
            int counter = 0;
            fvec3 prev_pos;
            bool collided = false;
            while (counter < steps && (!collided)) {
                prev_pos = current_pos;
                if (counter == steps - 1) {
                    current_pos = start_pos + entity_velocity;
                }
                else {
                    current_pos = current_pos + step_velocity;
                }
                moved_entity->set_pos(current_pos);
                for (auto &j: checking_against) {
                    if (j->collides_with(moved_entity)) {
                        collided = true;
                        break;
                    }
                }
                if (collided)
                    break;
                counter++;
            }
            if (!collided) {
                moved_entity->set_pos(start_pos + entity_velocity);
            }
            else {
                // precision step
                int precision_steps = 8;
                float precision_increment = 1.0f / precision_steps;
                step_velocity = step_velocity * precision_increment;
                current_pos = start_pos;
                for (int i = 0; i < precision_steps; i++) {
                    prev_pos = current_pos;
                    current_pos = current_pos + step_velocity;
                    for (auto &j: checking_against) {
                        if (j->collides_with(moved_entity)) {
                            collided = true;
                            break;
                        }
                    }
                    if (collided)
                        break;
                }
                moved_entity->set_pos(prev_pos);
                // TODO this is unsafe if our target object is the receiver on later collision detections..
                // and is removed
                // solution is easy - just put these objects into a queue (if they will remove themselves)
                if (moved_entity->after_collision(game)) {
                    // TODO add to some list saying "remove me!"
                    // note that this is mostly for projectiles that are like, totally blown up afterwards
                }
            }
        }
    }
    for (auto &i: entities) {
        if (!i->stable) {
            //i->velocity = fvec3(0,0,0);
            i->velocity = i->velocity * i->velocity_decay;
            if (fabsf(i->velocity.x) < 0.02f) {
                i->velocity.x = 0.0f;
            }
            if (fabsf(i->velocity.y) < 0.02f) {
                i->velocity.y = 0.0f;
            }
            if (fabsf(i->velocity.z) < 0.02f) {
                i->velocity.z = 0.0f;
            }
            if (i->velocity.x == 0 && i->velocity.y == 0 && i->velocity.z == 0) {
                i->stable = true;
            }
            i->angular_velocity = fvec3(0,0,0);
        }
    }
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
    weight += get_block_info(type.type)->weight;
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
    weight -= get_block_info(type.type)->weight;
    health -= type.life;
    block_counter--;
}

RenderableChunk* SuperObject::get_chunk(int x, int y, int z) {
    block_type to_arr[CX][CY][CZ];
    ivec3 pos = ivec3(x, y, z);
    IODataObject reader(get_chunk_save_path(&pos));
    if (reader.read(false))
        return 0;
    reader.read_pointer(&(to_arr[0][0][0]), sizeof(to_arr[0][0][0])*CX*CY*CZ);
    reader.close();
    
    RenderableChunk* chunk = new RenderableChunk(to_arr);
    return chunk;
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
        Entity* result = entities[i]->poke(x, y, z);
        if (result) {
            return result;
        }
    }
    return 0;
}

bool SuperObject::block_keyboard_callback(Game* game, Action key, Entity* ent, int ms) {
    bool any = false;
    for (Entity* entity: entities) {
        any = entity->block_keyboard_callback(game, key, ent, ms) || any;
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
    if (BlockTracing::show_item) {
        fvec3 lookingat = BlockTracing::pointed_pos;
        block_type* blk = get_block(lookingat.x, lookingat.y, lookingat.z);
        if (blk && has_block_mouse_action(*blk)) {
            ivec3 oac = get_floor_from_fvec3(fvec3(lookingat.x, lookingat.y, lookingat.z));
            ivec3 rwc;
            transform_into_world_integer_coordinates(&rwc, oac.x, oac.y, oac.z);
            return call_block_mouse_callback_from(blk, game, ent, rwc, button);
        }
    }
    // the else should not be called...
    return false;
}

void SuperObject::step(Game* game, int ms) {
    RenderableSuperObject::step(game, ms);
    bool check_collisions = false;
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->step(game, ms);
        check_collisions = check_collisions || (!entities[i]->stable);
    }
    if (check_collisions) {
        check_collision_detection_children(game);
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
    if (selected) {
        int frequency = 1 * convert_sec_to_milli(1);
        float intensity = (cosf((get_current_time() % frequency) * 2 * M_PI / frequency)) / 4.0f + 1.0f;
        OGLAttr::current_shader->set_shader_intensity(intensity);
    }
    
    RenderableSuperObject::render(transform);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render(transform);
    }
    
    if (selected) {
        OGLAttr::current_shader->set_shader_intensity(1.0f);
    }
}

void SuperObject::render_lights(fmat4* transform, fvec3 player_pos) {
    RenderableSuperObject::render_lights(transform, player_pos);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render_lights(transform, player_pos);
    }
}

void SuperObject::update_render(fvec3* player_pos) {
    // TODO check if the bounds of this superobject even intersect our vision
    RenderableSuperObject::update_render(player_pos);
    // TODO clean trash like here
    //fvec3 oac;
    //transform_into_my_coordinates(&oac, player_pos->x, player_pos->y, player_pos->z);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->update_render(player_pos);
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
            if (entity->load_self(obj)) {
                // this entity had an error while loading, don't add it
                continue;
            }
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

void SuperObject::save_self(IODataObject* obj) {
    RenderableSuperObject::save_self(obj);
    
    int entities_count = (int)entities.size();
    obj->save_value(entities_count);
    for (int i = 0; i < (int)entities.size(); i++) {
        if (entities[i]->entity_class == EntityType::PLAYER) {
            // don't save player as part of the world
            continue;
        }
        else {
            // but save all objects in the world, including those the player might
            // have left behind
            obj->save_value(entities[i]->pid);
            obj->save_value(entities[i]->vid);
            obj->save_value(entities[i]->entity_class);
            entities[i]->save_self(obj);
        }
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