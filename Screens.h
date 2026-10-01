/**
 * @file Screens.h
 * @brief Definições e renderizadores das telas individuais da interface (128x64).
 */

#ifndef SCREENS_H
#define SCREENS_H

#include <Arduino.h>
#include "Config.h"
#include "BikeState.h"
#include "DisplayManager.h"

enum ScreenId : uint8_t {
    SCREEN_BOOT = 0,
    SCREEN_DIAGNOSTICS,
    SCREEN_HOME,
    SCREEN_SPEEDOMETER
};

class Screens {
public:
    /**
     * @brief Renderiza a tela de Boot / Splash com progresso.
     */
    static void renderBoot(DisplayManager& display, uint32_t elapsedMs, uint32_t totalDurationMs);

    /**
     * @brief Renderiza a tela de Diagnóstico de Hardware e Memória.
     */
    static void renderDiagnostics(DisplayManager& display, const BikeState& state);

    /**
     * @brief Renderiza a tela Principal (Ciclocomputador / Home).
     */
    static void renderHome(DisplayManager& display, const BikeState& state);

    /**
     * @brief Renderiza o Velocímetro dedicado com animações.
     * Exibe velocidade em km/h em fonte grande com:
     *   - barra de gauge com interpolação suave
     *   - seta de tendência (acelerando / desacelerando / constante)
     *   - pulso de borda sincronizado com intensidade da velocidade
     */
    static void renderSpeedometer(DisplayManager& display, const BikeState& state);
};

#endif // SCREENS_H
