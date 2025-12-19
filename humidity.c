#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <gpiod.h>
#include "sensor_Types.h"  // your header

#define DHT_PIN 17       // GPIO pin
#define MAX_TIMINGS 85

humidityData readDHT22(struct gpiod_chip *chip, unsigned int line_num) {
    humidityData result = {0};
    result.valid = 0;

    struct gpiod_line *line = chip->gpiod_chip_get_info();
    if (!line) {
        fprintf(stderr, "Failed to get GPIO line\n");
        return result;
    }

    // Request line as output to send start signal
    struct gpiod_line_request_config req_out = {
        .consumer = "dht22",
        .request_type = GPIOD_LINE_DIRECTION_OUTPUT,
    };

    if (gpiod_line_request_get_fd(line, &req_out, 0) < 0) {
        fprintf(stderr, "Failed to request line as output\n");
        gpiod_line_request_release(line);
        return result;
    }

    gpiod_line_request_set_value(line, 0); // Pull low
    usleep(18000);                 // 18ms
    gpiod_line_request_set_value(line, 1); // Pull high
    usleep(40);

    // Request line as input to read data
    struct gpiod_line_request_config req_in = {
        .consumer = "dht22",
        .request_type = GPIOD_LINE_DIRECTION_INPUT,
    };

    if (gpiod_line_request_get_fd() < 0) {
        fprintf(stderr, "Failed to request line as input\n");
        gpiod_line_request_release(line);
        return result;
    }

    int data[5] = {0};
    int last_state = 1;
    int j = 0;

    for (int i = 0; i < MAX_TIMINGS; i++) {
        int counter = 0;
        while (gpiod_line_request_get_value(line) == last_state) {
            counter++;
            struct timespec ts = {0, 1000}; // 1 us
            nanosleep(&ts, NULL);
            if (counter == 255) break;
        }
        last_state = gpiod_line_request_get_value(line);

        if (counter == 255) break;

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (counter > 50) data[j / 8] |= 1;
            j++;
        }
    }

    if ((j >= 40) && ((data[0]+data[1]+data[2]+data[3]) & 0xFF) == data[4]) {
        result.humidity = ((data[0] << 8) + data[1]) / 10.0;
        result.temperature = (((data[2] & 0x7F) << 8) + data[3]) / 10.0;
        if (data[2] & 0x80) result.temperature = -result.temperature;
        result.valid = 1;
    }

    gpiod_line_request_release(line); // Release the line
    return result;
}

int main(void) {
    struct gpiod_chip *chip = gpiod_chip_info_get_name("gpiochip0");
    if (!chip) {
        fprintf(stderr, "Failed to open gpiochip0\n");
        return 1;
    }

    printf("Reading DHT22 on GPIO %d...\n", DHT_PIN);

    while (1) {
        humidityData reading = readDHT22(chip, DHT_PIN);
        if (reading.valid) {
            printf("Temp: %.1f°C | Humidity: %.1f%%\n",
                   reading.temperature, reading.humidity);
        } else {
            printf("Failed reading, retrying...\n");
        }
        usleep(2000000); // 2 sec
    }

    gpiod_chip_close(chip); // Close the chip
    return 0;
}
