#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define GPIO_PIN 22  // GPIO utilizada para o controle do servo
#define PWM_FREQUENCY 50   // Frequência do PWM em Hz (50Hz = período de 20ms)


// Função para configurar o PWM na GPIO especificada
void setup_pwm(uint gpio_pin, float pulse_width_us) {
    // Calcula o período do PWM em microssegundos
    float pwm_period_us = 1000000.0 / PWM_FREQUENCY;

    // Inicializa o slice e o canal do PWM
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint pwm_slice = pwm_gpio_to_slice_num(gpio_pin);

    // Configura o divisor de clock para gerar a frequência desejada
    pwm_set_clkdiv(pwm_slice, 125.0); // Divisor do clock (para 125 MHz padrão)

    // Configura o valor TOP para o período de 20ms
    uint16_t pwm_top_value = (uint16_t)(125000000 / (PWM_FREQUENCY * 125));
    pwm_set_wrap(pwm_slice, pwm_top_value);

    // Calcula o nível do PWM com base no ciclo de trabalho
    uint16_t pwm_level = (uint16_t)((pulse_width_us / pwm_period_us) * pwm_top_value);
    pwm_set_gpio_level(gpio_pin, pwm_level);

    // Ativa o PWM
    pwm_set_enabled(pwm_slice, true);
}

// Função para ajustar suavemente o ângulo do servo
void servo_move(uint gpio_pin, float start_pulse_us, float end_pulse_us, uint step_us, uint delay_ms) {
    if (start_pulse_us < end_pulse_us) {
        for (float pulse_width = start_pulse_us; pulse_width <= end_pulse_us; pulse_width += step_us) {
            setup_pwm(gpio_pin, pulse_width);
            sleep_ms(delay_ms);
        }
    } else {
        for (float pulse_width = start_pulse_us; pulse_width >= end_pulse_us; pulse_width -= step_us) {
            setup_pwm(gpio_pin, pulse_width);
            sleep_ms(delay_ms);
        }
    }
}

int main() {
    stdio_init_all();  // Inicializa o terminal de saída padrão
    printf("Controle de servo motor com Pico SDK\n");

    // Configura o GPIO do servo
    gpio_set_function(GPIO_PIN, GPIO_FUNC_PWM);

    // Passo 1: Define o servo para 180 graus
    setup_pwm(GPIO_PIN, 2400);  // Largura de pulso para 180 graus
    sleep_ms(5000);

    // Passo 2: Define o servo para 90 graus
    setup_pwm(GPIO_PIN, 1470);  // Largura de pulso para 90 graus
    sleep_ms(5000);

    // Passo 3: Define o servo para 0 graus
    setup_pwm(GPIO_PIN, 500);   // Largura de pulso para 0 graus
    sleep_ms(5000);

    // Passo 4: Movimentação suave entre 0 e 180 graus
    while (true) {
        // Movimento de 0 para 180 graus
        servo_move(GPIO_PIN, 500, 2400, 5, 10);

        // Movimento de 180 para 0 graus
        servo_move(GPIO_PIN, 2400, 500, 5, 10);
    }

    return 0;
}