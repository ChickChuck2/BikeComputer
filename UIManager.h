/**
 * @file UIManager.h
 * @brief Gerenciador de navegação entre telas e ciclo de renderização.
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"
#include "DisplayManager.h"
#include "Screens.h"

class UIManager {
public:
    UIManager(DisplayManager& display);

    void init();

    /**
     * @brief Executa o ciclo de atualização da interface sem bloquear o loop.
     * @param state Estado consolidado do sistema.
     */
    void update(const BikeState& state);

    /**
     * @brief Muda a tela atual manualmente.
     */
    void setScreen(ScreenId screen);
    ScreenId getCurrentScreen() const { return m_currentScreen; }

private:
    DisplayManager& m_display;
    ScreenId m_currentScreen;
    uint32_t m_lastRenderMs;
    uint32_t m_bootStartMs;
    uint32_t m_lastCycleMs;
};

#endif // UI_MANAGER_H
