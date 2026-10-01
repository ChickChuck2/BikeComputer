/**
 * @file Diagnostics.cpp
 * @brief Implementação dos utilitários de diagnóstico e medição de recursos.
 */

#include "Diagnostics.h"

// Variáveis estáticas de cálculo de FPS
uint32_t Diagnostics::s_lastFpsTime = 0;
uint16_t Diagnostics::s_frameCounter = 0;
uint8_t  Diagnostics::s_currentFps = 0;

#if defined(__AVR__)
extern int __heap_start, *__brkval;
#endif

void Diagnostics::init() {
    s_lastFpsTime = millis();
    s_frameCounter = 0;
    s_currentFps = 0;
}

uint16_t Diagnostics::getFreeRam() {
#if defined(__AVR__)
    int v;
    return (uint16_t)((int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval));
#elif defined(ESP32)
    return (uint16_t)(esp_get_free_heap_size() > 65535 ? 65535 : esp_get_free_heap_size());
#else
    return 0;
#endif
}

bool Diagnostics::pingI2CDevice(uint8_t address) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    return (error == 0);
}

void Diagnostics::formatUptime(char* buffer, size_t bufferSize, uint32_t totalSeconds) {
    if (buffer == nullptr || bufferSize < 9) return;

    uint32_t hours = totalSeconds / 3600;
    uint8_t minutes = (totalSeconds % 3600) / 60;
    uint8_t seconds = totalSeconds % 60;

    // Se as horas ultrapassarem 99, capamos em 99 para caber em 8 caracteres "HH:MM:SS"
    if (hours > 99) hours = 99;

    buffer[0] = '0' + (hours / 10);
    buffer[1] = '0' + (hours % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (minutes / 10);
    buffer[4] = '0' + (minutes % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (seconds / 10);
    buffer[7] = '0' + (seconds % 10);
    buffer[8] = '\0';
}

const __FlashStringHelper* Diagnostics::statusToString(SystemStatus status) {
    switch (status) {
        case STATUS_READY:           return F("OK");
        case STATUS_NOT_INSTALLED:   return F("N/INST");
        case STATUS_NOT_INITIALIZED: return F("INIT");
        case STATUS_ERROR:           return F("ERR");
        case STATUS_DISABLED:        return F("DIS");
        case STATUS_SIMULATED:       return F("SIM");
        default:                     return F("UNK");
    }
}

void Diagnostics::registerFrameRendered() {
    s_frameCounter++;
    uint32_t now = millis();
    if (now - s_lastFpsTime >= 1000) {
        s_currentFps = s_frameCounter;
        s_frameCounter = 0;
        s_lastFpsTime = now;
    }
}

uint8_t Diagnostics::getCalculatedFps() {
    return s_currentFps;
}
