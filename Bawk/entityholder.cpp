//
//  entityholder.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entityholder.h"
#include "entity_loader.h"
#include "bresenham3d.h"

void EntityHolder::set_global_entity(Entity* entity) {
    global_entity = entity;
}

void EntityHolder::add_entity(Entity* entity) {
    entities.push_back(entity);
}

void EntityHolder::remove_entity(Entity* entity) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities.at(i) == entity) {
            entities.erase(entities.begin() + i);
            break;
        }
    }
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
    return a->moving_lower.x < b->moving_lower.x;
}
bool sort_on_y(Entity* a, Entity* b) {
    return a->moving_lower.y < b->moving_lower.y;
}
bool sort_on_z(Entity* a, Entity* b) {
    return a->moving_lower.z < b->moving_lower.z;
}


class EntityPathTracer: public BresenhamTracer {
    Entity* candidate;
    std::vector<Entity*> against;
public:
    bool success;
    fvec3 final_pos;
    EntityPathTracer(Entity* c, std::vector<Entity*> &a, fvec3 init);
    bool check_at_coord(float x, float y, float z, BlockOrientation side) override;
};

EntityPathTracer::EntityPathTracer(Entity* c, std::vector<Entity*> &a, fvec3 init) {
    success = true;
    candidate = c;
    against = a;
    final_pos = init;
}

bool EntityPathTracer::check_at_coord(float x, float y, float z, BlockOrientation side) {
    candidate->pos = fvec3(floorf(x),floorf(y),floorf(z));
    for (auto &i: against) {
        if (i->collides_with(candidate)) {
            success = false;
            return true;
        }
    }
    final_pos = candidate->pos;
    return false;
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
void EntityHolder::step() {
    // step on each entity, updating velocity/stable/etc if needed
    // step should also add the velocities to the position/angle/dir
    global_entity->step();
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->step();
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
        if (!ent->stable) {
            collisionList[ent] = std::vector<Entity*>(1);
            collisionList[ent][0] = global_entity;
        }
    }
    
    // now run collision detection - see if anything collides
    std::vector<Entity*> opened;
    for (int i = 0; i < xbounding.size(); i++) {
        for (int j = (int)opened.size() - 1; j >= 0; j--) {
            // if current's lower greater than opened's upper
            if (opened[j]->moving_upper.x <= xbounding[i]->moving_lower.x) {
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
        for (int j = (int)opened.size() - 1; j >= 0; j--) {
            // if current's lower greater than opened's upper
            if (opened[j]->moving_upper.y <= ybounding[i]->moving_lower.y) {
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
        for (int j = (int)opened.size() - 1; j >= 0; j--) {
            // if current's lower greater than opened's upper
            if (opened[j]->moving_upper.z <= zbounding[i]->moving_lower.z) {
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
                        if (!first->stable) {
                            // first is the perpetrator of the action
                            if (collisionList.count(first) == 0) {
                                collisionList[first] = std::vector<Entity*>();
                            }
                            collisionList[first].push_back(second);
                        }
                        else {
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
        moved_entity->revert_velocities();
        moved_entity->apply_rotation();
        bool collides = false;
        for (auto &j : checking_against) {
            if (j->collides_with(moved_entity)) {
                collides = true;
                break;
            }
        }
        if (collides) {
            moved_entity->revert_velocities();
        }
        // now try moving the entity, 1 block step at a time, until it gets to its destination
        fvec3 start_pos = moved_entity->pos;
        fvec3 end_pos = moved_entity->pos + moved_entity->velocity;
        EntityPathTracer path_tracer(moved_entity, checking_against, start_pos);
        path_tracer.bresenham3D(start_pos.x, start_pos.y, start_pos.z,
                                end_pos.x, end_pos.y, end_pos.z);
        if (path_tracer.success) {
            moved_entity->pos = start_pos;
            moved_entity->apply_velocity();
        }
        else {
            moved_entity->pos = path_tracer.final_pos;
        }
    }
    for (auto &i: entities) {
        i->reset_velocities();
    }
}

void EntityHolder::render(fmat4* transform) {
    global_entity->render(transform);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render(transform);
    }
}

void EntityHolder::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    global_entity->update_chunks(old_pos, new_pos);
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->update_chunks(old_pos, new_pos);
    }
}

bool EntityHolder::collides_with(Entity* entity) {
    if (global_entity->collides_with(entity))
        return true;
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities[i] != entity && entities[i]->collides_with(entity)) {
            return true;
        }
    }
    return false;
}

Entity* EntityHolder::poke(float x, float y, float z) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities[i]->poke(x, y, z)) {
            return entities[i];
        }
    }
    return 0;
}

