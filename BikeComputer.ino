/**
 * @file BikeComputer.ino
 * @brief Ponto de entrada principal para compilação no Arduino IDE.
 * 
 * Projeto: Computador de Bordo para Bicicleta (Bike Computer)
 * Arquitetura modular sem lógica de negócio em setup() ou loop().
 */

#include "Config.h"
#include "SystemManager.h"

// Instância central do orquestrador do sistema embarcado
static SystemManager systemManager;

void setup() {
    systemManager.init();
}

void loop() {
    systemManager.update();
}
