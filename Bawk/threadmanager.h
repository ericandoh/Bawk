//
//  threadmanager.h
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__threadmanager__
#define __Bawk__threadmanager__

#include <stdio.h>
#include <vector>
#include "threadjob.h"

void init_threader();
void add_thread_job(ThreadJob* job);
void step_threader();
void close_all_threads();

#endif /* defined(__Bawk__threadmanager__) */
