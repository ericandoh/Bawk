//
//  chunkresourcemanager.cpp
//  Bawk
//
//  Created by Eric Oh on 11/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "chunkresourcemanager.h"


/*
 Algorithm Summary
 
 --- Partly Inspired by Jerry-Daycare (of Rick and Morty) ---
 
 Think of each chunk as a Jerry. We drop off a Jerry. If it gets lost, we'll just make a call and reload it no problem capiche. 
 Give each Jerry a placecard, if a Jerry exists too long and we need space we'll just toss it (and later if its needed some 
 Rick will come in with an invalid placecard and we'll just remake it for him no problem)
 
 Some Jerries we should absolutely never evict. Mark those with -dne (do not evict). Examples would be 
 chunks that we are rendering at a location.
 
 Some Jerries we'll use for a moment then we wont need it again. For example, if we're calculating new bounds of a superobject, 
 we'll need to bring in all chunks and then redo calculations. In that case, set the chunk to be allwoed to be freed after it is 
 used
 
 Jerries by default when loaded are marked with the -dne flag. unmark them if we don't need them necessarily (but maybe memory caching 
 will make it nice???)
 
 note that owners should be classified by PID,VID so if we have duplicate objects of the same PID, VID (like cursorobjects) they 
 will be pointing to the same chunk info if it is loaded in. this provides some sort of caching which is nice.  
 
 */

// register a new chunk owner (whether it be the baseworld, some superobject, etc)
int register_chunk_owner(std::string save_path) { return 0; }

// gets me a new placecard slot for which we can evict
int get_placecard_slot() { return 0; }

// creates a new chunk entirely. This is a new Jerry being dropped off for some nice daycaring!
RenderableChunk* create_new_chunk(block_type raw_chunk[CX][CY][CZ]) { return 0; }

// an owner hands his owner ID and the placecard he has.
// we go look at the Jerry we have at the placecard indicated. If his Jerry is NOT at that placecard we'll make
// him a new one, load it from memory
RenderableChunk* get_chunk_from_placecard(int owner, int placecard) { return 0; }

// some method to get all chunks loaded in for an owner, and call render on them, or some shizzle
ChunkOwnerInformation* get_all_chunks(int owner) { return 0; }

// create_new_chunk: creates a new chunk given data. a placecard is generated