#include "sensor_Types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int read_gps_data(int fd, gpsData *data) {
    // Simulated data
    data->latitude = 47.645231;
    data->longitude = -122.354123;
    data->speed = 15.5;  // km/h
    usleep(500000);      // simulate delay
    return 0;
}


int open_gps_serial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("GPS device is not responding");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);  // typical GPS baud rate
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB; // no parity
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

/*int read_gps_data(int fd, gpsData *data) {
    char buffer[256];
    int n = read(fd, buffer, sizeof(buffer)-1);
    if (n <= 0) return -1;

    buffer[n] = '\0';

    // Very basic parser: look for $GPRMC sentence
    char *line = strstr(buffer, "$GPRMC");
    if (!line) return -1;

    // Example GPRMC format: $GPRMC,hhmmss,A,lat,N,lon,E,speed,...
    char *token = strtok(line, ",");
    int field = 0;
    while (token) {
        switch (field) {
            case 3: data->latitude = atof(token); break;   // lat
            case 5: data->longitude = atof(token); break;  // lon
            case 7: data->speed = atof(token); break;      // speed in knots
        }
        token = strtok(NULL, ",");
        field++;
    }

    // Convert speed from knots to km/h
    data->speed *= 1.852;

    return 0;
}*/

#ifdef TEST_GPS
int main() {
    int fd = open_gps_serial("/dev/serial0"); // your Pi UART
    if (fd == -1) return 1;

    gpsData gps;
    while (1) {
        if (read_gps_data(fd, &gps) == 0) {
            printf("Lat: %f, Lon: %f, Speed: %.2f km/h\n",
                   gps.latitude, gps.longitude, gps.speed);
        }
        usleep(500000); // 0.5 sec
    }
    close(fd);
    return 0;
}
#endif
