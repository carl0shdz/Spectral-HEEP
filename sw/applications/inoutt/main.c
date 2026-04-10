// Custom library headers
#include <stdio.h>
#include <stdint.h>
#include "core_v_mini_mcu.h"

// Direcciones MMIO para el periférico INOUT
#define INOUT_BASE_ADDR         (EXT_SLAVE_START_ADDRESS)         // 0xF0000000
#define INOUT_DATAIN_OFFSET     0x0000
#define INOUT_DATAOUT_OFFSET    0x0004

#define INOUT_DATAIN_ADDR       (INOUT_BASE_ADDR + INOUT_DATAIN_OFFSET)
#define INOUT_DATAOUT_ADDR      (INOUT_BASE_ADDR + INOUT_DATAOUT_OFFSET)

int main(void) {
    uint16_t input_value  = 0xABCD;
    uint32_t output_value;

    // Escribir el valor de entrada
    *(volatile uint32_t*)INOUT_DATAIN_ADDR = (uint32_t)input_value;

    // Esperar algunos ciclos para que el dato sea registrado (mínimo 1 ciclo)
    // En HW real, esto podría ser una espera activa o delay
    for (volatile int i = 0; i < 20; i++);  // espera simple

    // Leer el valor de salida (debería reflejar el input registrado)
    output_value = *(volatile uint32_t*)INOUT_DATAOUT_ADDR;

    // Imprimir valores
    printf("W INOUT: 0x%04X\n", input_value);
    printf("R INOUT: 0x%04X\n", (uint16_t)output_value);

    return 0;
}
