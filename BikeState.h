/**
 * @file BikeState.h
 * @brief Estruturas de dados consolidadas de telemetria e estado do sistema.
 * 
 * Desacopla a camada de aquisição (sensores) da camada de apresentação (UI)
 * e telemetria (registro/comunicação).
 */

#ifndef BIKE_STATE_H
#define BIKE_STATE_H

#include <Arduino.h>

/**
 * @brief Estados possíveis de cada subsistema/sensor.
 */
enum SystemStatus : uint8_t {
    STATUS_NOT_INSTALLED   = 0, // Hardware ausente no estágio atual
    STATUS_NOT_INITIALIZED = 1, // Presente mas aguardando init
    STATUS_READY           = 2, // Operando normalmente
    STATUS_ERROR           = 3, // Falha de comunicação/leitura
    STATUS_DISABLED        = 4, // Desabilitado por configuração
    STATUS_SIMULATED       = 5  // Dados gerados por simulação (Mock)
};

/**
 * @brief Telemetria de Movimento (IMU - BMI270)
 */
struct MotionData {
    int16_t accelX;       // mG (mili-g)
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;        // dps (graus por segundo)
    int16_t gyroY;
    int16_t gyroZ;
    float pitchAngle;     // Graus estimados
    float rollAngle;
    SystemStatus status;
};

/**
 * @brief Telemetria de Navegação (GPS)
 */
struct NavigationData {
    float speedKmh;       // Velocidade atual
    float avgSpeedKmh;    // Velocidade média
    float maxSpeedKmh;    // Velocidade máxima
    float distanceKm;     // Odômetro da viagem
    int32_t latitudeE7;   // Lat * 1e7 para evitar perda de precisão float
    int32_t longitudeE7;  // Lon * 1e7
    uint8_t satellites;   // Número de satélites visíveis
    bool hasFix;          // Status de sincronismo 3D
    SystemStatus status;
};

/**
 * @brief Telemetria Ambiental e Altitude (Barômetro - BMP280/BMP388)
 */
struct AltitudeData {
    float pressureHPa;    // Pressão atmosférica (hPa)
    float currentAltM;    // Altitude atual (metros)
    float elevationGainM; // Ganho acumulado de elevação
    float tempC;          // Temperatura ambiente (°C)
    SystemStatus status;
};

/**
 * @brief Telemetria de Bateria e Alimentação
 */
struct BatteryData {
    uint16_t voltageMv;   // Tensão em milivolts
    uint8_t percent;      // Estimativa percentual (0-100%)
    bool isCharging;
    SystemStatus status;
};

/**
 * @brief Diagnóstico e métricas internas do sistema
 */
struct SystemDiagnostics {
    uint32_t uptimeSeconds;
    uint16_t freeRamBytes;
    uint8_t currentFps;
    uint8_t i2cErrorCount;
    SystemStatus i2cBusStatus;
    SystemStatus displayStatus;
    SystemStatus bleStatus;
};

/**
 * @brief Estado consolidado do computador de bordo
 */
struct BikeState {
    MotionData motion;
    NavigationData navigation;
    AltitudeData altitude;
    BatteryData battery;
    SystemDiagnostics diagnostics;
    bool isSimulated;     // Flag global indicando se dados da UI são simulados
    bool bleConnected;    // Conexão BLE com o smartphone ativa
};

#endif // BIKE_STATE_H
