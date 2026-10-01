/**
 * @file SystemManager.h
 * @brief Orquestrador central do ciclo de vida do sistema embarcado.
 */

#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"
#include "Diagnostics.h"
#include "DisplayManager.h"
#include "SensorManager.h"
#include "UIManager.h"
#include "BluetoothBridge.h"

class SystemManager {
public:
    SystemManager();

    /**
     * @brief Executado uma única vez na inicialização do microcontrolador (setup).
     */
    void init();

    /**
     * @brief Executado continuamente no loop principal sem bloqueios (loop).
     */
    void update();

    const BikeState& getState() const { return m_state; }

private:
    void updateSystemMetrics();
    void printBootLog();

    BikeState m_state;
    DisplayManager m_display;
    SensorManager m_sensors;
    UIManager m_ui;
    BluetoothBridge m_ble;
    uint32_t m_lastMetricsUpdateMs;
};

#endif // SYSTEM_MANAGER_H
