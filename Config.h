/**
 * @file Config.h
 * @brief Configurações globais de hardware, tempos e flags de compilação.
 * 
 * Projeto: Computador de Bordo para Bicicleta (Bike Computer)
 * Alvo Atual: Arduino UNO (ATmega328P) + SSD1306 I2C (128x64)
 * Alvo Futuro: ESP32 + Sensores I2C/SPI + GPS UART + SD SPI + BT/WiFi
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================================
// METADADOS DO FIRMWARE
// =============================================================================
#define FW_NAME             "BIKE-COMP"
#define FW_VERSION          "0.1.0-alpha"
#define FW_BUILD_TARGET     "UNO-SSD1306"

// =============================================================================
// DETECÇÃO AUTOMÁTICA DE PLATAFORMA (MCU)
// =============================================================================
#if defined(__AVR_ATmega328P__)
    #define TARGET_MCU_NAME  "ATmega328P"
    #define IS_AVR_UNO       1
    #define IS_ESP32         0
    #define IS_ESP32_S3      0
#elif defined(CONFIG_IDF_TARGET_ESP32S3) || defined(ARDUINO_ESP32S3_DEV) || defined(ESP32S3)
    #define TARGET_MCU_NAME  "ESP32-S3"
    #define IS_AVR_UNO       0
    #define IS_ESP32         1
    #define IS_ESP32_S3      1
#elif defined(ESP32)
    #define TARGET_MCU_NAME  "ESP32"
    #define IS_AVR_UNO       0
    #define IS_ESP32         1
    #define IS_ESP32_S3      0
#else
    #define TARGET_MCU_NAME  "GENERIC_MCU"
    #define IS_AVR_UNO       0
    #define IS_ESP32         0
    #define IS_ESP32_S3      0
#endif

// =============================================================================
// HARDWARE - DISPLAY OLED (SSD1306 128x64 I2C)
// =============================================================================
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64
#define DISPLAY_I2C_ADDR    0x3C // Endereço I2C comum (0x3C ou 0x3D)

// Pinos I2C de referência (Hardware I2C padrão)
// Arduino UNO: SDA = A4, SCL = A5
// ESP32 Clássico: SDA = GPIO 21, SCL = GPIO 22
// ESP32-S3: padrão da placa (SDA/SCL) ou GPIO 8 / 9
#if IS_AVR_UNO
    #define PIN_I2C_SDA     A4
    #define PIN_I2C_SCL     A5
#elif IS_ESP32_S3
    #if defined(SDA) && defined(SCL)
        #define PIN_I2C_SDA SDA
        #define PIN_I2C_SCL SCL
    #else
        #define PIN_I2C_SDA 8
        #define PIN_I2C_SCL 9
    #endif
#elif IS_ESP32
    #define PIN_I2C_SDA     21
    #define PIN_I2C_SCL     22
#endif

// Clock I2C: 400kHz (Fast Mode) é suportado pelo SSD1306 e reduz tempo no barramento
#define I2C_CLOCK_SPEED     400000UL

// =============================================================================
// TEMPORIZAÇÃO E TAXAS DE ATUALIZAÇÃO (Sem delay())
// =============================================================================
// Taxa de atualização do display (FPS).
#if IS_ESP32
    #define UI_TARGET_FPS       25
#else
    #define UI_TARGET_FPS       12
#endif
#define UI_REFRESH_INTERVAL_MS (1000 / UI_TARGET_FPS)

// Taxa de amostragem de sensores (quando implementados)
#define SENSORS_UPDATE_INTERVAL_MS 100 // 10 Hz

// Duração da tela de Splash/Boot (milissegundos)
#define BOOT_SCREEN_DURATION_MS 2500

// Modo de demonstração sem botões físicos:
// Como ainda não há botões instalados, alterna automaticamente entre telas
// para visualização de diagnóstico e tela principal.
#define UI_AUTO_CYCLE_SCREENS   1
#define UI_AUTO_CYCLE_INTERVAL_MS 6000 // Alterna a cada 6 segundos

// =============================================================================
// CONTROLE DE MÓDULOS E SIMULAÇÃO
// =============================================================================
// 0 = Sensores desabilitados (retornam STATUS_NOT_INSTALLED ou aguardam dados reais)
// 1 = Gera dados fictícios identificados obrigatoriamente como "SIMULATION / MOCK"
#define ENABLE_MOCK_TELEMETRY   0

// Serial Debug (Desabilitado no UNO para economizar ~160 bytes de SRAM essenciais para a pilha)
#if IS_AVR_UNO
    #define SERIAL_DEBUG_ENABLED    0
#else
    #define SERIAL_DEBUG_ENABLED    1
#endif
#define SERIAL_BAUD_RATE        115200

#endif // CONFIG_H
