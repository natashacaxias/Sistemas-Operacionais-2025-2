#include "tempo.h"

#ifdef _WIN32
// ---------------- WINDOWS -----------------
#include <windows.h>

double tempo_atual() {
    static LARGE_INTEGER freq;
    static int initialized = 0;

    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return (double) now.QuadPart / freq.QuadPart;
}

#else
// ---------------- LINUX / UNIX -----------------
#define _POSIX_C_SOURCE 200112L
#include <time.h>

double tempo_atual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}
#endif
