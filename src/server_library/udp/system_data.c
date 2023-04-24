//
// Created by Kirill Zhukov on 24.04.2023.
//
#include "system_data.h"

int get_num_cpus() {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif defined(__APPLE__)
    int ncpu;
    size_t size = sizeof(ncpu);
    sysctlbyname("hw.logicalcpu", &ncpu, &size, NULL, 0);
    return ncpu;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}