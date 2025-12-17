#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

typedef struct {
	double latitude;
	double longitude;
	double speed;

}	gpsData;

typedef struct {
	float x;
	float y;
	float z;
	float xAccel;
        float yAccel;
        float zAccel;
}	gyroData;

typedef struct {
	float temp;
	float rpm;
	float speed;
}	vehicleData;

typedef struct {
	float temperature;
	float humidity;
	int valid;
}	humidityData;

#endif
