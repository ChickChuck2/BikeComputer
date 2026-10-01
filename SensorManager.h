/**
 * @file SensorManager.h
 * @brief Orquestrador de sensores e consolidador de telemetria.
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"
#include "IMU.h"
#include "Barometer.h"
#include "GPS.h"
#include "Battery.h"

class SensorManager {
public:
    SensorManager();

    /**
     * @brief Inicializa a camada de sensores.
     */
    void init();

    /**
     * @brief Atualiza leituras de sensores (chamado periodicamente sem delay).
     * @param state Referência ao estado consolidado da bicicleta.
     */
    void update(BikeState& state);

    IMU& getIMU() { return m_imu; }
    Barometer& getBarometer() { return m_barometer; }
    GPS& getGPS() { return m_gps; }
    Battery& getBattery() { return m_battery; }

    bool isSimulated() const { return m_simulationActive; }
    void setSimulation(bool enable) { m_simulationActive = enable; }

private:
    IMU m_imu;
    Barometer m_barometer;
    GPS m_gps;
    Battery m_battery;
    bool m_simulationActive;
    uint32_t m_lastUpdateMs;
};

#endif // SENSOR_MANAGER_H
