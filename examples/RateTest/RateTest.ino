/* 
   RateTest.ino: Example of how to adjust Output Data Rates (ODRs) for SENtral sensor 

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

#ifdef __MK20DX256__
#include <i2c_t3.h>
#define NOSTOP I2C_NOSTOP
#else
#include <Wire.h>
#define NOSTOP false
#endif

EM7180 em7180;

void setup()
{
    // Start I^2C
#ifdef __MK20DX256__
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
#else
    Wire.begin();
#endif

    delay(100);

    Serial.begin(38400);

    // Initialize the EM7180 ODRs
    em7180.accelRate = 200;
    em7180.gyroRate = 200;
    em7180.baroRate = 40;
    em7180.qRateDivisor = 5;

    // Start the EM7180 in master mode
    if (!em7180.begin()) {

        while (true) {
            Serial.println(em7180.getErrorString());
        }
    }
}

static void report(const char * label, uint32_t & count, const char * delim, uint16_t actual=0)
{
    Serial.print(label);
    Serial.print(": ");
    Serial.print(count);
    Serial.print("Hz");

    if (actual > 0) {
        Serial.print(" [");
        Serial.print(actual);
        Serial.print("]");
    }

    Serial.print(delim);

    count = 0;
}

void loop()
{  
    static uint32_t q, a, g, m, b;

    em7180.checkEventStatus();

    if (em7180.gotError()) {
        Serial.print("ERROR: ");
        Serial.println(em7180.getErrorString());
        return;
    }

    if (em7180.gotQuaternions()) {
        float quat[4]; em7180.readQuaternions(quat);
        q++;
    }

    if (em7180.gotAccelerometer()) {
        int16_t accel[3]; em7180.readAccelerometer(accel);
        a++;
    }

    if (em7180.gotGyrometer()) {
        int16_t gyro[3]; em7180.readGyrometer(gyro);
        g++;
    }

    if (em7180.gotMagnetometer()) {
        int16_t mag[3]; em7180.readMagnetometer(mag);
        m++;
    }

    if (em7180.gotBarometer()) {
        float pressure, temperature; em7180.readBarometer(pressure, temperature);
        b++;
    }

    static uint32_t start;

    if (micros() - start > 1e6) {
        b /= 2; // need two cycles for complete baro reading (temperature, pressure)
        report("G", g, "\t", em7180.getActualGyroRate());
        report("A", a, "\t", em7180.getActualAccelRate());
        report("M", m, "\t", em7180.getActualMagRate());
        report("B", b, "\t", em7180.getActualBaroRate());
        report("Q", q, "\n");
        start = micros();
    }
}


