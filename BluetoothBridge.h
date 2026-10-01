/**
 * @file BluetoothBridge.h
 * @brief Gerenciador de conexão BLE (Bluetooth Low Energy) via Web Bluetooth (Nordic UART Service).
 * 
 * Permite que um smartphone (via Chrome / Web Bluetooth) atue como sensor GPS,
 * transmitindo velocidade, altitude, distância e odômetro em tempo real para o ESP32.
 */

#ifndef BLUETOOTH_BRIDGE_H
#define BLUETOOTH_BRIDGE_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"

#if IS_ESP32
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Nordic UART Service (NUS) UUIDs (Padrão mundial para Web Bluetooth)
#define NUS_SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_CHAR_RX_UUID           "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // Smartphone escreve aqui
#define NUS_CHAR_TX_UUID           "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // ESP32 notifica smartphone

class BluetoothBridge : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    BluetoothBridge();

    /**
     * @brief Inicializa o rádio BLE e inicia anúncio com o nome "BikeComputer".
     */
    bool init();

    /**
     * @brief Atualiza o estado da telemetria com base nos pacotes recebidos do smartphone.
     * @param state Estrutura central do BikeState.
     */
    void update(BikeState& state);

    /**
     * @brief Retorna se o smartphone está conectado ao BLE.
     */
    bool isConnected() const { return m_deviceConnected; }

    // Callbacks do Servidor BLE
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;

    // Callbacks de recepção de dados (Nordic UART RX)
    void onWrite(BLECharacteristic* pCharacteristic) override;

private:
    BLEServer* m_pServer;
    BLECharacteristic* m_pTxCharacteristic;
    bool m_deviceConnected;
    bool m_oldDeviceConnected;
    uint32_t m_lastPacketTimeMs;

    // Buffer de dados recebidos
    volatile bool m_newPacketReceived;
    float m_rxSpeedKmh;
    float m_rxAltitudeM;
    float m_rxDistanceKm;
    bool  m_rxHasFix;
    char  m_rxBuffer[64];
    uint8_t m_rxBufIdx;

    void parseTelemetryPacket(const char* data);
};

#else

// Mock para plataformas sem BLE nativo (ex: Arduino UNO)
class BluetoothBridge {
public:
    BluetoothBridge() {}
    bool init() { return false; }
    void update(BikeState& state) {}
    bool isConnected() const { return false; }
};

#endif // IS_ESP32

#endif // BLUETOOTH_BRIDGE_H
