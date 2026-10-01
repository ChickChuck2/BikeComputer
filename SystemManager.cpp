/**
 * @file SystemManager.cpp
 * @brief Implementação do orquestrador do sistema.
 */

#include "SystemManager.h"

SystemManager::SystemManager()
    : m_display(),
      m_sensors(),
      m_ui(m_display),
      m_lastMetricsUpdateMs(0) {
    memset(&m_state, 0, sizeof(BikeState));
}

void SystemManager::init() {
#if SERIAL_DEBUG_ENABLED
    Serial.begin(SERIAL_BAUD_RATE);
#endif

    Diagnostics::init();

    // 1. Inicializa o subsistema de vídeo OLED
    bool displayOk = m_display.init();
    m_state.diagnostics.displayStatus = displayOk ? STATUS_READY : STATUS_ERROR;
    m_state.diagnostics.i2cBusStatus = displayOk ? STATUS_READY : STATUS_ERROR;

    // 2. Inicializa o subsistema de sensores
    m_sensors.init();

    // 3. Inicializa o subsistema de interface gráfica
    m_ui.init();

    // 4. Inicializa o rádio BLE para conexão com o smartphone
    m_ble.init();

    // 5. Medição inicial de recursos
    updateSystemMetrics();

    // 6. Emite log de boot seguro
    printBootLog();
}

void SystemManager::update() {
    uint32_t now = millis();

    // Atualiza diagnósticos internos a cada 250 ms
    if (now - m_lastMetricsUpdateMs >= 250) {
        m_lastMetricsUpdateMs = now;
        updateSystemMetrics();
    }

    // Atualiza sensores físicos/mock
    m_sensors.update(m_state);

    // Atualiza dados recebidos via Bluetooth (GPS do smartphone)
    m_ble.update(m_state);

    // Atualiza UI e renderiza display (taxa controlada internamente no UIManager)
    m_ui.update(m_state);
}

void SystemManager::updateSystemMetrics() {
    m_state.diagnostics.uptimeSeconds = millis() / 1000UL;
    m_state.diagnostics.freeRamBytes = Diagnostics::getFreeRam();
    m_state.diagnostics.currentFps = Diagnostics::getCalculatedFps();
    m_state.diagnostics.displayStatus = m_display.getStatus();
}

void SystemManager::printBootLog() {
#if SERIAL_DEBUG_ENABLED
    Serial.println(F("\n====================================="));
    Serial.print(F("FIRMWARE: "));
    Serial.print(F(FW_NAME));
    Serial.print(F(" v"));
    Serial.println(F(FW_VERSION));
    Serial.print(F("TARGET MCU: "));
    Serial.println(F(TARGET_MCU_NAME));
    Serial.print(F("DISPLAY: SSD1306 (0x3C) -> "));
    Serial.println(m_state.diagnostics.displayStatus == STATUS_READY ? F("OK") : F("FAILED"));
    Serial.print(F("FREE RAM: "));
    Serial.print(m_state.diagnostics.freeRamBytes);
    Serial.println(F(" bytes"));
    Serial.print(F("SIMULATION: "));
    Serial.println(m_state.isSimulated ? F("ENABLED (MOCK)") : F("DISABLED"));
    Serial.println(F("=====================================\n"));
#endif
}
