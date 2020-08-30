#define _GNU_SOURCE
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

static FILE *outf;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static bool inited = false;

static int (*real_pthread_create)(
		pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);
static int (*real_pthread_join)(pthread_t thread, void **retval);
static int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
static int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
static int (*real_pthread_cond_wait)(pthread_cond_t *cond, pthread_mutex_t *mutex);
static int (*real_pthread_cond_timedwait)(
		pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *time);
static int (*real_pthread_cond_broadcast)(pthread_cond_t *cond);
static int (*real_pthread_cond_signal)(pthread_cond_t *cond);

static void tlog2(const char *name, pthread_t self, pthread_t other) {
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	real_pthread_mutex_lock(&mut);
	fprintf(outf, "%lu:%lu %s %lu %lu\n", tv.tv_sec, tv.tv_nsec, name, self, other);
	real_pthread_mutex_unlock(&mut);
}

static void tlog1(const char *name, pthread_t self) {
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	real_pthread_mutex_lock(&mut);
	fprintf(outf, "%lu:%lu %s %lu\n", tv.tv_sec, tv.tv_nsec, name, self);
	real_pthread_mutex_unlock(&mut);
}

__attribute__((constructor (101)))
void threadviz_begin() {
	// Don't load for forks unless THREADVIZ_RECURSIVE=1
	if (getenv("THREADVIZ_RECURSIVE") == NULL || strcmp(getenv("THREADVIZ_RECURSIVE"), "1") != 0) {
		unsetenv("LD_PRELOAD");
	}

	if (mkdir("threadviz-data", 0777) < 0 && errno != EEXIST) {
		fprintf(stderr, "THREADVIZ: Create threadviz-data: %s\n", strerror(errno));
		abort();
	}

	char pathbuf[128];
	snprintf(pathbuf, sizeof(pathbuf), "threadviz-data/threadviz.%i.log", getpid());
	outf = fopen(pathbuf, "w");
	if (outf == NULL) {
		fprintf(stderr, "THREADVIZ: Open %s: %s\n", pathbuf, strerror(errno));
		abort();
	} else {
		fprintf(stderr, "THREADVIZ: Begin %s\n", pathbuf);
	}

	real_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
	real_pthread_join = dlsym(RTLD_NEXT, "pthread_join");
	real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	real_pthread_cond_wait = dlsym(RTLD_NEXT, "pthread_cond_wait");
	real_pthread_cond_timedwait = dlsym(RTLD_NEXT, "pthread_cond_timedwait");
	real_pthread_cond_broadcast = dlsym(RTLD_NEXT, "pthread_cond_broadcast");
	real_pthread_cond_signal = dlsym(RTLD_NEXT, "pthread_cond_signal");

	tlog1("THREAD_MAIN_BEGIN", pthread_self());
	inited = true;
}

__attribute__((destructor))
void threadviz_end() {
	if (!inited) {
		return;
	}

	tlog1("THREAD_MAIN_END", pthread_self());
	inited = false;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {
	int ret = real_pthread_create(thread, attr, start_routine, arg);
	tlog2("THREAD_START", pthread_self(), *thread);
	return ret;
}

int pthread_join(pthread_t thread, void **retval) {
	tlog2("THREAD_JOIN_BEGIN", pthread_self(), thread);
	int ret = real_pthread_join(thread, retval);
	tlog2("THREAD_JOIN_END", pthread_self(), thread);
	return ret;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	tlog1("MUTEX_LOCK_BEGIN", pthread_self());
	int ret = real_pthread_mutex_lock(mutex);
	tlog1("MUTEX_LOCK_END", pthread_self());
	return ret;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	return real_pthread_mutex_unlock(mutex);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	tlog1("COND_WAIT_BEGIN", pthread_self());
	int ret = real_pthread_cond_wait(cond, mutex);
	tlog1("COND_WAIT_END", pthread_self());
	return ret;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *time) {
	tlog1("COND_WAIT_BEGIN", pthread_self());
	int ret = real_pthread_cond_timedwait(cond, mutex, time);
	tlog1("COND_WAIT_END", pthread_self());
	return ret;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
	return real_pthread_cond_broadcast(cond);
}

int pthread_cond_signal(pthread_cond_t *cond) {
	return real_pthread_cond_signal(cond);
}
