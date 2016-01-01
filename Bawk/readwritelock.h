//
//  readwritelock.h
//  Bawk
//
//  Created by Eric Oh on 12/31/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__readwritelock__
#define __Bawk__readwritelock__

#include <stdio.h>

struct SDL_mutex;

class ReadWriteLock {
    SDL_mutex* global;
    SDL_mutex* readers;
    // counter of readers
    int counter;
    int errors;
    int owner;
public:
    void init();
    
    void start_read();
    void end_read();
    
    void start_write();
    void end_write();
    
    void start_safe(int owner=0);
    void end_safe(int owner=0);
};

#endif /* defined(__Bawk__readwritelock__) */
