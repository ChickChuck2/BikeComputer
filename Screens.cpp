/**
 * @file Screens.cpp
 * @brief Implementação dos layouts gráficos para display 128x64 pixels.
 */

#include "Screens.h"
#include "Diagnostics.h"

// =============================================================================
// TELA DE BOOT / SPLASH
// =============================================================================
void Screens::renderBoot(DisplayManager& display, uint32_t elapsedMs, uint32_t totalDurationMs) {
    auto& u8g2 = display.getU8g2();

    // Moldura externa elegante
    u8g2.drawFrame(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Título Principal Centralizado (mantido em Flash)
    u8g2.setFont(u8g2_font_7x14B_tf);
    static const char title[] PROGMEM = "BIKE COMPUTER";
    char titleBuf[16];
    strncpy_P(titleBuf, title, sizeof(titleBuf) - 1);
    titleBuf[sizeof(titleBuf) - 1] = '\0';
    int16_t w = u8g2.getStrWidth(titleBuf);
    u8g2.setCursor((DISPLAY_WIDTH - w) / 2, 18);
    u8g2.print(titleBuf);

    // Linha sutil
    u8g2.drawHLine(14, 23, DISPLAY_WIDTH - 28);

    // Versão e MCU
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(20, 34);
    u8g2.print(F("v"));
    u8g2.print(F(FW_VERSION));
    u8g2.print(F(" | "));
    u8g2.print(F(TARGET_MCU_NAME));

    // Status de inicialização
    u8g2.setCursor(20, 44);
    u8g2.print(F("INITIALIZING HARDWARE..."));

    // Barra de Progresso (Calculada sem ultrapassar limites)
    uint8_t barWidth = DISPLAY_WIDTH - 40; // 88 pixels
    uint8_t barX = 20;
    uint8_t barY = 50;
    uint8_t barH = 5;

    u8g2.drawFrame(barX, barY, barWidth, barH);
    uint32_t progress = (elapsedMs >= totalDurationMs) ? barWidth : (elapsedMs * barWidth / totalDurationMs);
    if (progress > barWidth) progress = barWidth;
    u8g2.drawBox(barX, barY, progress, barH);
}

// =============================================================================
// TELA DE DIAGNÓSTICO
// =============================================================================
void Screens::renderDiagnostics(DisplayManager& display, const BikeState& state) {
    auto& u8g2 = display.getU8g2();

    // Cabeçalho padronizado
    display.drawHeader(F("DIAGNOSTICS"), F("TEST"));

    u8g2.setFont(u8g2_font_6x10_tf);

    // Linha 1: MCU
    u8g2.setCursor(2, 23);
    u8g2.print(F("MCU:     "));
    u8g2.print(F(TARGET_MCU_NAME));

    // Linha 2: Display OLED
    u8g2.setCursor(2, 33);
    u8g2.print(F("DISPLAY: SSD1306 "));
    u8g2.print(Diagnostics::statusToString(state.diagnostics.displayStatus));

    // Linha 3: Status BLE
    u8g2.setCursor(2, 43);
    u8g2.print(F("BLE GPS: "));
    u8g2.print(state.bleConnected ? F("CONECTADO") : F("AGUARDANDO"));

    // Linha 4: Memória SRAM disponível
    u8g2.setCursor(2, 53);
    u8g2.print(F("FREE RAM: "));
    u8g2.print(state.diagnostics.freeRamBytes);
    u8g2.print(F(" B"));

    // Linha 5: Uptime & FPS
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(2, 62);
    u8g2.print(F("UP: "));
    
    char uptimeBuf[10];
    Diagnostics::formatUptime(uptimeBuf, sizeof(uptimeBuf), state.diagnostics.uptimeSeconds);
    u8g2.print(uptimeBuf);

    u8g2.print(F(" | FPS: "));
    u8g2.print(state.diagnostics.currentFps);
}

// =============================================================================
// TELA PRINCIPAL (HOME)
// =============================================================================
void Screens::renderHome(DisplayManager& display, const BikeState& state) {
    auto& u8g2 = display.getU8g2();

    // Cabeçalho: Nome do projeto e tag explicativa do estado BLE
    display.drawHeader(F("BIKE COMPUTER"), state.bleConnected ? F("BLE:OK") : F("NO BLE"));

    // =========================================================================
    // SEÇÃO DE VELOCIDADE (DESTAQUE)
    // =========================================================================
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.setCursor(10, 31);

    char speedBuf[10];
    if (state.isSimulated) {
        dtostrf(state.navigation.speedKmh, 4, 1, speedBuf);
        u8g2.print(speedBuf);
    } else {
        u8g2.print(F(" 0.0"));
    }

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(52, 31);
    u8g2.print(F("km/h"));

    // Linha vertical separadora
    u8g2.drawVLine(86, 14, 26);

    // Uptime no canto superior direito
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(90, 23);
    u8g2.print(F("UPTIME"));
    u8g2.setCursor(90, 33);
    char uptimeBuf[10];
    Diagnostics::formatUptime(uptimeBuf, sizeof(uptimeBuf), state.diagnostics.uptimeSeconds);
    u8g2.print(uptimeBuf);

    // Linha horizontal divisória intermediária
    display.drawDivider(40);

    // =========================================================================
    // SEÇÃO DE SENSORES / DADOS SECUNDÁRIOS
    // =========================================================================
    u8g2.setFont(u8g2_font_5x7_tf);

    // Esquerda: Distância percorrida
    u8g2.setCursor(2, 50);
    u8g2.print(F("DST: "));
    if (state.isSimulated) {
        char dstBuf[8];
        dtostrf(state.navigation.distanceKm, 4, 2, dstBuf);
        u8g2.print(dstBuf);
        u8g2.print(F(" km"));
    } else {
        u8g2.print(F("0.00 km"));
    }

    // Direita: Altitude
    u8g2.setCursor(68, 50);
    u8g2.print(F("ALT: "));
    if (state.isSimulated) {
        char altBuf[8];
        dtostrf(state.altitude.currentAltM, 4, 0, altBuf);
        u8g2.print(altBuf);
        u8g2.print(F(" m"));
    } else {
        u8g2.print(F("-- m"));
    }

    // =========================================================================
    // FAIXA DE STATUS INFERIOR (AVISO DE MOCK / AUSÊNCIA DE HARDWARE)
    // =========================================================================
    u8g2.drawHLine(0, 53, DISPLAY_WIDTH);
    u8g2.setCursor(2, 62);
    if (state.isSimulated) {
        u8g2.print(F("* SIMULATION DATA ACTIVE *"));
    } else {
        u8g2.print(F("* STANDBY - SENSORS N/INST *"));
    }
}

// =============================================================================
// TELA DE VELOCÍMETRO DEDICADO
//
// Layout corrigido (128×64) — Y=baseline da fonte:
//
//   Y=0..2   : margens de segurança (borda não entra aqui)
//   Y=3..13  : header — seta de tendência (esq) + "km/h" (dir)
//   Y=14     : linha separadora
//   Y=15..48 : número gigante centralizado (logisoso32, baseline=48)
//   Y=49..56 : gauge suavizado (0–SPEED_MAX_KMH)
//   Y=57..63 : rodapé — MX: (esq) + tag SIM (dir)
//
// Animações:
//   1. Gauge com lerp inteiro ×4 (sub-pixel suavizado)
//   2. Cantos pulsantes (4 cantos L-shape, não borda completa)
//   3. Seta de tendência ▲ / ▼ / — (snapshot de 1s)
//   4. Decimal piscante quando Δv/s > 5 km/h/s
// =============================================================================

#define SPEED_MAX_KMH    80
// Comprimento do segmento L nos cantos pulsantes (pixels)
#define CORNER_LEN       8

void Screens::renderSpeedometer(DisplayManager& display, const BikeState& state) {
    auto& u8g2 = display.getU8g2();
    uint32_t now = millis();

    // -----------------------------------------------------------------------
    // ESTADO DE ANIMAÇÃO (static — persistido entre frames, custo ~20 bytes)
    // -----------------------------------------------------------------------
    static uint16_t gaugeFixed4  = 0;       // gauge em sub-pixels ×4
    static float    prevSpeedKmh = 0.0f;    // snapshot para tendência
    static uint32_t prevSpeedTime= 0;
    static int8_t   trendDir     = 0;       // -1 / 0 / +1
    static uint8_t  pulseTick    = 0;       // 0 ou 1 (cantos piscam)
    static uint32_t lastPulseMs  = 0;
    static uint8_t  decimalBlink = 0;
    static uint32_t lastBlinkMs  = 0;

    // -----------------------------------------------------------------------
    // 1. LÓGICA DE ANIMAÇÃO
    // -----------------------------------------------------------------------
    float spd = state.navigation.speedKmh;
    if (spd < 0.0f)   spd = 0.0f;
    if (spd > 999.0f) spd = 999.0f;

    // Gauge lerp inteiro
    uint16_t target4 = (uint16_t)((spd / SPEED_MAX_KMH) * (DISPLAY_WIDTH * 4));
    if (target4 > (uint16_t)(DISPLAY_WIDTH * 4)) target4 = (uint16_t)(DISPLAY_WIDTH * 4);
    if (gaugeFixed4 < target4) {
        uint16_t d = target4 - gaugeFixed4;
        gaugeFixed4 += (d < 6) ? d : (d / 6 + 1);
    } else if (gaugeFixed4 > target4) {
        uint16_t d = gaugeFixed4 - target4;
        gaugeFixed4 -= (d < 6) ? d : (d / 6 + 1);
    }

    // Tendência: snapshot a cada 1s
    if (now - prevSpeedTime >= 1000) {
        float delta = spd - prevSpeedKmh;
        trendDir = (delta >  1.2f) ?  1 :
                   (delta < -1.2f) ? -1 : 0;
        prevSpeedKmh  = spd;
        prevSpeedTime = now;
    }

    // Pulso de cantos: frequência sobe com a velocidade
    // 0 km/h → 1200 ms por ciclo | 80 km/h → 180 ms por ciclo
    uint16_t period = (spd < 1.0f) ? 1200
        : (uint16_t)(1200 - (spd / SPEED_MAX_KMH) * 1020);
    if (period < 180) period = 180;
    if (now - lastPulseMs >= period) {
        lastPulseMs = now;
        pulseTick ^= 1;
    }

    // Decimal piscante: quando variação rápida
    float dAbs = (spd > prevSpeedKmh) ? (spd - prevSpeedKmh) : (prevSpeedKmh - spd);
    if (dAbs > 5.0f) {
        if (now - lastBlinkMs >= 200) {
            lastBlinkMs = now;
            decimalBlink ^= 1;
        }
    } else {
        decimalBlink = 0;
    }

    // -----------------------------------------------------------------------
    // 2. RENDERIZAÇÃO
    // -----------------------------------------------------------------------

    // --- 2a. CANTOS PULSANTES (L-shapes nos 4 cantos da tela)
    //         Substituem drawFrame() para não cortar o texto do header
    //         Ficam exclusivamente nos cantos, sem invadir a área central.
    if (pulseTick) {
        uint8_t W = DISPLAY_WIDTH - 1;  // 127
        uint8_t H = DISPLAY_HEIGHT - 1; // 63
        uint8_t L = CORNER_LEN;

        // Canto superior esquerdo
        u8g2.drawHLine(0, 0, L);
        u8g2.drawVLine(0, 0, L);
        // Canto superior direito
        u8g2.drawHLine(W - L + 1, 0, L);
        u8g2.drawVLine(W, 0, L);
        // Canto inferior esquerdo
        u8g2.drawHLine(0, H, L);
        u8g2.drawVLine(0, H - L + 1, L);
        // Canto inferior direito
        u8g2.drawHLine(W - L + 1, H, L);
        u8g2.drawVLine(W, H - L + 1, L);
    }

    // --- 2b. HEADER (Y baseline = 13 → ascendente 5x7 chega até Y≈6, fora da borda)
    u8g2.setFont(u8g2_font_6x10_tf); // 6×10: mais legível que 5×7 no topo

    // Seta de tendência (esquerda)
    u8g2.setCursor(3, 13);
    if (trendDir > 0)       u8g2.print(F("+ ACC"));
    else if (trendDir < 0)  u8g2.print(F("- DEC"));
    else                    u8g2.print(F("= CST"));

    // Indicador BLE centralizado
    if (state.bleConnected) {
        u8g2.setCursor(54, 13);
        u8g2.print(F("BT"));
    } else {
        u8g2.setCursor(44, 13);
        u8g2.print(F("NO-BT"));
    }

    // Unidade "km/h" (direita) — âncora na borda direita
    const char* unit = "km/h";
    int16_t uw = u8g2.getStrWidth(unit);
    u8g2.setCursor(DISPLAY_WIDTH - uw - 3, 13);
    u8g2.print(unit);

    // Linha separadora limpa (2px abaixo da baseline → não toca nos descenders)
    u8g2.drawHLine(0, 15, DISPLAY_WIDTH);

    // --- 2c. NÚMERO GIGANTE DE VELOCIDADE
    //         Baseline Y=48: ascendente chega em ~Y=16 (2px de folga abaixo da linha)
    u8g2.setFont(u8g2_font_logisoso32_tn);

    // Monta string sem sprintf/dtostrf
    char spdStr[7];
    uint16_t sp10 = (uint16_t)(spd * 10.0f + 0.5f);
    uint8_t  spInt = (uint8_t)(sp10 / 10);
    uint8_t  spDec = (uint8_t)(sp10 % 10);
    uint8_t  idx = 0;
    if (spInt >= 100) spdStr[idx++] = '0' + (spInt / 100);
    if (spInt >= 10)  spdStr[idx++] = '0' + ((spInt / 10) % 10);
    spdStr[idx++] = '0' + (spInt % 10);
    if (!decimalBlink) {
        spdStr[idx++] = '.';
        spdStr[idx++] = '0' + spDec;
    }
    spdStr[idx] = '\0';

    int16_t nw = u8g2.getStrWidth(spdStr);
    u8g2.setCursor((DISPLAY_WIDTH - nw) / 2, 48);
    u8g2.print(spdStr);

    // --- 2d. GAUGE (Y=50 a Y=56, altura 5px + borda = 7px total)
    const uint8_t GY = 50;
    const uint8_t GH = 5;
    uint8_t gPx = (uint8_t)(gaugeFixed4 / 4);
    if (gPx > DISPLAY_WIDTH) gPx = DISPLAY_WIDTH;

    u8g2.drawFrame(0, GY, DISPLAY_WIDTH, GH);
    if (gPx > 2) {
        u8g2.drawBox(1, GY + 1, gPx - 2, GH - 2);
    }

    // Marcadores de escala: 20/40/60/80 km/h → 25%/50%/75% de 128px
    const uint8_t marks[3] = {32, 64, 96};
    for (uint8_t i = 0; i < 3; i++) {
        u8g2.drawVLine(marks[i], GY - 2, 2);
    }

    // --- 2e. RODAPÉ (baseline Y=63: 5px de font não ultrapassam Y=63, seguro)
    u8g2.setFont(u8g2_font_5x7_tf);

    // Velocidade máxima (esquerda)
    u8g2.setCursor(2, 62);
    u8g2.print(F("MX:"));
    uint8_t mx = (uint8_t)(state.navigation.maxSpeedKmh + 0.5f);
    if (mx >= 100)      u8g2.print(F("---"));
    else if (mx >= 10) { u8g2.print((char)('0' + mx / 10)); u8g2.print((char)('0' + mx % 10)); }
    else                 u8g2.print((char)('0' + mx));
    u8g2.print(F(" km/h"));

    // Indicador BLE / GPS (direita)
    u8g2.setCursor(92, 62);
    if (state.bleConnected) {
        if (state.navigation.hasFix) {
            u8g2.print(F("GPS:OK"));
        } else {
            u8g2.print(F("NO:FIX"));
        }
    } else {
        u8g2.print(F("AGUARDA"));
    }
}
