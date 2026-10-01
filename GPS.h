/**
 * @file GPS.h
 * @brief Interface para receptor GNSS/GPS (Futuro: NMEA via UART).
 * 
 * No hardware atual (Arduino UNO), este sensor não está fisicamente instalado.
 * Permanece como NOT_INSTALLED ou com simulação explícita (MOCK).
 */

#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"

class GPS {
public:
    GPS() : m_status(STATUS_NOT_INSTALLED) {}

    bool init() {
        m_status = STATUS_NOT_INSTALLED;
        return false;
    }

    void update(NavigationData& data, bool enableSimulation = false) {
        if (enableSimulation) {
            m_status = STATUS_SIMULATED;
            uint32_t t = millis();
            // Simula uma pedalada a ~24.5 km/h variando suavemente
            data.speedKmh = 23.5f + sin(t * 0.001f) * 4.2f;
            if (data.speedKmh < 0.0f) data.speedKmh = 0.0f;
            data.avgSpeedKmh = 21.8f;
            data.maxSpeedKmh = 34.2f;
            data.distanceKm = 12.45f + (t / 100000.0f);
            data.satellites = 9;
            data.hasFix = true;
            data.latitudeE7 = -235505200; // Ex: São Paulo
            data.longitudeE7 = -466333080;
            data.status = STATUS_SIMULATED;
        } else {
            m_status = STATUS_NOT_INSTALLED;
            data.status = STATUS_NOT_INSTALLED;
            data.speedKmh = 0.0f;
            data.avgSpeedKmh = 0.0f;
            data.maxSpeedKmh = 0.0f;
            data.distanceKm = 0.0f;
            data.satellites = 0;
            data.hasFix = false;
            data.latitudeE7 = 0;
            data.longitudeE7 = 0;
        }
    }

    SystemStatus getStatus() const { return m_status; }

private:
    SystemStatus m_status;
};

#endif // GPS_H
