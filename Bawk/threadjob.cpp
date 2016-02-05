//
//  threadjob.cpp
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "threadjob.h"

ThreadJob::ThreadJob(int tid) {
    am_finished = false;
    am_closed = false;
    thread_id = tid;
    
    am_finished_lock = SDL_CreateMutex();
    printf("Making new thread job %d\n", thread_id);
    //SDL_DestroyMutex(console_mutex);
}

void ThreadJob::mark_finished() {
    SDL_LockMutex(am_finished_lock);
    am_finished = true;
    SDL_UnlockMutex(am_finished_lock);
    am_closed = true;
}

void ThreadJob::canceled() {
    printf("Canceling thread job %d\n", thread_id);
    mark_finished();
}

bool ThreadJob::is_finished() {
    if (am_closed)
        return true;
    SDL_LockMutex(am_finished_lock);
    bool result = am_finished;
    SDL_UnlockMutex(am_finished_lock);
    if (result) {
        SDL_DestroyMutex(am_finished_lock);
    }
    return result;
}

const char* ThreadJob::get_job_name() {
    return job_name.c_str();
}