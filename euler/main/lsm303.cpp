#include <cstring>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "cJSON.h"

#include "parameter.h"

extern QueueHandle_t xQueueTrans;
extern MessageBufferHandle_t xMessageBufferToClient;

static const char *TAG = "MPU";

// I2Cdev and LSM303DLHC must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "LSM303DLHC.h"

#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD 0.0174533

LSM303DLHC accelMag(LSM303DLHC_DEFAULT_ADDRESS_A, LSM303DLHC_DEFAULT_ADDRESS_M);

#define SENSORS_GAUSS_TO_MICROTESLA 100
#define SENSORS_GRAVITY_EARTH (9.80665F) //< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD (SENSORS_GRAVITY_EARTH)
#define _lsm303Mag_Gauss_LSB_XY 1100
#define _lsm303Mag_Gauss_LSB_Z 980
#define _lsm303Acc_SHIFT 6
#define _lsm303Acc_LSB 0.00782


// https://github.com/adafruit/Adafruit_AHRS/blob/master/src/Adafruit_Simple_AHRS.cpp
void getOrientation(float *roll, float *pitch, float *heading) {
	// Get raw value
	int16_t ax, ay, az;
	int16_t mx, my, mz;
	accelMag.getAcceleration(&ax, &ay, &az);
	accelMag.getMag(&mx, &my, &mz);
	ESP_LOGD(TAG, "acc=0x%x 0x%x 0x%x", ax, ay, az);
	ESP_LOGD(TAG, "mag=%d %d %d", mx, my, mz);
	mx = mx + CONFIG_MAGX;
	my = my + CONFIG_MAGY;
	mz = mz + CONFIG_MAGZ;

	// Calculation by scale
	float _ax = (float)(ax>>_lsm303Acc_SHIFT) * _lsm303Acc_LSB * SENSORS_GRAVITY_STANDARD;
	float _ay = (float)(ay>>_lsm303Acc_SHIFT) * _lsm303Acc_LSB * SENSORS_GRAVITY_STANDARD;
	float _az = (float)(az>>_lsm303Acc_SHIFT) * _lsm303Acc_LSB * SENSORS_GRAVITY_STANDARD;
	ESP_LOGD(TAG, "acc=%f %f %f", _ax, _ay, _az);
	float _mx = (float)mx / _lsm303Mag_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
	float _my = (float)my / _lsm303Mag_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
	float _mz = (float)mz / _lsm303Mag_Gauss_LSB_Z * SENSORS_GAUSS_TO_MICROTESLA;
	ESP_LOGD(TAG, "mag=%f %f %f", _mx, _my, _mz);

	//float const PI_F = 3.14159265F;
	float const PI_F = M_PI;
	float _roll;
	float _pitch;
	float _heading;

	// roll: Rotation around the X-axis. -180 <= roll <= 180
	// a positive roll angle is defined to be a clockwise rotation about the
	// positive X-axis
	//
	//					  y
	//		roll = atan2(---)
	//					  z
	//
	// where:  y, z are returned value from accelerometer sensor
	_roll =
		(float)atan2(_ay, _az);

	// pitch: Rotation around the Y-axis. -180 <= roll <= 180
	// a positive pitch angle is defined to be a clockwise rotation about the
	// positive Y-axis
	//
	//								   -x
	//		pitch = atan(-------------------------------)
	//					  y * sin(roll) + z * cos(roll)
	//
	// where:  x, y, z are returned value from accelerometer sensor
	if (_ay * sin(_roll) + _az * cos(_roll) == 0)
		_pitch = _ax > 0 ? (PI_F / 2) : (-PI_F / 2);
	else
		_pitch =
			(float)atan(-_ax /
			(_ay * sin(_roll) +
			_az * cos(_roll)));

	// heading: Rotation around the Z-axis. -180 <= roll <= 180
	// a positive heading angle is defined to be a clockwise rotation about the
	// positive Z-axis
	//
	//										 z * sin(roll) - y * cos(roll)
	//	 heading =
	//	 atan2(--------------------------------------------------------------------------)
	//					  x * cos(pitch) + y * sin(pitch) * sin(roll) + z *
	//					  sin(pitch) * cos(roll))
	//
	// where:  x, y, z are returned value from magnetometer sensor
	_heading =
		(float)atan2(_mz * sin(_roll) - _my * cos(_roll),
		_mx * cos(_pitch) + _my * sin(_pitch) * sin(_roll) + _mz * sin(_pitch) * cos(_roll));

	// Convert angular data to degree
	*roll = _roll * 180 / PI_F;
	*pitch = _pitch * 180 / PI_F;
	*heading = _heading * 180 / PI_F;
}

