#include "sensor_Types.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    while (1) {
        FILE *fp;
        double temp_c;

        fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
        if (fp == NULL) {
            perror("Error opening temperature file");
            return 1;
        }

        if (fscanf(fp, "%lf", &temp_c) == 1) {
            printf("CPU Temperature: %.2f°C\n", temp_c / 1000.0);
        } else {
            perror("Error reading temperature");
        }

        fclose(fp);
        sleep(2); // wait 2 seconds before next read
    }
    return 0;
}

