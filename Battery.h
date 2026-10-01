/**
 * @file Battery.h
 * @brief Interface para monitoramento de alimentação e bateria.
 */

#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"

class Battery {
public:
    Battery() : m_status(STATUS_NOT_INSTALLED) {}

    bool init() {
        m_status = STATUS_NOT_INSTALLED;
        return false;
    }

    void update(BatteryData& data, bool enableSimulation = false) {
        if (enableSimulation) {
            m_status = STATUS_SIMULATED;
            data.voltageMv = 3850; // 3.85V (Lipo ~75%)
            data.percent = 78;
            data.isCharging = false;
            data.status = STATUS_SIMULATED;
        } else {
            m_status = STATUS_NOT_INSTALLED;
            data.status = STATUS_NOT_INSTALLED;
            data.voltageMv = 5000; // Alimentação USB padrão do UNO
            data.percent = 100;
            data.isCharging = true;
        }
    }

    SystemStatus getStatus() const { return m_status; }

private:
    SystemStatus m_status;
};

#endif // BATTERY_H
