//
//  worldevent.cpp
//  Bawk
//
//  Created by Eric Oh on 10/31/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "worldevent.h"
#include "world.h"
#include "game_info_loader.h"
#include "cursorsuperobject.h"

WorldEvent::WorldEvent() {
    eid = 0;
    expires = true;
    expiration = 0;
}

WorldEvent::WorldEvent(long duration) {
    eid = 0;
    expires = true;
    expiration = duration;
}

class DebugEvent: public WorldEvent {
public:
    DebugEvent(long duration);
    void action(World* world) override;
};
DebugEvent::DebugEvent(long duration): WorldEvent(duration) {
    eid = 1;
}
void DebugEvent::action(World* world) {
    printf("Event fired\n");
}

class SpawnEvent: public WorldEvent {
    uint16_t recipe_id;
    fvec3 position;
public:
    SpawnEvent(long duration, uint16_t rid, fvec3 pos);
    void action(World* world) override;
};
SpawnEvent::SpawnEvent(long duration, uint16_t rid, fvec3 pos): WorldEvent(duration) {
    eid = 2;
    recipe_id = rid;
    position = pos;
}
void SpawnEvent::action(World* world) {
    // spawn recipe with
    CursorSuperObject* item = (CursorSuperObject*)get_recipe_cursoritem_from(recipe_id);
    // set position
    item->pos = position;
    item->set_blocks(0, world, 0);
    item->cleanup();
}

void publish_world_event(World* world, WorldEvent* event) {
    event->expiration += world->age;
    world->add_event(event);
}

void make_debug_event(World* world, long duration) {
    publish_world_event(world, new DebugEvent(duration));
}

void make_spawn_event(World* world, uint16_t recipe, fvec3 pos) {
    publish_world_event(world, new SpawnEvent(0, recipe, pos));
}

void make_spawn_ore_event(World* world, uint16_t type, fvec3 pos, int radius) {
    return;
}

void free_world_event(WorldEvent* event);
void free_world_event(WorldEvent* event) {
    if (event->eid == 1) {
        delete (DebugEvent*) event;
    }
    else if (event->eid == 2) {
        delete (SpawnEvent*) event;
    }
    else {
        printf("An EID for a event is undefined: %d\n", event->eid);
        printf("Could not free memory\n");
    }
}