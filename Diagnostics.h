/**
 * @file Diagnostics.h
 * @brief Utilitários de diagnóstico para monitoramento de saúde do sistema.
 */

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"
#include "BikeState.h"

class Diagnostics {
public:
    static void init();
    
    /**
     * @brief Retorna a quantidade de bytes de SRAM livres no microcontrolador.
     */
    static uint16_t getFreeRam();

    /**
     * @brief Testa se um endereço I2C responde no barramento.
     * @param address Endereço I2C (ex: 0x3C)
     * @return true se recebeu ACK (0), false se NACK ou erro de barramento
     */
    static bool pingI2CDevice(uint8_t address);

    /**
     * @brief Formata o tempo de uptime em formato HH:MM:SS sem alocação dinâmica.
     * @param buffer Ponteiro para array char de pelo menos 9 bytes ("HH:MM:SS\0")
     * @param bufferSize Tamanho do buffer
     * @param totalSeconds Segundos totais de uptime
     */
    static void formatUptime(char* buffer, size_t bufferSize, uint32_t totalSeconds);

    /**
     * @brief Converte um SystemStatus para uma string armazenada na Flash (economiza SRAM).
     */
    static const __FlashStringHelper* statusToString(SystemStatus status);

    /**
     * @brief Atualiza a contagem e cálculo de FPS do renderizador.
     */
    static void registerFrameRendered();
    static uint8_t getCalculatedFps();

private:
    static uint32_t s_lastFpsTime;
    static uint16_t s_frameCounter;
    static uint8_t  s_currentFps;
};

#endif // DIAGNOSTICS_H
