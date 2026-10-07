/**
 * @file BluetoothBridge.cpp
 * @brief Implementação de recepção Bluetooth Serial (SPP) para Android.
 */

#include "BluetoothBridge.h"
#include <string.h>
#include <stdlib.h>

#if IS_ESP32

BluetoothBridge::BluetoothBridge()
    : m_connected(false),
      m_lastPacketTimeMs(0),
      m_rxSpeedKmh(0.0f),
      m_rxAltitudeM(0.0f),
      m_rxDistanceKm(0.0f),
      m_rxHasFix(false),
      m_rxBufIdx(0) {
    memset(m_rxBuffer, 0, sizeof(m_rxBuffer));
}

bool BluetoothBridge::init() {
    // Inicializa como dispositivo Bluetooth Classic SPP
    m_serialBT.begin("BikeComputer");
    return true;
}

bool BluetoothBridge::isConnected() const {
    return const_cast<BluetoothSerial&>(m_serialBT).hasClient();
}

void BluetoothBridge::update(BikeState& state) {
    bool clientConnected = m_serialBT.hasClient();
    state.bleConnected = clientConnected;
    state.diagnostics.bleStatus = clientConnected ? STATUS_READY : STATUS_NOT_INITIALIZED;

    uint32_t now = millis();

    // Lê stream de dados da serial Bluetooth
    while (m_serialBT.available()) {
        char c = (char)m_serialBT.read();
        if (c == '\n' || c == '\r') {
            if (m_rxBufIdx > 0) {
                m_rxBuffer[m_rxBufIdx] = '\0';
                parseTelemetryPacket(m_rxBuffer);
                m_rxBufIdx = 0;
            }
        } else if (m_rxBufIdx < sizeof(m_rxBuffer) - 1) {
            m_rxBuffer[m_rxBufIdx++] = c;
        }
    }

    if (clientConnected) {
        state.navigation.status = STATUS_READY;
        state.navigation.speedKmh = m_rxSpeedKmh;
        state.navigation.distanceKm = m_rxDistanceKm;
        state.navigation.hasFix = m_rxHasFix;
        state.altitude.currentAltM = m_rxAltitudeM;
        state.altitude.status = STATUS_READY;
        state.isSimulated = false;

        // Atualiza velocidade máxima
        if (m_rxSpeedKmh > state.navigation.maxSpeedKmh) {
            state.navigation.maxSpeedKmh = m_rxSpeedKmh;
        }

        // Se o celular parar de transmitir por mais de 4 segundos, zera velocidade
        if (now - m_lastPacketTimeMs > 4000) {
            state.navigation.speedKmh = 0.0f;
        }
    } else {
        state.navigation.speedKmh = 0.0f;
        m_rxSpeedKmh = 0.0f;
    }
}

void BluetoothBridge::parseTelemetryPacket(const char* data) {
    if (data == nullptr || data[0] != 'V' || data[1] != ',') {
        return;
    }

    const char* p = data + 2;
    char* endPtr = nullptr;

    // 1. Velocidade (km/h)
    float spd = strtof(p, &endPtr);
    if (endPtr == p || *endPtr != ',') return;
    p = endPtr + 1;

    // 2. Altitude (m)
    float alt = strtof(p, &endPtr);
    if (endPtr == p || *endPtr != ',') return;
    p = endPtr + 1;

    // 3. Distância (km)
    float dist = strtof(p, &endPtr);
    if (endPtr == p || *endPtr != ',') return;
    p = endPtr + 1;

    // 4. Fix (1 ou 0)
    long fix = strtol(p, &endPtr, 10);

    if (spd < 0.0f) spd = 0.0f;
    if (dist < 0.0f) dist = 0.0f;

    m_rxSpeedKmh = spd;
    m_rxAltitudeM = alt;
    m_rxDistanceKm = dist;
    m_rxHasFix = (fix == 1);
    m_lastPacketTimeMs = millis();
}

#endif // IS_ESP32
