/**
 * @file DisplayManager.cpp
 * @brief Implementação do DisplayManager com U8g2.
 */

#include "DisplayManager.h"
#include "Diagnostics.h"

DisplayManager::DisplayManager()
    : m_u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE),
      m_status(STATUS_NOT_INITIALIZED) {
}

bool DisplayManager::init() {
#if IS_ESP32
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
#else
    Wire.begin();
#endif
    Wire.setClock(I2C_CLOCK_SPEED);

    // Verifica se o display SSD1306 responde no barramento I2C
    bool detected = Diagnostics::pingI2CDevice(DISPLAY_I2C_ADDR);
    
    if (detected) {
        m_u8g2.begin();
        m_u8g2.clearBuffer();
        m_u8g2.setFontMode(0); // Não transparente (fundo preto sólido)
        m_status = STATUS_READY;
        return true;
    } else {
        m_status = STATUS_ERROR;
        // Inicializa mesmo assim para evitar null-pointers internos caso o barramento acorde
        m_u8g2.begin();
        return false;
    }
}

void DisplayManager::clearBuffer() {
    m_u8g2.clearBuffer();
}

void DisplayManager::sendBuffer() {
    m_u8g2.sendBuffer();
    Diagnostics::registerFrameRendered();
}

void DisplayManager::drawHeader(const __FlashStringHelper* title, const __FlashStringHelper* rightTag) {
    m_u8g2.setFont(u8g2_font_6x10_tf);
    
    // Título à esquerda
    m_u8g2.setCursor(2, 9);
    if (title != nullptr) {
        m_u8g2.print(title);
    }

    // Tag à direita (ex: TEST, SIM, SYS)
    if (rightTag != nullptr) {
        char tagBuf[12];
        strncpy_P(tagBuf, reinterpret_cast<PGM_P>(rightTag), sizeof(tagBuf) - 1);
        tagBuf[sizeof(tagBuf) - 1] = '\0';
        int16_t tagWidth = m_u8g2.getStrWidth(tagBuf);
        m_u8g2.setCursor(DISPLAY_WIDTH - tagWidth - 2, 9);
        m_u8g2.print(tagBuf);
    }

    // Linha divisória abaixo do cabeçalho
    drawDivider(12);
}

void DisplayManager::drawDivider(int16_t y) {
    m_u8g2.drawHLine(0, y, DISPLAY_WIDTH);
}
