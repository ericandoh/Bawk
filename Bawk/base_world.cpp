//
//  base_world.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "base_world.h"
#include "block_loader.h"
#include "world_generator.h"
#include <vector>
#include <unordered_map>

#define GROUND_DEPTH 4

BaseWorld::BaseWorld() {
    pid = 0;
    vid = 0;
    entity_class = 1;
    get_empty_chunk(air);
    get_empty_chunk(ground);
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            for (int y = 0; y < GROUND_DEPTH; y++) {
                ground[x][y][z].type = 2;
            }
        }
    }
}

// --- SuperObject ---
int BaseWorld::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    if (!SuperObject::get_chunk(to_arr, x, y, z)) {
        return 0;
    }
    fill_chunk_at(ivec3(x, y, z), to_arr);
    return 0;
}

bool BaseWorld::within_dimensions_chunk(int x, int y, int z) {
    // all the world is within dimensions technically
    return true;
}

void BaseWorld::update_dimensions_from_chunk(ivec3 chunk_pos) {
    // do nothing
}

bool BaseWorld::intersects_chunk(ivec3 lower, ivec3 upper, ivec3 chunkpos) {
    if (load_chunk(chunkpos.x, chunkpos.y, chunkpos.z) ) {
        return false;
    }
    return chunks[chunkpos]->intersects_my_bounds(lower, upper);
}

// --- Entity ---
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
void BaseWorld::step() {
    SuperObject::step();
    
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
            collisionList[ent] = std::vector<Entity*>(1);
            collisionList[ent][0] = this;
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
            }
        }
        if (moved_entity->velocity.x != 0 || moved_entity->velocity.y != 0 || moved_entity->velocity.z != 0) {
            // now try moving the entity slowly in the direction until final destination
            fvec3 start_pos = moved_entity->pos;
            fvec3 current_pos = start_pos;
            fvec3 normalized_velocity = glm::normalize(moved_entity->velocity);
            fvec3 step_velocity = normalized_velocity * 0.1f;
            int steps = 0;
            if (step_velocity.x == 0) {
                if (step_velocity.y == 0) {
                    steps = (int)(moved_entity->velocity.z / step_velocity.z);
                }
                else {
                    steps = (int)(moved_entity->velocity.y / step_velocity.y);
                }
            } else {
                steps = (int)(moved_entity->velocity.x / step_velocity.x);
            }
            if (checking_against.size() == 2) {
                printf("frog %d %d\n", steps, rand());
                if (steps < 0) {
                    printf("what\n");
                }
            }
            if (steps <= 0) {
                steps = 1;
            }
            int counter = 0;
            fvec3 prev_pos;
            bool collided = false;
            while (counter < steps && (!collided)) {
                prev_pos = current_pos;
                current_pos = current_pos + step_velocity;
                moved_entity->pos = current_pos;
                for (auto &i: checking_against) {
                    if (i->collides_with(moved_entity)) {
                        collided = true;
                        break;
                    }
                }
                if (collided)
                    break;
                counter++;
            }
            if (!collided) {
                moved_entity->pos = start_pos + moved_entity->velocity;
            }
            else {
                moved_entity->pos = prev_pos;
            }
        }
    }
    for (auto &i: entities) {
        i->stable = true;
        i->velocity = fvec3(0,0,0);
        i->angular_velocity = fvec3(0,0,0);
    }
}

bool BaseWorld::collides_with(Entity* other) {
    if (!can_collide || !other->can_collide) {
        return false;
    }
    
    bounding_box other_rwc_bounds;
    other->transform_into_world_coordinates(&other_rwc_bounds.lower, other->bounds.lower.x, other->bounds.lower.y, other->bounds.lower.z);
    other->transform_into_world_coordinates(&other_rwc_bounds.upper, other->bounds.upper.x, other->bounds.upper.y, other->bounds.upper.z);
    other_rwc_bounds.refit_for_rotation();
    
    return SuperObject::collides_with(other, &other_rwc_bounds, &other_rwc_bounds, get_collision_level(), other->get_collision_level());
}

