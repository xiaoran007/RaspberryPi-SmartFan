#ifndef UTIL_H
#define UTIL_H

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "ssd1306_i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/vfs.h>
#include <unistd.h>


#define MODEL_PATH "/proc/device-tree/model"
#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_LEN 32

typedef struct{
    char MemTotal[20];
    char MemFree[20];
    unsigned int total;
    unsigned int free;
}mem_info;

typedef struct{
    char name[20];                  
    unsigned int user;              
    unsigned int nice;              
    unsigned int system;            
    unsigned int idle;              
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
}cpu_occupy;

int get_pi_version(void);
char* get_ip_info(void);

#endif