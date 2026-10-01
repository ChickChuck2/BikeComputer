/**
 * @file IMU.h
 * @brief Interface para sensor IMU (Futuro: Bosch BMI270 - Acelerômetro + Giroscópio).
 * 
 * No hardware atual (Arduino UNO), este sensor não está fisicamente instalado.
 * Permanece como NOT_INSTALLED ou com simulação explícita (MOCK).
 */

#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"

class IMU {
public:
    IMU() : m_status(STATUS_NOT_INSTALLED) {}

    bool init() {
        // Hardware ausente no Arduino UNO
        m_status = STATUS_NOT_INSTALLED;
        return false;
    }

    void update(MotionData& data, bool enableSimulation = false) {
        if (enableSimulation) {
            m_status = STATUS_SIMULATED;
            // Gera oscilação senoidal suave para teste de animação/UI
            uint32_t t = millis();
            data.accelX = (int16_t)(sin(t * 0.002f) * 150.0f);
            data.accelY = (int16_t)(cos(t * 0.002f) * 120.0f);
            data.accelZ = 1000; // ~1G estático
            data.gyroX = 0;
            data.gyroY = 0;
            data.gyroZ = (int16_t)(sin(t * 0.003f) * 20.0f);
            data.pitchAngle = sin(t * 0.001f) * 5.0f;
            data.rollAngle = cos(t * 0.001f) * 3.0f;
            data.status = STATUS_SIMULATED;
        } else {
            m_status = STATUS_NOT_INSTALLED;
            data.status = STATUS_NOT_INSTALLED;
            data.accelX = data.accelY = data.accelZ = 0;
            data.gyroX = data.gyroY = data.gyroZ = 0;
            data.pitchAngle = data.rollAngle = 0.0f;
        }
    }

    SystemStatus getStatus() const { return m_status; }

private:
    SystemStatus m_status;
};

#endif // IMU_H
