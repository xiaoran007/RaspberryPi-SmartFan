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

#define HARDWARE_PATH "/proc/device-tree/model"
#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE 32
#define Max_LED 3

typedef struct mem_info{
    char MemTotal[20];
    char MemFree[20];
    unsigned int total;
    unsigned int free;

}mem_info_t;

typedef struct cpu_occupy_{
    char name[20];                  
    unsigned int user;              
    unsigned int nice;              
    unsigned int system;            
    unsigned int idle;              
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
}cpu_occupy_t;

void setRGB(int num, int R, int G, int B, int fd_i2c);
void closeRGB(int fd_i2c);
mem_info_t getRAM(void);
double cal_cpuoccupy (cpu_occupy_t *o, cpu_occupy_t *n);
void get_cpuoccupy (cpu_occupy_t *cpust);
double get_sysCpuUsage(void);


int main(void){
	int readed_ip = 0;
	int count = 0;
	int fd_temp;
	int fd_hardware;            
	int raspi_version = 0;      
	double temp = 0, fan_state = 2;
	char buf[MAX_SIZE];

	struct sysinfo sys_info;
	struct statfs disk_info;

	struct ifaddrs *ifAddrStruct = NULL;
	void *tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

	int fd_i2c;
	wiringPiSetup();
	fd_i2c = wiringPiI2CSetup(0x0d);
	while (fd_i2c < 0){
		fd_i2c = wiringPiI2CSetup(0x0d);
		fprintf(stderr, "fail to init I2C\n");
		delay(500);
	}

    // enable RGB
	// wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x03);

	// wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);   // close fan
	// ssd1306_display();      //show logo
	// ssd1306_clearDisplay();
	// delay(500);
	printf("init ok!\n");

	while (1){
		if (sysinfo(&sys_info) != 0){
			printf("sysinfo-Error\n");
			ssd1306_clearDisplay();
			char *text = "sysinfo-Error";
			ssd1306_drawString(text);
			ssd1306_display();
			continue;
		}
		else{
			ssd1306_clearDisplay();

			char CPUInfo[MAX_SIZE] = {0};
			unsigned long avgCpuLoad = sys_info.loads[0] / 1000;
			sprintf(CPUInfo, "CPU: %.1lf%%", get_sysCpuUsage());

			char RamInfo[MAX_SIZE];
			// unsigned long totalRam = sys_info.totalram >> 20;
			// unsigned long freeRam = sys_info.freeram >> 20;
			// sprintf(RamInfo, "RAM:%ld/%ld MB", freeRam, totalRam);
			mem_info_t mem_info;
			mem_info = getRAM();
			// int usedlRam = (mem_info.total-mem_info.free) / 1024;
			int usedlRam = mem_info.free / 1024;
			int totalRam = mem_info.total / 1024;
			sprintf(RamInfo, "RAM: %d / %d", usedlRam, totalRam);

			char IPInfo[MAX_SIZE];
			if (readed_ip == 0){
				while (ifAddrStruct != NULL){
					if (ifAddrStruct->ifa_addr->sa_family == AF_INET){ // check it is IP4 is a valid IP4 Address
						tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
						char addressBuffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

						if (strcmp(ifAddrStruct->ifa_name, "eth0") == 0){
							sprintf(IPInfo, "eth0: %s", addressBuffer);
							readed_ip = 1;
							break;
						}
						else if (strcmp(ifAddrStruct->ifa_name, "wlan0") == 0){
							sprintf(IPInfo, "wlan0: %s", addressBuffer);
							readed_ip = 1;
							break;
						}else {
							readed_ip = 0;
						}
					}
					ifAddrStruct = ifAddrStruct->ifa_next;
				}
				getifaddrs(&ifAddrStruct);
			}

			if (raspi_version == 0){
				char hardware[MAX_SIZE];
				fd_hardware = open(HARDWARE_PATH, O_RDONLY);
				if (fd_hardware < 0){
					printf("failed to open /proc/device-tree/model\n");
				}
				else{
					if (read(fd_hardware, hardware, MAX_SIZE) < 0){
						printf("fail to read hardware version\n");
					}
					else{
						char raspi[23] = "0";
						strncpy(raspi, hardware, 22);
						if (strcmp(raspi, "Raspberry Pi 4 Model B") == 0){
							raspi_version = 4;
						}
						else if (strcmp(raspi, "Raspberry Pi 3 Model B") == 0){
							raspi_version = 3;
						}
					}
					
				}
				close(fd_hardware);
			}

			char CPUTemp[MAX_SIZE];
			fd_temp = open(TEMP_PATH, O_RDONLY);
			if (fd_temp < 0){
				temp = 0;
				printf("failed to open thermal_zone0/temp\n");
			}
			else{
				if (read(fd_temp, buf, MAX_SIZE) < 0){
					temp = 0;
					printf("fail to read temp\n");
				}
				else{
					temp = atoi(buf) / 1000.0;
					// printf("temp: %.1f\n", temp);
					sprintf(CPUTemp, "Temp: %.1fC", temp);
				}
			}
			close(fd_temp);

			char DiskInfo[MAX_SIZE];
			statfs("/", &disk_info);
			unsigned long long totalBlocks = disk_info.f_bsize;
			unsigned long long totalSize = totalBlocks * disk_info.f_blocks;
			size_t mbTotalsize = totalSize >> 20;
			unsigned long long freeDisk = disk_info.f_bfree * totalBlocks;
			size_t mbFreedisk = freeDisk >> 20;
			sprintf(DiskInfo, "Disk: %ld/%ldMB", mbFreedisk, mbTotalsize);

			ssd1306_drawText(0, 0, CPUInfo);
			ssd1306_drawText(56, 0, CPUTemp);
			ssd1306_drawText(0, 8, RamInfo);
			ssd1306_drawText(0, 16, DiskInfo);
			ssd1306_drawText(0, 24, IPInfo);

			ssd1306_display();
			delay(10);
		}

		if (raspi_version == 4){
			if (temp >= 55){
				if (fan_state != 1){
					wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
					fan_state = 1;
				}
			}
			else if (temp <= 48){
				if (fan_state != 0){
					wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
					fan_state = 0;
				}
			}
		}
		else if (raspi_version == 3){
			if (temp >= 46){
				if (fan_state != 1){
					wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
					fan_state = 1;
				}
			}
			else if (temp <= 40){
				if (fan_state != 0){
					wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
					fan_state = 0;
				}
			}
		}
		else{
			if (temp >= 55){
				if (fan_state != 1){
					wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
					fan_state = 1;
				}
			}
			else if (temp <= 48){
				if (fan_state != 0){
					wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
					fan_state = 0;
				}
			}
		}
		
		delay(500);

		if (temp <= 45){
			setRGB(Max_LED, 0x00, 0x00, 0xff, fd_i2c);
		}
		else if (temp <= 50){
			setRGB(Max_LED, 0x5f, 0x9e, 0xa0, fd_i2c);
		}
		else if (temp <= 55){
			setRGB(Max_LED, 0xff, 0xd7, 0x00, fd_i2c);
		}
		else if (temp <= 60){
			setRGB(Max_LED, 0xff, 0x8c, 0x00, fd_i2c);
		}
		else if (temp > 60 || temp < 65){
			setRGB(Max_LED, 0xff, 0x45, 0x00, fd_i2c);
		}
		else if (temp >= 65){
			setRGB(Max_LED, 0xff, 0x00, 0x00, fd_i2c);
		}
		
		count++;
		delay(500);
	}

	return 0;
}


