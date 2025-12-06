/* Edge Impulse TinyML Final Project Code
 * Integrates BH1750 sensor reading with local classification on ESP32.
 * Based on Edge Impulse Arduino Example.
 */

/* Includes ---------------------------------------------------------------- */
// Incluído o cabeçalho do modelo (o nome exato após a correção manual da pasta)
#include <g2_iot_inferencing.h> 
#include <Wire.h>
#include <BH1750.h> 

/* Constant defines -------------------------------------------------------- */
#define FREQUENCY_HZ             19         // Frequência de amostragem (Hz)
// Intervalo em ms: 1000ms / 19Hz = 52.6ms
#define EI_CLASSIFIER_INTERVAL_MS (1000 / FREQUENCY_HZ) 
#define LED_PIN                  2          // Pino GPIO 2 (D4) para o Atuador (LED)

/* Private variables ------------------------------------------------------- */
static bool debug_nn = false; 
BH1750 lightMeter; 

// --- Variáveis de Amostragem ---
static uint32_t next_tick = 0;
static uint32_t current_sample = 0;
// Buffer alocado estaticamente para as amostras (tamanho definido pelo Edge Impulse)
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 }; 

// --- Protótipos de Funções Auxiliares ---
void control_actuator(const char* best_match_label); 
// Funções requeridas pelo Edge Impulse
extern "C" { // Garante que as funções são compatíveis com a biblioteca C++ do Edge Impulse
    int ei_write(char *data, int length);
}
int get_signal_data(size_t offset, size_t length, float *out);

// Função para printar no Serial (usa Serial.write)
int ei_write(char *data, int length) {
    for(int i=0; i<length; i++) {
        Serial.write(data[i]);
    }
    return length;
}


/**
* @brief      Arduino setup function
*/
void setup()
{
    Serial.begin(115200);
    Wire.begin(); 
    
    // Inicializa sensor BH1750
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("Failed to initialize BH1750 sensor!");
        while(1);
    }
    
    // Configura o pino do LED
    pinMode(LED_PIN, OUTPUT);
    
    // Verificações
    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 1) { 
        Serial.printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 1 (eixo Lux)\n");
        while(1);
    }
    Serial.printf("Edge Impulse iniciado | Frequência: %d Hz\n", FREQUENCY_HZ);
    delay(1000); // Aguarda estabilização
}


/**
* @brief      Loop principal: Coleta de dados e Inferência
*/
void loop()
{
    uint32_t now = micros(); // Usamos micros() para precisão

    // 1. Coleta de Amostras (Amostragem a 19 Hz)
    if (now >= next_tick) {
        
        // Inicializa o tick na primeira vez
        if (next_tick == 0) {
            next_tick = now;
        }

        float lux = lightMeter.readLightLevel();

        // Armazena a amostra se for válida e se houver espaço
        if (!isnan(lux) && current_sample < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            buffer[current_sample] = lux;
            current_sample++;
        }
        
        // Agenda o próximo tick em microssegundos
        next_tick += (EI_CLASSIFIER_INTERVAL_MS * 1000); 

        // ----------------------------------------------------
        // 2. Classificação (Executa quando a janela de 2s está completa)
        if (current_sample >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            
            ei_impulse_result_t result = { 0 };
            signal_t signal;
            
            // Cria o objeto signal a partir do buffer de amostras
            int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
            
            if (err == 0) {
                // Roda o classificador Edge Impulse
                err = run_classifier(&signal, &result, debug_nn);
            }
            
            if (err == EI_IMPULSE_OK) {
                // 3. Serial Print e Ativações (Requisito de Avaliação)
                ei_printf("\n--- INFERÊNCIA --- \n");
                ei_printf("Tempo (DSP: %d ms, Classif: %d ms)\n",
                          result.timing.dsp, result.timing.classification);
                
                const char *best_match_label = result.classification[0].label;
                float max_value = result.classification[0].value;
                
                // Imprime Ativações e determina o melhor rótulo
                ei_printf("Ativações:\n");
                for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
                    ei_printf("   %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
                    if (result.classification[ix].value > max_value) {
                        max_value = result.classification[ix].value;
                        best_match_label = result.classification[ix].label;
                    }
                }
                
                // 4. Atuador: Controla o LED
                control_actuator(best_match_label);
                ei_printf("CLASSIFICAÇÃO FINAL: **%s** (Prob: %.2f)\n", best_match_label, max_value);
                
            } else {
                ei_printf("ERR: Falha ao rodar classificador (%d)\n", err);
            }

            // Reinicia a coleta
            current_sample = 0;
        }
    }
}


// --- Funções Auxiliares ---

// 1. Implementa o requisito do Atuador (LED no GPIO 2/D4)
void control_actuator(const char* best_match_label) {
    // Lógica: Acende o LED se detectar 'lanterna', caso contrário, apaga.
    
    // CORREÇÃO: A comparação strcmp precisa dos argumentos e fechar o parêntese
    if (strcmp(best_match_label, "lanterna") == 0) {
        // Bloco de código para LIGAR o LED
        digitalWrite(LED_PIN, HIGH); 
        ei_printf("ATUADOR: LED LIGADO (LANTERNA DETECTADA)\n"); 
    } else {
        // Bloco de código para DESLIGAR o LED
        digitalWrite(LED_PIN, LOW);
        ei_printf("ATUADOR: LED DESLIGADO\n"); 
    }
}