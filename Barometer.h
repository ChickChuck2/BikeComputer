/**
 * @file Barometer.h
 * @brief Interface para sensor Barômetro (Futuro: BMP280 / BMP388).
 * 
 * No hardware atual (Arduino UNO), este sensor não está fisicamente instalado.
 * Permanece como NOT_INSTALLED ou com simulação explícita (MOCK).
 */

#ifndef BAROMETER_H
#define BAROMETER_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"

class Barometer {
public:
    Barometer() : m_status(STATUS_NOT_INSTALLED) {}

    bool init() {
        m_status = STATUS_NOT_INSTALLED;
        return false;
    }

    void update(AltitudeData& data, bool enableSimulation = false) {
        if (enableSimulation) {
            m_status = STATUS_SIMULATED;
            uint32_t t = millis();
            data.pressureHPa = 1013.25f + sin(t * 0.0005f) * 4.0f;
            data.currentAltM = 780.0f + sin(t * 0.001f) * 15.0f;
            data.elevationGainM = 125.0f + (t / 10000.0f);
            data.tempC = 24.5f;
            data.status = STATUS_SIMULATED;
        } else {
            m_status = STATUS_NOT_INSTALLED;
            data.status = STATUS_NOT_INSTALLED;
            data.pressureHPa = 0.0f;
            data.currentAltM = 0.0f;
            data.elevationGainM = 0.0f;
            data.tempC = 0.0f;
        }
    }

    SystemStatus getStatus() const { return m_status; }

private:
    SystemStatus m_status;
};

#endif // BAROMETER_H
