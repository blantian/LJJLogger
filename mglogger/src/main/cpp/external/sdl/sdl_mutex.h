

#ifndef AV_MGLOGGER_SDL__MUTEX_H
#define AV_MGLOGGER_SDL__MUTEX_H

#include <stdint.h>
#include <pthread.h>

#define SDL_MUTEX_TIMEDOUT  1
#define SDL_MUTEX_MAXWAIT   (~(uint32_t)0)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef struct SDL_mutex {
    pthread_mutex_t id;
} SDL_mutex;

SDL_mutex  *SDL_CreateMutex(void);
void        SDL_DestroyMutex(SDL_mutex *mutex);
void        SDL_DestroyMutexP(SDL_mutex **mutex);
int         SDL_LockMutex(SDL_mutex *mutex);
int         SDL_UnlockMutex(SDL_mutex *mutex);

typedef struct SDL_cond {
    pthread_cond_t id;
} SDL_cond;

SDL_cond   *SDL_CreateCond(void);
void        SDL_DestroyCond(SDL_cond *cond);
void        SDL_DestroyCondP(SDL_cond **mutex);
int         SDL_CondSignal(SDL_cond *cond);
int         SDL_CondBroadcast(SDL_cond *cond);
int         SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, uint32_t ms);
int         SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

