/*
 * mpu6050.cpp
 *
 * Created: 12/18/2013 11:05:43 PM
 *  Author: Mason
 */ 

#include <avr/io.h>
#include "mpu6050.h"
#include "twi.h"

static inline void mpu_set_clock_source(uint8_t source) {
 	twi_write_bits(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_PWR_MGMT_1, MPU60X0_PWR1_CLKSEL_BIT, MPU60X0_PWR1_CLKSEL_LENGTH, source);
}

static inline void mpu_set_gyro_full_scale_range(uint8_t range) {
	twi_write_bits(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_GYRO_CONFIG, MPU60X0_GCONFIG_FS_SEL_BIT, MPU60X0_GCONFIG_FS_SEL_LENGTH, range);
}

static inline void mpu_set_accel_full_scale_range(uint8_t range) {
	twi_write_bits(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_ACCEL_CONFIG, MPU60X0_ACONFIG_AFS_SEL_BIT, MPU60X0_ACONFIG_AFS_SEL_LENGTH, range);
}

static inline void mpu_set_sleep_enabled(bool enabled) {
	twi_write_bit(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_PWR_MGMT_1, MPU60X0_PWR1_SLEEP_BIT, enabled);
}

static inline uint8_t mpu_get_device_ID() {
	volatile uint8_t temp = twi_read_bits(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_WHO_AM_I, MPU60X0_WHO_AM_I_BIT, MPU60X0_WHO_AM_I_LENGTH);
	return temp;
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void mpu_init() {
    mpu_set_clock_source(MPU60X0_CLOCK_PLL_XGYRO);
    mpu_set_gyro_full_scale_range(MPU60X0_GYRO_FS_250);
    mpu_set_accel_full_scale_range(MPU60X0_ACCEL_FS_2);
    mpu_set_sleep_enabled(false);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool mpu_test_connection() {
	return mpu_get_device_ID() == 0b110100;
}

/** Get raw 6-axis motion sensor readings (accel/gyro).
 * Retrieves all currently available motion sensor values.
 * @param ax 16-bit signed integer container for accelerometer X-axis value
 * @param ay 16-bit signed integer container for accelerometer Y-axis value
 * @param az 16-bit signed integer container for accelerometer Z-axis value
 * @param gx 16-bit signed integer container for gyroscope X-axis value
 * @param gy 16-bit signed integer container for gyroscope Y-axis value
 * @param gz 16-bit signed integer container for gyroscope Z-axis value
 * @see getAcceleration()
 * @see getRotation()
 * @see MPU60X0_RA_ACCEL_XOUT_H
 */
void mpu_get_motion_six_dof(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
	uint8_t buffer[14] = {0};
	for(uint8_t i = 0; i < 14; i++)
		buffer[i] = twi_read_byte(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_ACCEL_XOUT_H + i);

	*ax = (((int16_t)buffer[0]) << 8) | buffer[1];
	*ay = (((int16_t)buffer[2]) << 8) | buffer[3];
	*az = (((int16_t)buffer[4]) << 8) | buffer[5];
	*gx = (((int16_t)buffer[8]) << 8) | buffer[9];
	*gy = (((int16_t)buffer[10]) << 8) | buffer[11];
	*gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}
