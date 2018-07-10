/* 
   GetInfo.pp: Report info on EM7180 SENtral sensor hub using WiringPi

   This file is part of EM7180.

   EM7180 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   EM7180 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with EM7180.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "EM7180.h"
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static const uint8_t  ARES           = 8;    // Gs
static const uint16_t GRES           = 2000; // degrees per second
static const uint16_t MRES           = 1000; // microTeslas
static const uint8_t  MAG_RATE       = 100;  // Hz
static const uint16_t ACCEL_RATE     = 200;  // Hz
static const uint16_t GYRO_RATE      = 200;  // Hz
static const uint8_t  BARO_RATE      = 50;   // Hz
static const uint8_t  Q_RATE_DIVISOR = 3;    // 1/3 gyro rate
 
EM7180 em7180 = EM7180(ARES, GRES, MRES, MAG_RATE, ACCEL_RATE, GYRO_RATE, BARO_RATE, Q_RATE_DIVISOR);

void setup()
{
    // Set up the wiringPi library
    if (wiringPiSetup () < 0) {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        exit(1);
    }

    // Start the EM7180 in master mode
    if (!em7180.begin()) {

        while (true) {
            printf(em7180.getErrorString());
        }
    }    
}

void loop()
{  
    if (em7180.hasBaro())        printf("A barometer is installed.\n");
    if (em7180.hasHumidity())    printf("A humidity sensor is installed.\n");
    if (em7180.hasTemperature()) printf("A temperature sensor is installed.\n");
    if (em7180.hasCustom1())     printf("A custom sensor is installed.\n");
    if (em7180.hasCustom2())     printf("A second custom sensor is installed.\n");
    if (em7180.hasCustom3())     printf("A third custom sensor is installed.\n");

    printf("EM7180 ROM Version: 0x%02X; should be: 0xE609\n", em7180.getRomVersion());
    printf("EM7180 RAM Version: 0x%02X\n", em7180.getRamVersion());
    printf("EM7180 ProductID:   0x%02X; should be 0x80\n", em7180.getProductId());
    printf("EM7180 RevisionID:  0x%02X; should 0x02\n", em7180.getRevisionId());

    printf("Actual MagRate   = %d Hz\n", em7180.getActualMagRate());
    printf("Actual AccelRate = %d Hz\n", 10*em7180.getActualAccelRate());
    printf("Actual GyroRate  = %d Hz\n", 10*em7180.getActualGyroRate());
    printf("Actual BaroRate  = %d Hz\n", em7180.getActualBaroRate());
    printf("Actual TempRate  = %d Hz\n", em7180.getActualTempRate());

    printf("Run status: %s\n", em7180.runStatusNormal() ? "normal" : "other");

    if (em7180.algorithmStatusStandby())                 printf("EM7180 standby status\n");
    if (em7180.algorithmStatusSlow())                    printf("EM7180 algorithm slow\n");
    if (em7180.algorithmStatusStillness())               printf("EM7180 in stillness mode\n");
    if (em7180.algorithmStatusMagCalibrationCompleted()) printf("EM7180 mag calibration completed\n");
    if (em7180.algorithmStatusMagneticAnomalyDetected()) printf("EM7180 magnetic anomaly detected\n");
    if (em7180.algorithmStatusUnreliableData())          printf("EM7180 unreliable sensor data\n");

    uint8_t  accelFs=0;
    uint16_t gyroFs=0;
    uint16_t magFs=0;
    em7180.getFullScaleRanges(accelFs, gyroFs, magFs);

    printf("Magnetometer Full Scale Range: +/-%d uT\n", magFs);
    printf("Accelerometer Full Scale Range: +/-%d g\n", accelFs);
    printf("Gyroscope Full Scale Range: +/- %d dps\n", gyroFs);

    static uint32_t count;
    printf("%d ----------------------------------------------\n", count++);

    delay(1000);
}