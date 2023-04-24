//
// Created by Kirill Zhukov on 24.04.2023.
//

#ifndef UDP_SYSTEM_DATA_H
#define UDP_SYSTEM_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

int get_num_cpus();

#ifdef __cplusplus
}
#endif

#endif //UDP_SYSTEM_DATA_H
