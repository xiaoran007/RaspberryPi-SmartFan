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


