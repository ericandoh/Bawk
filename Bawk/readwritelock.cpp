//
//  readwritelock.cpp
//  Bawk
//
//  learned from https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock
//  thanks wikipedia you're great ill donate soon i promise
//
//  Created by Eric Oh on 12/31/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "readwritelock.h"
#include "importsdl.h"

void ReadWriteLock::init() {
    global = SDL_CreateMutex();
    readers = SDL_CreateMutex();
    counter = 0;
    errors = 0;
    owner = -1;
}
    
void ReadWriteLock::start_read() {
    printf("starting read\n");
    errors += SDL_LockMutex(readers);
    counter++;
    if (counter == 1) {
        errors += SDL_LockMutex(global);
    }
    errors += SDL_UnlockMutex(readers);
}

void ReadWriteLock::end_read() {
    printf("ending read\n");
    errors += SDL_LockMutex(readers);
    counter--;
    if (counter == 0) {
        errors += SDL_UnlockMutex(global);
    }
    errors += SDL_UnlockMutex(readers);
}
    
void ReadWriteLock::start_write() {
    printf("starting write\n");
    errors += SDL_LockMutex(global);
}

void ReadWriteLock::end_write() {
    printf("ending write\n");
    errors += SDL_UnlockMutex(global);
}

void ReadWriteLock::start_safe(int owner) {
    printf("starting safe\n");
    errors += SDL_LockMutex(readers);
    if (this->owner != owner) {
        errors += SDL_LockMutex(global);
        this->owner = owner;
    }
    errors += SDL_UnlockMutex(readers);
}

void ReadWriteLock::end_safe(int owner) {
    printf("ending safe\n");
    errors += SDL_LockMutex(readers);
    errors += SDL_UnlockMutex(global);
    this->owner = -1;
    errors += SDL_UnlockMutex(readers);
}