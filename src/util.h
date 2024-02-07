#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MODEL_PATH "/proc/device-tree/model"
#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"

typedef struct mem_info
{
    char MemTotal[20];
    char MemFree[20];
    unsigned int total;
    unsigned int free;
}mem_info_t;

typedef struct cpu_occupy_          
{
    char name[20];                  
    unsigned int user;              
    unsigned int nice;              
    unsigned int system;            
    unsigned int idle;              
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
}cpu_occupy_t;

int get_pi_version(void);

#endif