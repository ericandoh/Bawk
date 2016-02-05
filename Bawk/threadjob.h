//
//  threadjob.h
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__threadjob__
#define __Bawk__threadjob__

#include <stdio.h>
#include <string>
#include "basic_types.h"
#include "importsdl.h"

class ThreadJob {
    bool am_finished;
    bool am_closed;
    SDL_mutex* am_finished_lock;
protected:
    std::string job_name;
    void mark_finished();
public:
    int thread_id;
    
    ThreadJob(int tid);
    
    // run at the start of the thread job, in the main thread
    virtual void init() EMPTY_FUNCTION;
    // run asynchronously
    virtual void run() EMPTY_FUNCTION;
    // run every so often, in the main thread. also guaranteed to be called after run() has finished
    virtual void flush() EMPTY_FUNCTION;
    // when this is called, this thread is canceled. make appropriate calls to close this thread
    virtual void canceled();
    
    bool is_finished();
    const char* get_job_name();
};

#endif /* defined(__Bawk__threadjob__) */
