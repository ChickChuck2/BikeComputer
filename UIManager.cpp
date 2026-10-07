/**
 * @file UIManager.cpp
 * @brief Implementação da máquina de estados de navegação de telas e taxas de atualização.
 */

#include "UIManager.h"

UIManager::UIManager(DisplayManager& display)
    : m_display(display),
      m_currentScreen(SCREEN_BOOT),
      m_lastRenderMs(0),
      m_bootStartMs(0),
      m_lastCycleMs(0) {
}

void UIManager::init() {
    m_bootStartMs = millis();
    m_lastRenderMs = 0;
    m_lastCycleMs = millis();
    m_currentScreen = SCREEN_BOOT;
}

void UIManager::setScreen(ScreenId screen) {
    m_currentScreen = screen;
}

void UIManager::update(const BikeState& state) {
    uint32_t now = millis();

    // 1. Controle de transição da tela de Boot
    if (m_currentScreen == SCREEN_BOOT) {
        uint32_t elapsed = now - m_bootStartMs;
        if (elapsed >= BOOT_SCREEN_DURATION_MS) {
            // Após o boot, fixa diretamente no velocímetro dedicado
            m_currentScreen = SCREEN_SPEEDOMETER;
            m_lastCycleMs = now;
        }
    } 
    // 2. Alternância automática de demonstração (quando configurada e sem botões)
    else if (UI_AUTO_CYCLE_SCREENS) {
        if (now - m_lastCycleMs >= UI_AUTO_CYCLE_INTERVAL_MS) {
            m_lastCycleMs = now;
            // Ciclo: DIAGNOSTICS → SPEEDOMETER → HOME → DIAGNOSTICS
            if (m_currentScreen == SCREEN_DIAGNOSTICS) {
                m_currentScreen = SCREEN_SPEEDOMETER;
            } else if (m_currentScreen == SCREEN_SPEEDOMETER) {
                m_currentScreen = SCREEN_HOME;
            } else {
                m_currentScreen = SCREEN_DIAGNOSTICS;
            }
        }
    }

    // 3. Limitação de Framerate para manter o barramento I2C e a CPU estáveis
    if (now - m_lastRenderMs < UI_REFRESH_INTERVAL_MS) {
        return;
    }
    m_lastRenderMs = now;

    // 4. Renderização do quadro
    m_display.clearBuffer();

    switch (m_currentScreen) {
        case SCREEN_BOOT: {
            uint32_t elapsed = now - m_bootStartMs;
            Screens::renderBoot(m_display, elapsed, BOOT_SCREEN_DURATION_MS);
            break;
        }
        case SCREEN_DIAGNOSTICS:
            Screens::renderDiagnostics(m_display, state);
            break;
        case SCREEN_HOME:
            Screens::renderHome(m_display, state);
            break;
        case SCREEN_SPEEDOMETER:
            Screens::renderSpeedometer(m_display, state);
            break;
        default:
            Screens::renderDiagnostics(m_display, state);
            break;
    }

    m_display.sendBuffer();
}
