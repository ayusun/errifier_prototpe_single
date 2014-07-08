#ifndef COMMON_H
#define COMMON_H

/**
 * A structure that is passed to the thread
 */
typedef struct watcherArgList {
    char filename[500];
    char friendlyname[50];
    int fd;
    int *toWatch;
}WatcherArgList;

/**
 * A structure that keeps track of all the threads
 */
typedef struct threadList {
    pthread_t thread[512];
    int threadcount;
    int mutexthread[512];
}ThreadList;

#endif // COMMON_H
