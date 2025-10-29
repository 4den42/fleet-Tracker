#include "sensor_Types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void read_voltage() {
    FILE *fp;
    char buffer[128];

    fp = popen("vcgencmd measure_volts", "r");
    if (fp == NULL) {
        perror("Failed to run vcgencmd");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("Voltage: %s", buffer);
    }

    pclose(fp);
}

void read_throttled() {
    FILE *fp;
    char buffer[128];

    fp = popen("vcgencmd get_throttled", "r");
    if (fp == NULL) {
        perror("Failed to run vcgencmd");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        unsigned int throttled;
        if (sscanf(buffer, "throttled=0x%x", &throttled) == 1) {
            printf("Throttled: 0x%x\n", throttled);
            if (throttled == 0) {
                printf("✅ No throttling detected.\n");
            } else {
                if (throttled & 1)  printf("⚠️  Under-voltage detected!\n");
                if (throttled & 2)  printf("⚠️  ARM frequency capped!\n");
                if (throttled & 4)  printf("⚠️  Currently throttled!\n");
                if (throttled & 16) printf("⚠️  Under-voltage has occurred previously.\n");
                if (throttled & 32) printf("⚠️  Frequency capping has occurred previously.\n");
                if (throttled & 64) printf("⚠️  Throttling has occurred previously.\n");
            }
        }
    }

    pclose(fp);
}

int main(void) {
    while (1) {
        system("clear"); // Clears the terminal for a live-updating display
        printf("=== Raspberry Pi 5 Power Monitor ===\n\n");
        read_voltage();
        read_throttled();
        printf("\n(Updating every 2 seconds...)\n");
        sleep(2);
    }

    return 0;
}

