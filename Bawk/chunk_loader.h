//
//  Reads in chunk data, compressing or uncompressing as appropriate
//  -
//  -
//  -
//
//  Used by:
//  - superobject.cpp for chunk loading/unloading
//  - cursorsuperobject.cpp for loading from custom path
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  - implementing run length encoding to speed up memory reads
//  -
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__chunk_loader__
#define __Bawk__chunk_loader__

#include <stdio.h>
#include "block_loader.h"

class RenderableChunk;

RenderableChunk* read_chunk_data(IODataObject* obj);
void save_chunk_data(RenderableChunk* chunk, IODataObject* obj);

#endif /* defined(__Bawk__chunk_loader__) */
