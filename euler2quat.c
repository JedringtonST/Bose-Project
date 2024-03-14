/**
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "peripherals.h"
#include "reg_map.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

void __attribute__ ((signal)) algo_00_init(void);
void __attribute__ ((signal)) algo_00(void);
//Sensitivity based on fullscale and ODR
#define ACC_SENS 0.000244f // **** raw acc sensitivity scaling  --> ACC FULL SCALE MUST BE SET TO +/-8 g
#define GYR_SENS 0.070f    // **** raw gyro sensitivity scaling --> GYRO FULL SCALE MUST BE SET TO +/-2000 dps
#define GYR_SENS_RPS 0.00122173f // Rad-per-sec instead of Deg-per-sec, equal to GYR_SENS / 180 * pi
#define ACC_GYR_ODR 104.0f // **** output data rate             --> ODR MUST BE SET TO 104 Hz

#define GYRO_WEIGHT 0.98f  // trust gyro more for a stable output

#define NEAR_ZERO        0.0001f

//PI definitions
#define NEAR_SINGULARITY 1.56f // pi/2 is 1.57, cos(1.56) is 0.01
#define TWO_PI  6.28318530f
#define PI      3.14159265f
#define HALF_PI 1.57079632f
#define RAD_DEG 57.2957795f

#define MEAN_VAR_IIR_ALPHA   0.1f // not too low otherwise smooth out too much and variance not reactive to local conditions
#define MEAN_VAR_IIR_1_ALPHA 0.9f // **** MUST BE 1.0f - ALPHA
#define ACC_VAR_TH         5e-4f  // threshold based on tests
#define GYR_VAR_TH         2e-3f  // threshold based on tests
#define STATIC_CNT_TH      10     // not too high otherwise gbias update delayed

static uint16_t samplecnt; // unsigned so that wraparound upon increment is defined behavior

static float AccX_m, AccY_m, AccZ_m, AccX_v, AccY_v, AccZ_v;
static float GyrX_m, GyrY_m, GyrZ_m, GyrX_v, GyrY_v, GyrZ_v;
static float GbiasX, GbiasY, GbiasZ;
static int8_t static_cnt;

// delta time is a float but ISPU can only read 2 bytes at a time; referred to 104Hz, must be multiplied by 104Hz/actual ODR
static union {
	uint32_t u32;
	float f;
} dtime;

static float Roll, Pitch, Yaw;

static volatile uint32_t int_status;


void __attribute__ ((signal)) algo_00_init(void)
{
	// delta time is a float but ISPU can only read 2 bytes at a time; referred to 104Hz, must be multiplied by 104Hz/actual ODR
	dtime.u32 = ((uint32_t) *((volatile uint16_t *)ISPU_DTIME_1) << 16) | ((uint32_t) *((volatile uint16_t *)ISPU_DTIME_0));
	dtime.f *= 104.0f / ACC_GYR_ODR; // value is referred to 104Hz, must be multiplied by 104Hz/actual ODR

	Roll  = 0.0f;
	Pitch = 0.0f;
	Yaw   = 0.0f;

	samplecnt = 0;

	GbiasX = 0.0f;
	GbiasY = 0.0f;
	GbiasZ = 0.0f;
	static_cnt = 0;
}

void __attribute__ ((signal)) algo_00(void)
{
	// read raw data from sensors
	int16_t rawAccX = cast_sint16_t(ISPU_ARAW_X);
	int16_t rawAccY = cast_sint16_t(ISPU_ARAW_Y);
    int16_t rawAccZ = cast_sint16_t(ISPU_ARAW_Z);

	int16_t rawGyrX = cast_sint16_t(ISPU_GRAW_X);
	int16_t rawGyrY = cast_sint16_t(ISPU_GRAW_Y);
    int16_t rawGyrZ = cast_sint16_t(ISPU_GRAW_Z);


    // apply sensitivity scaling based full scale settings
    float AccX = ACC_SENS * (float)rawAccX;
	float AccY = ACC_SENS * (float)rawAccY;
	float AccZ = ACC_SENS * (float)rawAccZ;

	float GyrX = GYR_SENS_RPS * (float)rawGyrX;
	float GyrY = GYR_SENS_RPS * (float)rawGyrY;
	float GyrZ = GYR_SENS_RPS * (float)rawGyrZ;



	// mean-var estimation based on IIR
	if (samplecnt == 0) {
		AccX_m = AccX; AccY_m = AccY; AccZ_m = AccZ;
		GyrX_m = GyrX; GyrY_m = GyrY; GyrZ_m = GyrZ;
	}

	float diff, incr;
	diff = AccX - AccX_m; incr = MEAN_VAR_IIR_ALPHA * diff; AccX_m += incr; AccX_v = MEAN_VAR_IIR_1_ALPHA * (AccX_v + diff * incr);
    diff = AccY - AccY_m; incr = MEAN_VAR_IIR_ALPHA * diff; AccY_m += incr; AccY_v = MEAN_VAR_IIR_1_ALPHA * (AccY_v + diff * incr);
    diff = AccZ - AccZ_m; incr = MEAN_VAR_IIR_ALPHA * diff; AccZ_m += incr; AccZ_v = MEAN_VAR_IIR_1_ALPHA * (AccZ_v + diff * incr);

    diff = GyrX - GyrX_m; incr = MEAN_VAR_IIR_ALPHA * diff; GyrX_m += incr; GyrX_v = MEAN_VAR_IIR_1_ALPHA * (GyrX_v + diff * incr);
    diff = GyrY - GyrY_m; incr = MEAN_VAR_IIR_ALPHA * diff; GyrY_m += incr; GyrY_v = MEAN_VAR_IIR_1_ALPHA * (GyrY_v + diff * incr);
    diff = GyrZ - GyrZ_m; incr = MEAN_VAR_IIR_ALPHA * diff; GyrZ_m += incr; GyrZ_v = MEAN_VAR_IIR_1_ALPHA * (GyrZ_v + diff * incr);




	// identify static condition when variance is below threhsold
	int8_t static_flag = (AccX_v < ACC_VAR_TH) && (AccY_v < ACC_VAR_TH) && (AccZ_v < ACC_VAR_TH)
	                  && (GyrX_v < GYR_VAR_TH) && (GyrY_v < GYR_VAR_TH) && (GyrZ_v < GYR_VAR_TH);

	// update bias estimate when variance is low enough and mean has had time to settle
	if (static_flag) {
		if (static_cnt < STATIC_CNT_TH) ++static_cnt; // bounded growth
		else { // static_cnt >= STATIC_CNT_TH // mean estimate has settled
			GbiasX = GyrX_m;
			GbiasY = GyrY_m;
			GbiasZ = GyrZ_m;
		}
	} else static_cnt = 0; // reset

	// clean up sensor data using available mean (this is optional and can be commented out)
	//AccX = AccX_m; AccY = AccY_m; AccZ = AccZ_m; // smooth Acc data
	//GyrX = GyrX_m; GyrY = GyrY_m; GyrZ = GyrZ_m; // smooth Gyr data

	// compensate for gyro bias (this is needed to avoid drift on Yaw because there is no Mag in the system)
	GyrX -= GbiasX;
	GyrY -= GbiasY;
	GyrZ -= GbiasZ;




	// estimate from accelerometer data, DT0058
	float Roll_from_Acc = atan2f(AccY, AccZ);

    float Pitch_from_Acc;
	float AccZ2 = AccY * sinf(Roll_from_Acc) + AccZ * cosf(Roll_from_Acc);
	if (fabsf(AccZ2) < NEAR_ZERO) { // avoid div-by-zero
		if (AccX < 0.0f) Pitch_from_Acc = +HALF_PI;
		else             Pitch_from_Acc = -HALF_PI;
	} else {
		Pitch_from_Acc = atanf(-AccX / AccZ2);
	}

	// initialize best estimate upon first iteration
	if (samplecnt == 0) {
		Roll  = Roll_from_Acc;
		Pitch = Pitch_from_Acc;
		Yaw   = 0.0f;
	}

	// compute Euler angle increment/derivative, DT0060
	float cosRoll  = cosf(Roll);
	float sinRoll  = sinf(Roll);
	float invCosPitch = 1.0f/cosf(Pitch); // division is slow, done once here
	float tanPitch = tanf(Pitch);

	float Roll_d  = GyrX + GyrY * sinRoll * tanPitch    + GyrZ * cosRoll * tanPitch;
	float Pitch_d =        GyrY * cosRoll               - GyrZ * sinRoll;
    float Yaw_d   =        GyrY * sinRoll * invCosPitch + GyrZ * cosRoll * invCosPitch;

	// handle singularity / Gimbal lock
	if ((Pitch > +NEAR_SINGULARITY) || (Pitch < -NEAR_SINGULARITY)) {
		Roll_d  = -GyrZ;
		Pitch_d = -GyrY;
		Yaw_d   = -GyrX;
	}

    // update previous best estimate, DT0060
	float Roll_from_Gyr  = Roll  + Roll_d  * dtime.f;
	float Pitch_from_Gyr = Pitch + Pitch_d * dtime.f;
	float Yaw_from_Gyr   = Yaw   + Yaw_d   * dtime.f;



	// apply complementary filter, mix estimates from Acc and Gyro, DT0060
	float d;

	d = Roll_from_Acc - Roll_from_Gyr;
	if (d > +PI) Roll_from_Acc = Roll_from_Acc - TWO_PI;
	if (d < -PI) Roll_from_Acc = Roll_from_Acc + TWO_PI;
	Roll = Roll_from_Acc + GYRO_WEIGHT * (Roll_from_Gyr - Roll_from_Acc); // (1-gyroweigth)*fromacc + gyroweigth*fromgyro

	d = Pitch_from_Acc - Pitch_from_Gyr;
	if (d > +PI) Pitch_from_Acc = Pitch_from_Acc - TWO_PI;
	if (d < -PI) Pitch_from_Acc = Pitch_from_Acc + TWO_PI;
	Pitch = Pitch_from_Acc + GYRO_WEIGHT * (Pitch_from_Gyr - Pitch_from_Acc); // (1-gyroweigth)*fromacc + gyroweigth*fromgyro

	Yaw = Yaw_from_Gyr; // there is no Yaw from Acc



	// reduce to allowed range, DT0060
	if (Roll > +TWO_PI) Roll -= TWO_PI;
	if (Roll < -TWO_PI) Roll += TWO_PI;
	if (Roll > +PI)     Roll -= TWO_PI;
	if (Roll < -PI)     Roll += TWO_PI;

    if (Pitch > +TWO_PI)  Pitch -= TWO_PI;
	if (Pitch < -TWO_PI)  Pitch += TWO_PI;
	if (Pitch > +PI)      Pitch -= TWO_PI;
	if (Pitch < -PI)      Pitch += TWO_PI;
	if (Pitch > +HALF_PI) Pitch = +PI - Pitch;
	if (Pitch < -HALF_PI) Pitch = -PI - Pitch;

	if (Yaw > +TWO_PI) Yaw -= TWO_PI;
	if (Yaw < -TWO_PI) Yaw += TWO_PI;
	if (Yaw > +PI)     Yaw -= TWO_PI;
	if (Yaw < -PI)     Yaw += TWO_PI;



	//Radians to Degrees
	float RollD 	= Roll 	* RAD_DEG;
	float PitchD	= Pitch * RAD_DEG;
	float YawD 		= Yaw 	* RAD_DEG;


	// Euler to Quaternion Output
	 float r2 = Roll / 2.0f;
	 float p2 = Pitch/ 2.0f;
	 float y2 = Yaw  / 2.0f;

	 float Cr2 = cosf(r2); float Sr2 = sinf(r2);
	 float Cp2 = cosf(p2); float Sp2 = sinf(p2);
	 float Cy2 = cosf(y2); float Sy2 = sinf(y2);

	 float A = Cr2*Cp2;    float B = Sr2*Sp2;
	 float C = Sr2*Cp2;    float D = Cr2*Sp2;

	 float qw = A*Cy2 + B*Sy2;
	 float qx = C*Cy2 - D*Sy2;
	 float qy = D*Cy2 + C*Sy2;
	 float qz = A*Sy2 - B*Cy2;


	// write outputs
	cast_uint16_t(ISPU_DOUT_00) = samplecnt;

    cast_sint16_t(ISPU_DOUT_01) = rawAccX;
	cast_sint16_t(ISPU_DOUT_02) = rawAccY;
	cast_sint16_t(ISPU_DOUT_03) = rawAccZ;

    cast_sint16_t(ISPU_DOUT_04) = rawGyrX;
	cast_sint16_t(ISPU_DOUT_05) = rawGyrY;
	cast_sint16_t(ISPU_DOUT_06) = rawGyrZ;

	cast_float(ISPU_DOUT_07) = YawD;
	cast_float(ISPU_DOUT_09) = PitchD;
	cast_float(ISPU_DOUT_11) = RollD;


	cast_float(ISPU_DOUT_13) = qx;
	cast_float(ISPU_DOUT_15) = qy;
	cast_float(ISPU_DOUT_17) = qz;
	cast_float(ISPU_DOUT_19) = qw;

	cast_float(ISPU_DOUT_21) = GbiasX;
	cast_float(ISPU_DOUT_23) = GbiasY;
	cast_float(ISPU_DOUT_25) = GbiasZ;

    ++samplecnt; // this is unsigned, wraparound upon increment is defined behavior


    int_status = int_status | 0x1u;

}

int main(void)
{
	// set boot done flag
	uint8_t status = cast_uint8_t(ISPU_STATUS);
	status = status | 0x04u;
	cast_uint8_t(ISPU_STATUS) = status;

	// enable algorithms interrupt request generation
	cast_uint8_t(ISPU_GLB_CALL_EN) = 0x01u;

	while (true) {
		stop_and_wait_start_pulse;

		// reset status registers and interrupts
		int_status = 0u;
		cast_uint32_t(ISPU_INT_STATUS) = 0u;
		cast_uint8_t(ISPU_INT_PIN) = 0u;

		// get all the algorithms to run in this time slot
		cast_uint32_t(ISPU_CALL_EN) = cast_uint32_t(ISPU_ALGO) << 1;

		// wait for all algorithms execution
		while (cast_uint32_t(ISPU_CALL_EN) != 0u) {
		}

		// get interrupt flags
		uint8_t int_pin = 0u;
		int_pin |= ((int_status & cast_uint32_t(ISPU_INT1_CTRL)) > 0u) ? 0x01u : 0x00u;
		int_pin |= ((int_status & cast_uint32_t(ISPU_INT2_CTRL)) > 0u) ? 0x02u : 0x00u;

		// set status registers and generate interrupts
		cast_uint32_t(ISPU_INT_STATUS) = int_status;
		cast_uint8_t(ISPU_INT_PIN) = int_pin;
	}
}

