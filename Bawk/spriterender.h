//
//  spriterender.h
//  Bawk
//
//  Created by Eric Oh on 12/8/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__spriterender__
#define __Bawk__spriterender__

#include <stdio.h>
//#include "positionable.h"
#include "entity.h"
#include "lightrender.h"

class Player;
class SpriteRender;

/*
 A bit of design philosophy
 
 - There is a SpriteRender for every instance of a sprite
 - The spriterender has pointers to objects that describe what it can do
 - If the object does NOT need to contain state, there is one globally allocated version of that class
   and every SpriteRender that uses that function will point to that object
 - If the object does HAVE state, but that state can be shared between multiple versions of the sprite 
   (for example, a sprite rendering a certain texture can have several instances), we have one instance 
   of the sprite behaviour variable and all sprite instances point to that
 - If the object DOES need state (for example, age), then we DO allocate some memory in the heap and then 
   access it via that. And this object is pointed to ONLY by our spriterenderable instance. We might set a 
   variable asking if it needs to be freed or not
 - Sprites are defined in the game json with the appropriate params, loaded in, and then linked with the 
   appropriate block actions (which denote which sprite it wants to use as its own param, in turn)
   (ex. "action": ["id": "explode", "sprite": "bomb1"]...."sprites": {"1": {"id": "bomb1", "type": "explode", "texture": "xxx"}}...
 - We copy this model for block actions as well (that's awesome!)
 
 
 
 Design Process for adding a new sprite:
 
 - Create a new class that overrides either SpriteSteppable, SpriteRenderable, etc...some action element of sprite
 - Define behaviour in that class
 - If you want a new copy of that action because it carries state, implement/override the copy function
 - Create a single function to get that sprite with parameters. Call this function A.
 - Go into spritegetter.cpp and define how to read the JSON info, and then call function A with that info to get our sprite. 
 - Write the sprite in the JSON, and maybe the behaviours it uses
 - The rest will take care of itself - the game info loader will keep an instance of the sprite, and if we need to duplicate 
   the sprite, the behaviour objects will be respectively copied/created and linked
 
 - Advantages: If we have multiple types of exploding sprites that all use different textures but behave the same way, 
               we can recycle the method (behaviour) and simply write in the JSON different params for how to define 
               that same state
 
 - Once again, as stated above, copy this design paradigm for block actions (very useful!)
 - Multiplex in the base BlockAction class, define + copy behaviours for different engines of different strength/etc
 */

class Game;

class SpriteSteppable {
public:
    virtual void step(Game* game, SpriteRender* render);
    // don't override if you don't want new instance every time
    virtual SpriteSteppable* copy();
};

class SpriteRenderable {
public:
    virtual void render(fmat4* transform, SpriteRender* render);
    // don't override if you don't want new instance every time
    virtual SpriteRenderable* copy();
};

// defined for subclasses when fetching
namespace Json {
    class Value;
}

class SpriteRender: public Entity {
public:
    Player* player;
    SpriteSteppable* steppable;
    SpriteRenderable* renderable;
    // time to live, set to -1 if this will keep living
    unsigned int ttl;
    // this is constant, how long the sprite lives in total (doesnt count down)
    unsigned int duration;
    RenderableLight light;
    
    SpriteRender();
    
    // --- SpriteRender ---
    // some helper functions for common operations usually done on sprite, that are stateless
    float get_perc_age();
    void set_relative_transform();
    void render_rectangle(fmat4* transform, float width, float height, fvec3 texture_corners[4]);
    SpriteRender* copy();
    
    // --- Entity ---
    void step(Game* game, int ms) override; //override;
    void render(fmat4* transform) override;
    void render_lights(fmat4* transform, fvec3 player_pos) override;

};

SpriteRender* get_sprite_from_id(std::string name);

#endif /* defined(__Bawk__spriterender__) */
