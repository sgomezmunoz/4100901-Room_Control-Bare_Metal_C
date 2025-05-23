#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs y leer el botón (aunque el botón es por EXTI)
#include "systick.h" // Para obtener ticks y manejar retardos/tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM
static volatile uint32_t led_onoff_start_time = 0;
static volatile uint8_t led_onoff_active = 0;
static volatile uint32_t last_button_press_time = 0;

void room_control_app_init(void)
{
    // Inicializar variables de estado si es necesario.
    // Por ejemplo, asegurar que los LEDs estén apagados al inicio
    gpio_write_pin(GPIOA, 7, GPIO_PIN_RESET); // LED ON/OFF apagado
    tim3_ch1_pwm_set_duty_cycle(0); // PWM al 0%
    // tim3_ch1_pwm_set_duty_cycle(50); // Establecer un duty cycle inicial para el PWM LED
}



void room_control_on_button_press() {
    // Esperar mientras el botón sigue presionado
    uint32_t start_time = get_tick_ms();
    while (gpio_read_pin(GPIOC, 13) == 0) {
        // Esperar máximo 5 segundos (por seguridad)
        if ((get_tick_ms() - start_time) > 5000) {
            return; // Cancelar si excede el tiempo
        }
    }

    uint32_t press_duration = get_tick_ms() - start_time;

    if (press_duration >= 3000) {
        gpio_toggle_pin(GPIOA, 7);  // Cambiar estado del LED solo si >= 3s
        uart_send_string("Boton mantenido 3s, LED toggled\r\n");
    } else {
        uart_send_string("Presion corta, ignorado\r\n");
    }
}

void room_control_on_uart_receive(char received_char)
{
    // TODO: Procesar el carácter para realizar acciones
    // Ejemplo: si recibe 'h' o 'H', encender el LED PWM al 100%.
    //          si recibe 'l' o 'L', apagar el LED PWM (0%).
    //          si recibe 't', hacer toggle del LED ON/OFF.
    switch(received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100); // 100% duty cycle
            uart2_send_string("PWM al 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0); // 0% duty cycle (apagado)
            uart2_send_string("PWM al 0%\r\n");
            break;
        default:
            // Opcional: enviar mensaje de comando no reconocido
            break;
    }
}
void room_control_process(void)
{
    // Esta función debe ser llamada periódicamente (ej. desde el main loop)
    // para manejar tareas temporizadas
    
    if (led_onoff_active && (systick_get_tick() - led_onoff_start_time >= 3000)) {
        gpio_write_pin(GPIOA, 7, GPIO_PIN_RESET);
        led_onoff_active = 0;
        uart2_send_string("3 Segundos LED OFF.\r\n");
    }
}