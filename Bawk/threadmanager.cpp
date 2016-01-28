//
//  threadmanager.cpp
//  Bawk
//
//  Created by Eric Oh on 1/28/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "threadmanager.h"
#include <map>
#include <queue>
#include "importsdl.h"

struct ThreadJobAndThread {
    ThreadJob* job;
    SDL_Thread* thread;
};

class ThreadManager {
    std::map<int, ThreadJobAndThread> jobs;
    std::map<int, std::queue<ThreadJob*>> waiting_jobs;
public:
    // initializes threader
    void init_threads();
    void add_thread_job(ThreadJob* job);
    
    void step();
    
    // makes sure all threads are closed
    void close_threads();
};

namespace Threading {
    ThreadManager manager;
}

using namespace Threading;

void ThreadManager::init_threads() {
    jobs = std::map<int, ThreadJobAndThread>();
    waiting_jobs = std::map<int, std::queue<ThreadJob*>>();

    //jobs.clear();
    //waiting_jobs.clear();
}

int run_job(void* ptr) {
    ThreadJob* job = (ThreadJob*) ptr;
    job->run();
    return 0;
}

void ThreadManager::add_thread_job(ThreadJob* job) {
    if (jobs.count(job->thread_id)) {
        // a thread of this instance is already running
        
        // tell the current job to cancel itself, a new job will replace it
        jobs[job->thread_id].job->canceled();
        
        if (!waiting_jobs.count(job->thread_id)) {
            waiting_jobs[job->thread_id] = std::queue<ThreadJob*>();
        }
        waiting_jobs[job->thread_id].push(job);
        return;
    }
    
    // tell job to init itself
    job->init();
    
    SDL_Thread* thread;
    thread = SDL_CreateThread(run_job, job->get_job_name(), (void*)job);
    if (thread == 0) {
        printf("Thread creation failed\n");
        // run the job in main thread
        job->run();
    }
    ThreadJobAndThread combo;
    combo.job = job;
    combo.thread = thread;
    jobs[job->thread_id] = combo;
}

void ThreadManager::step() {
    std::vector<int> remove_ids;
    for (auto &i: jobs) {
        ThreadJob* job = i.second.job;
        job->flush();
        if (job->is_finished()) {
            int status;
            SDL_WaitThread(i.second.thread, &status);
            if (status) {
                // error
                printf("Thread finished with error\n");
            }
            remove_ids.push_back(i.first);
        }
    }
    
    for (auto &thread_id: remove_ids) {
        // remove this job from the queue
        ThreadJob* delete_job = jobs[thread_id].job;
        jobs.erase(thread_id);
        delete delete_job;
        // see if any jobs can replace this job
        if (waiting_jobs.count(thread_id)) {
            ThreadJob* job = waiting_jobs[thread_id].front();
            waiting_jobs[thread_id].pop();
            if (!waiting_jobs[thread_id].size()) {
                waiting_jobs.erase(thread_id);
            }
            add_thread_job(job);
        }
    }
}

// makes sure all threads are closed
void ThreadManager::close_threads() {
    // tell all jobs to cancel themselves
    // wait for all jobs to close
    for (auto &i: jobs) {
        ThreadJob* job = i.second.job;
        job->canceled();
        int status;
        SDL_WaitThread(i.second.thread, &status);
        if (status) {
            // error
            printf("Thread finished with error\n");
        }
    }
    
    jobs.clear();
    waiting_jobs.clear();
}


void init_threader() {
    manager = ThreadManager();
    manager.init_threads();
}

void add_thread_job(ThreadJob* job) {
    manager.add_thread_job(job);
}

void step_threader(){
    manager.step();
}

void close_all_threads() {
    manager.close_threads();
}