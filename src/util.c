#include "util.h"

int get_pi_version(void){
    char pi_version[MAX_LEN];
    int fd;
    int return_version;
    fd = open(MODEL_PATH, O_RDONLY);
    if (fd < 0){
        printf("open %s failed!\n", MODEL_PATH);
        return -1;
    }
    else{
        if (read(fd, pi_version, MAX_LEN) < 0){
            printf("read %s failed!\n", MODEL_PATH);
        }
        else{
            char raspi[23] = "0";
			strncpy(raspi, pi_version, 22);
			if (strcmp(raspi, "Raspberry Pi 4 Model B") == 0){
				return_version = 4;
			}
			else if (strcmp(raspi, "Raspberry Pi 3 Model B") == 0){
				return_version = 3;
			}
            else{
                return_version = -1;
            }
        }
        close(fd);
        return return_version;
    }
}

char* get_ip_info(void){
    char ip_info[MAX_LEN];
    struct ifaddrs *ifaddr;
    getifaddrs(&ifaddr);
    while (ifaddr!= NULL){
        if (ifaddr->ifa_addr->sa_family == AF_INET){
            struct sockaddr_in *ipv4 = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, &ipv4->sin_addr, ip_info, MAX_LEN);
        }
        ifaddr = ifaddr->ifa_next;
    }
}


