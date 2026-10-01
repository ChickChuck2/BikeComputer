# Computador de Bordo para Bicicleta (Bike Computer Firmware)

Firmware de base embarcado para ciclocomputador, projetado com arquitetura modular e desacoplada, preparado para migração de **Arduino UNO (ATmega328P)** para **ESP32**.

---

## 1. Visão Geral da Arquitetura

O sistema adota uma arquitetura em camadas para isolar responsabilidades e garantir que a interface gráfica e a lógica de telemetria operem independentemente dos drivers de hardware físicos.

```
                  ┌──────────────────────┐
                  │   BikeComputer.ino   │  (Entry point simples)
                  └──────────┬───────────┘
                             │
                  ┌──────────▼───────────┐
                  │    SystemManager     │  (Orquestrador central de ciclo de vida)
                  └──────┬───┬───┬───────┘
                         │   │   │
        ┌────────────────┘   │   └────────────────┐
        ▼                    ▼                    ▼
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│SensorManager │     │  BikeState   │     │  UIManager   │
└───────┬──────┘     │  (Data Bus)  │     └───────┬──────┘
        │            └──────────────┘             │
        ├─ IMU (BMI270)                           ▼
        ├─ Barometer (BMP280)             ┌──────────────┐
        ├─ GPS (GNSS UART)                │DisplayManager│ (U8g2 SSD1306)
        └─ Battery                        └───────┬──────┘
                                                  │
                                                  ▼
                                          ┌──────────────┐
                                          │   Screens    │
                                          │(Boot,Diag,UI)│
                                          └──────────────┘
```

### Princípios Rigorosos Aplicados:
1. **Zero Heap Allocation / Sem `String`**: Toda a formatação utiliza arrays estáticos e buffers seguros em C puro para evitar fragmentação e overflow dos 2 KB de SRAM do ATmega328P.
2. **Flash Memory Optimization**: Rótulos e strings de log mantidos em Flash via macro `F()` e tipos `PROGMEM`.
3. **Loop Não-Bloqueante**: Nenhum `delay()` é utilizado no loop principal; a temporização é orientada a `millis()`.
4. **Resiliência a Falhas de Hardware**: Se o display ou qualquer sensor falhar ou não estiver instalado, o firmware não entra em travamento (`panic/loop infinito`).

---

## 2. Pinagem e Conexões (Hardware Atual)

### Arduino UNO (ATmega328P) + OLED SSD1306 128×64 I²C

| Pino OLED | Pino Arduino UNO | Descrição |
|:---:|:---:|:---|
| **VCC** | **5V** (ou 3.3V) | Alimentação do módulo |
| **GND** | **GND** | Terra comum |
| **SCL** | **A5** (ou pino SCL dedicado) | Clock I²C (configurado a 400 kHz) |
| **SDA** | **A4** (ou pino SDA dedicado) | Dados I²C |

> **Nota:** Não são necessários resistores de pull-up externos se o módulo OLED já possuir os resistores integrados na placa (típico na grande maioria dos módulos de 4 pinos).

---

## 3. Estados de Sensores e Subsistemas

Os sensores e barramentos reportam seu estado via enum `SystemStatus`:
- `STATUS_NOT_INSTALLED`: Módulo de hardware ausente (estado atual de IMU, Barômetro e GPS no UNO).
- `STATUS_NOT_INITIALIZED`: Módulo presente, porém ainda não configurado.
- `STATUS_READY`: Módulo operacional e respondendo no barramento.
- `STATUS_ERROR`: Falha de comunicação, NACK I²C ou timeout.
- `STATUS_DISABLED`: Desabilitado explicitamente em `Config.h`.
- `STATUS_SIMULATED`: Dados gerados artificialmente para testes visuais da interface (Mock Data).

---

## 4. Configuração da Biblioteca U8g2

A biblioteca U8g2 deve ser instalada no Arduino IDE:
- **Gerenciador de Bibliotecas**: `Sketch` -> `Incluir Biblioteca` -> `Gerenciar Bibliotecas...` -> Pesquise por `U8g2` e instale a versão mais recente por **oliver**.

Construtor utilizado (em [DisplayManager.h](file:///b:/Repos/nada/BikeComputer/DisplayManager.h)):
```cpp
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
```

---

## 5. Procedimento de Upload e Teste

1. Conecte o Arduino UNO à porta USB do computador.
2. Abra a pasta `BikeComputer` no **Arduino IDE** (abrindo o arquivo `BikeComputer.ino`).
3. No menu **Ferramentas**:
   - **Placa**: *Arduino Uno*
   - **Porta**: Selecione a porta COM correspondente ao seu Arduino.
4. Clique em **Carregar (Upload)** (Ctrl + U).
5. Abra o **Monitor Serial** (Ctrl + Shift + M) a **115200 baud** para acompanhar o log de boot e diagnóstico de SRAM.

### Comportamento Esperado ao Ligar:
1. **0.0s - 2.5s**: Tela de Splash/Boot com barra de progresso e informações de versão.
2. **2.5s - 8.5s**: Tela de **DIAGNOSTICS** exibindo:
   - Identificação do MCU: `ATmega328P`
   - Estado do Display: `SSD1306 OK`
   - Barramento I²C: `400kHz (0x3C)`
   - Memória SRAM Livre: Calculada dinamicamente (`~800-900 bytes`)
   - Uptime e FPS de renderização.
3. **8.5s em diante**: Alternância automática de 6 em 6 segundos entre **DIAGNOSTICS** e a tela **HOME** (mostrando layout de ciclocomputador com etiqueta explícita de `SIMULATION DATA` ou `STANDBY`).
