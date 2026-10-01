/**
 * @file SensorManager.cpp
 * @brief Implementação do SensorManager.
 */

#include "SensorManager.h"

SensorManager::SensorManager()
    : m_simulationActive(ENABLE_MOCK_TELEMETRY),
      m_lastUpdateMs(0) {
}

void SensorManager::init() {
    // Inicialização individual dos módulos
    // Cada módulo deve lidar de forma limpa caso o hardware não esteja presente
    m_imu.init();
    m_barometer.init();
    m_gps.init();
    m_battery.init();
}

void SensorManager::update(BikeState& state) {
    uint32_t now = millis();
    if (now - m_lastUpdateMs < SENSORS_UPDATE_INTERVAL_MS) {
        return; // Taxa de amostragem controlada
    }
    m_lastUpdateMs = now;

    // Atualiza leituras
    m_imu.update(state.motion, m_simulationActive);
    m_barometer.update(state.altitude, m_simulationActive);
    m_gps.update(state.navigation, m_simulationActive);
    m_battery.update(state.battery, m_simulationActive);

    state.isSimulated = m_simulationActive;
}