int EntityHolder::load_self(IODataObject* obj) {
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
    return 0;
}

void EntityHolder::remove_self(IODataObject* obj) {
    
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->remove_selfs();
    }
    global_entity->remove_selfs();
    
    int entities_count = (int)entities.size();
    for (int i = 0; i < entities_count; i++) {
        // remove from entity count all entities that are baseworld or player
        // because we don't want to save those!
        if (entities[i]->entity_class == 1 || entities[i]->entity_class == 2) {
            entities_count--;
        }
    }
    
    obj->save_value(entities_count);
    for (int i = 0; i < (int)entities.size(); i++) {
        if (entities[i]->entity_class == 1 || entities[i]->entity_class == 2)
            continue;
        obj->save_value(entities[i]->pid);
        obj->save_value(entities[i]->vid);
        obj->save_value(entities[i]->entity_class);
    }
}

void test_print_entities_positions(std::vector<Entity*> &entities) {
    printf("new run\n");
    for (int i = 1; i < entities.size(); i++) {
        printf("%d at (%f,%f,%f)\n",
               i,
               (float)entities[i]->pos.x,
               (float)entities[i]->pos.y,
               (float)entities[i]->pos.z);
    }
}

void test_entity_holder_collision_detection() {
    int testcount = 2;
    testcount++;    // for global
    std::vector<Entity*> all_entities(testcount);
    for (int i = 0; i < testcount; i++) {
        all_entities[i] = new Entity();
        all_entities[i]->stable = true;
    }
    int x = 0;
    // global entity - make it a point!
    all_entities[x]->pos = fvec3(0,0,0);
    all_entities[x]->lower_bound = fvec3(0,0,0);
    all_entities[x++]->upper_bound = fvec3(0,0,0);
    
    
    all_entities[x]->pos = fvec3(1,0,1);
    all_entities[x]->center_pos = fvec3(0.5f,0.5f,0.5f);
    all_entities[x]->lower_bound = fvec3(0,0,0);
    all_entities[x++]->upper_bound = fvec3(1,1,1);
    
    all_entities[x]->pos = fvec3(3,0,2);
    all_entities[x]->center_pos = fvec3(0.5f,0.5f,0.5f);
    all_entities[x]->lower_bound = fvec3(0,0,0);
    all_entities[x++]->upper_bound = fvec3(1,1,1);
    
    EntityHolder holder;
    holder.set_global_entity(all_entities[0]);
    for (int i = 1; i < testcount; i++) {
        holder.add_entity(all_entities[i]);
    }
    
    // RUN TESTS
    // move <1> from (1,1) to (2,1)
    all_entities[1]->velocity = fvec3(1,0,0);
    all_entities[1]->stable = false;
    holder.step();
    test_print_entities_positions(all_entities);
    // move <1> from (2,1) to (2,2)
    all_entities[1]->velocity = fvec3(0,0,1);
    all_entities[1]->stable = false;
    holder.step();
    test_print_entities_positions(all_entities);
    // FAIL: move <2> from (3,2) to (2,2)
    all_entities[2]->velocity = fvec3(-1,0,0);
    all_entities[2]->stable = false;
    holder.step();
    test_print_entities_positions(all_entities);
    // move <1> from (2,2) to (-5,2)
    all_entities[1]->velocity = fvec3(-7,0,0);
    all_entities[1]->stable = false;
    holder.step();
    test_print_entities_positions(all_entities);
    // FAIL: OVERSHOOT move <1> from (-5,2) to (10,2)
    all_entities[1]->velocity = fvec3(15,0,0);
    all_entities[1]->stable = false;
    holder.step();
    test_print_entities_positions(all_entities);
    // CLEANUP
    for (int i = 0; i < testcount; i++) {
        delete all_entities[i];
    }
}