void lsm303(void *pvParameters){
	// Initialize device
	accelMag.initialize();
	
	if (!accelMag.testConnection()) {
		ESP_LOGE(TAG, "testConnection fail");
		vTaskDelete(NULL);
	}

	// set scale to 4Gs
	accelMag.setAccelFullScale(4);

	// set accel data rate to 200Hz
	accelMag.setAccelOutputDataRate(200);

	// test scale
	ESP_LOGI(TAG, "Accel Scale: %d", accelMag.getAccelFullScale());

	// test data rate
	ESP_LOGI(TAG, "Accel Output Data Rate: %d", accelMag.getAccelOutputDataRate());

	// set mag data rate to 220Hz
	accelMag.setMagOutputDataRate(220);

	// test mag data rate
	ESP_LOGI(TAG, "Mag Output Data Rate: %d", accelMag.getMagOutputDataRate());

	// set mag gain
	accelMag.setMagGain(1100);

	// test mag gain
	ESP_LOGI(TAG, "Mag Gain: %d", accelMag.getMagGain());

	// enable mag
	accelMag.setMagMode(LSM303DLHC_MD_CONTINUOUS);

	// test mag gain
	ESP_LOGI(TAG, "Mag mode: %d", accelMag.getMagMode());

	int elasped = 0;
	while(1){
#if 0
		int16_t ax, ay, az;
		int16_t mx, my, mz; 
		accelMag.getAcceleration(&ax, &ay, &az);
		accelMag.getMag(&mx, &my, &mz);
		ESP_LOGI(TAG, "acc=0x%x 0x%x 0x%x", ax, ay, az);
		ESP_LOGI(TAG, "mag=%d %d %d", mx, my, mz);
		mx = mx + CONFIG_MAGX;
		my = my + CONFIG_MAGY;
		mz = mz + CONFIG_MAGZ;

		float _ax = (float)(ax>>_lsm303Acc_SHIFT) * _lsm303Acc_LSB * SENSORS_GRAVITY_STANDARD;
		float _ay = (float)(ay>>_lsm303Acc_SHIFT) * _lsm303Acc_LSB * SENSORS_GRAVITY_STANDARD;
		float _az = (float)(az>>_lsm303Acc_SHIFT) * _lsm303Acc_LSB * SENSORS_GRAVITY_STANDARD;
		ESP_LOGI(TAG, "acc=%f %f %f", _ax, _ay, _az);
		float _mx = (float)mx / _lsm303Mag_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
		float _my = (float)my / _lsm303Mag_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
		float _mz = (float)mz / _lsm303Mag_Gauss_LSB_Z * SENSORS_GAUSS_TO_MICROTESLA;
		ESP_LOGI(TAG, "mag=%f %f %f", _mx, _my, _mz);
#endif
		float roll, pitch, heading;
		getOrientation(&roll, &pitch, &heading);
		ESP_LOGD(TAG, "roll=%f pitch=%f heading=%f", roll, pitch, heading);
		
		/* Print Data every 10 times */
		if (elasped > 10) {
			ESP_LOGI(TAG, "roll=%f pitch=%f heading=%f", roll, pitch, heading);
			// Send UDP packet
			POSE_t pose;
			pose.roll = roll;
			pose.pitch = pitch;
			pose.yaw = heading;
			if (xQueueSend(xQueueTrans, &pose, 100) != pdPASS ) {
				ESP_LOGE(pcTaskGetName(NULL), "xQueueSend fail");
			}

			// Send WEB request
			cJSON *request;
			request = cJSON_CreateObject();
			cJSON_AddStringToObject(request, "id", "data-request");
			cJSON_AddNumberToObject(request, "roll", roll);
			cJSON_AddNumberToObject(request, "pitch", pitch);
			cJSON_AddNumberToObject(request, "yaw", heading);
			char *my_json_string = cJSON_Print(request);
			ESP_LOGD(TAG, "my_json_string\n%s",my_json_string);
			size_t xBytesSent = xMessageBufferSend(xMessageBufferToClient, my_json_string, strlen(my_json_string), 100);
			if (xBytesSent != strlen(my_json_string)) {
				ESP_LOGE(TAG, "xMessageBufferSend fail");
			}
			cJSON_Delete(request);
			cJSON_free(my_json_string);

			vTaskDelay(1);
			elasped = 0;
		}
		elasped++;
		vTaskDelay(1);
	}

	// Never reach here
	vTaskDelete(NULL);
}