void setRGB(int num, int R, int G, int B, int fd_i2c){
    if (num >= Max_LED){
        wiringPiI2CWriteReg8(fd_i2c, 0x00, 0xff);
        wiringPiI2CWriteReg8(fd_i2c, 0x01, R);
        wiringPiI2CWriteReg8(fd_i2c, 0x02, G);
        wiringPiI2CWriteReg8(fd_i2c, 0x03, B);
    }
    else if (num >= 0){
        wiringPiI2CWriteReg8(fd_i2c, 0x00, num);
        wiringPiI2CWriteReg8(fd_i2c, 0x01, R);
        wiringPiI2CWriteReg8(fd_i2c, 0x02, G);
        wiringPiI2CWriteReg8(fd_i2c, 0x03, B);
    }
}


void closeRGB(int fd_i2c){
    wiringPiI2CWriteReg8(fd_i2c, 0x07, 0x00);
    delay(10);
}


mem_info_t getRAM(void){
    mem_info_t mem;
    int fd,i;
    char buff[83];
    char kb[3];

    fd = open ("/proc/meminfo", O_RDONLY);
    if(fd < 0){
            perror("open:/proc/meminfo error");
            exit (0);
    }
    i=read(fd,buff,83);
    if(i<0){
            perror("read:/proc/meminfo error");
            exit (0);
    }
    //printf("%s i=%d\n",buff,i);
    sscanf(buff,"%s %u %s %s %u",mem.MemTotal,&mem.total,kb,mem.MemFree,&mem.free);
    //printf("tot=%u free=%u \n",mem.total,mem.free);
    close(fd);
    return mem;
}


double cal_cpuoccupy (cpu_occupy_t *o, cpu_occupy_t *n){
    double od, nd;
    double id, sd;
    double cpu_use ;
 
    od = (double) (o->user + o->nice + o->system +o->idle+o->softirq+o->iowait+o->irq);
    nd = (double) (n->user + n->nice + n->system +n->idle+n->softirq+n->iowait+n->irq);
 
    id = (double) (n->idle);    
    sd = (double) (o->idle);    
    if((nd-od) != 0)
        cpu_use =100.0 - ((id-sd))/(nd-od)*100.00;
        cpu_use = 0;
    return cpu_use;
}
 
void get_cpuoccupy (cpu_occupy_t *cpust){
    int fd;
    int n,i;
    char buff[256];
    cpu_occupy_t *cpu_occupy;
    cpu_occupy=cpust;

    fd = open ("/proc/stat", O_RDONLY);
    if(fd < 0){
            perror("open:/proc/stat error");
            exit (0);
    }
    i=read(fd,buff,256);
    if(i<0){
            perror("read:/proc/stat error");
            exit (0);
    }
    //printf("%s i=%d\n",buff,i);
    sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle ,&cpu_occupy->iowait,&cpu_occupy->irq,&cpu_occupy->softirq);
    close(fd);
}
 
double get_sysCpuUsage(){
    cpu_occupy_t cpu_stat1;
    cpu_occupy_t cpu_stat2;
    double cpu;
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat1);
    sleep(1);
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat2);
 
    cpu = cal_cpuoccupy ((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);
 
    return cpu;
}
