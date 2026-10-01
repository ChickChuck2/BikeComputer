/**
 * @file DisplayManager.h
 * @brief Gerenciador e encapsulamento do display OLED SSD1306 via U8g2.
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "Config.h"
#include "BikeState.h"

class DisplayManager {
public:
    DisplayManager();

    /**
     * @brief Inicializa o barramento I2C e a controladora SSD1306.
     * @return true se o display respondeu no barramento I2C com sucesso.
     */
    bool init();

    /**
     * @brief Limpa o buffer interno da U8g2.
     */
    void clearBuffer();

    /**
     * @brief Transfere o buffer da RAM para o display via hardware I2C.
     */
    void sendBuffer();

    /**
     * @brief Acesso à instância direta da U8g2 para renderização gráfica pelas telas.
     */
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C& getU8g2() { return m_u8g2; }

    /**
     * @brief Status atual do display.
     */
    SystemStatus getStatus() const { return m_status; }

    /**
     * @brief Desenha cabeçalho padrão com título e indicador de status.
     * @param title Título da tela armazenado na Flash
     * @param rightTag Tag do canto superior direito (ex: "TEST", "MOCK", "SYS")
     */
    void drawHeader(const __FlashStringHelper* title, const __FlashStringHelper* rightTag = nullptr);

    /**
     * @brief Desenha linha horizontal divisória.
     */
    void drawDivider(int16_t y);

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C m_u8g2;
    SystemStatus m_status;
};

#endif // DISPLAY_MANAGER_H
