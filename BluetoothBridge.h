/**
 * @file BluetoothBridge.h
 * @brief Gerenciador de conexão Bluetooth Serial (Classic SPP) para comunicação com smartphone Android.
 */

#ifndef BLUETOOTH_BRIDGE_H
#define BLUETOOTH_BRIDGE_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"

#if IS_ESP32
#include "BluetoothSerial.h"

class BluetoothBridge {
public:
    BluetoothBridge();

    /**
     * @brief Inicializa o Bluetooth Serial com o nome "BikeComputer".
     */
    bool init();

    /**
     * @brief Processa dados recebidos da porta serial Bluetooth e atualiza a telemetria.
     */
    void update(BikeState& state);

    /**
     * @brief Retorna se há um cliente (celular) conectado via Bluetooth.
     */
    bool isConnected() const;

private:
    BluetoothSerial m_serialBT;
    bool m_connected;
    uint32_t m_lastPacketTimeMs;

    float m_rxSpeedKmh;
    float m_rxAltitudeM;
    float m_rxDistanceKm;
    bool  m_rxHasFix;
    char  m_rxBuffer[64];
    uint8_t m_rxBufIdx;

    void parseTelemetryPacket(const char* data);
};

#else

class BluetoothBridge {
public:
    BluetoothBridge() {}
    bool init() { return false; }
    void update(BikeState& state) {}
    bool isConnected() const { return false; }
};

#endif // IS_ESP32

#endif // BLUETOOTH_BRIDGE_H
