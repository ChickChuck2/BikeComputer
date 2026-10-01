/**
 * @file BluetoothBridge.cpp
 * @brief Implementação da ponte BLE Nordic UART para recepção do GPS do smartphone.
 */

#include "BluetoothBridge.h"
#include <string.h>
#include <stdlib.h>

#if IS_ESP32

BluetoothBridge::BluetoothBridge()
    : m_pServer(nullptr),
      m_pTxCharacteristic(nullptr),
      m_deviceConnected(false),
      m_oldDeviceConnected(false),
      m_lastPacketTimeMs(0),
      m_newPacketReceived(false),
      m_rxSpeedKmh(0.0f),
      m_rxAltitudeM(0.0f),
      m_rxDistanceKm(0.0f),
      m_rxHasFix(false),
      m_rxBufIdx(0) {
    memset(m_rxBuffer, 0, sizeof(m_rxBuffer));
}

bool BluetoothBridge::init() {
    BLEDevice::init("BikeComputer");

    // Cria o Servidor BLE
    m_pServer = BLEDevice::createServer();
    m_pServer->setCallbacks(this);

    // Cria o Serviço Nordic UART (NUS)
    BLEService* pService = m_pServer->createService(NUS_SERVICE_UUID);

    // Cria a Característica TX (ESP32 envia dados/status de volta para o celular)
    m_pTxCharacteristic = pService->createCharacteristic(
        NUS_CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    m_pTxCharacteristic->addDescriptor(new BLE2902());

    // Cria a Característica RX (Celular escreve a telemetria GPS aqui)
    BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
        NUS_CHAR_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
    );
    pRxCharacteristic->setCallbacks(this);

    // Inicia o serviço
    pService->start();

    // Inicia anúncio BLE (Advertising)
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(NUS_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // Funções de ajuda para estabilidade de conexão no Android
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    return true;
}

void BluetoothBridge::onConnect(BLEServer* pServer) {
    m_deviceConnected = true;
}

void BluetoothBridge::onDisconnect(BLEServer* pServer) {
    m_deviceConnected = false;
}

void BluetoothBridge::onWrite(BLECharacteristic* pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
        parseTelemetryPacket(rxValue.c_str());
    }
}

/**
 * @brief Decodifica pacote de telemetria enviado pelo Web App do smartphone.
 * Formato esperado: "V,<speed_kmh>,<alt_m>,<dist_km>,<has_fix>"
 * Exemplo: "V,28.4,752.1,5.32,1"
 */
void BluetoothBridge::parseTelemetryPacket(const char* data) {
    if (data == nullptr || data[0] != 'V' || data[1] != ',') {
        return;
    }

    // Ponteiro após "V,"
    const char* p = data + 2;

    // 1. Velocidade (km/h)
    char* endPtr = nullptr;
    float spd = strtof(p, &endPtr);
    if (endPtr == p || *endPtr != ',') return;
    p = endPtr + 1;

    // 2. Altitude (metros)
    float alt = strtof(p, &endPtr);
    if (endPtr == p || *endPtr != ',') return;
    p = endPtr + 1;

    // 3. Distância (km)
    float dist = strtof(p, &endPtr);
    if (endPtr == p || *endPtr != ',') return;
    p = endPtr + 1;

    // 4. GPS Fix (1 = Fix, 0 = Buscando)
    long fix = strtol(p, &endPtr, 10);

    if (spd < 0.0f) spd = 0.0f;
    if (dist < 0.0f) dist = 0.0f;

    m_rxSpeedKmh = spd;
    m_rxAltitudeM = alt;
    m_rxDistanceKm = dist;
    m_rxHasFix = (fix == 1);
    m_lastPacketTimeMs = millis();
    m_newPacketReceived = true;
}

void BluetoothBridge::update(BikeState& state) {
    uint32_t now = millis();

    state.bleConnected = m_deviceConnected;
    state.diagnostics.bleStatus = m_deviceConnected ? STATUS_READY : STATUS_NOT_INITIALIZED;

    // Tratamento de reconexão do anúncio quando o celular desconectar
    if (!m_deviceConnected && m_oldDeviceConnected) {
        delay(20); // Pequeno atraso para a stack BLE estabilizar
        m_pServer->startAdvertising();
        m_oldDeviceConnected = m_deviceConnected;

        // Limpa dados de velocidade ao desconectar
        state.navigation.speedKmh = 0.0f;
        state.navigation.hasFix = false;
        state.navigation.status = STATUS_NOT_INITIALIZED;
    }

    if (m_deviceConnected && !m_oldDeviceConnected) {
        m_oldDeviceConnected = m_deviceConnected;
    }

    if (m_deviceConnected) {
        state.navigation.status = STATUS_READY;

        if (m_newPacketReceived) {
            m_newPacketReceived = false;

            state.navigation.speedKmh = m_rxSpeedKmh;
            state.navigation.distanceKm = m_rxDistanceKm;
            state.navigation.hasFix = m_rxHasFix;
            state.altitude.currentAltM = m_rxAltitudeM;
            state.altitude.status = STATUS_READY;

            // Atualiza velocidade máxima
            if (m_rxSpeedKmh > state.navigation.maxSpeedKmh) {
                state.navigation.maxSpeedKmh = m_rxSpeedKmh;
            }

            // Desativa qualquer flag de simulação
            state.isSimulated = false;
        }

        // Se o celular parar de enviar pacotes por mais de 3.5 segundos (ex: app em background suspenso)
        if (now - m_lastPacketTimeMs > 3500) {
            state.navigation.speedKmh = 0.0f;
        }
    } else {
        // Desconectado: velocidade zero garantida
        state.navigation.speedKmh = 0.0f;
    }
}

#endif // IS_ESP32
