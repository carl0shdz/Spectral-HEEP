#include <stdio.h>
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"
#include "input_data.h"
#include "timer_sdk.h"

#define DATA_SIZE 16
#define INPUT_SIZE 1920
//#define INPUT_SIZE 1024*160
#define OUTPUT_SIZE 16

uint32_t *RAM_scr;

int main() {
    dma_trans_t trans;
    dma_target_t tgt_src, tgt_dst;

    uint32_t src[DATA_SIZE] __attribute__((aligned(4))) = {0, 1, 2, 3, 4, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 5};
    uint32_t dst[DATA_SIZE] __attribute__((aligned(4))) = {0};

    RAM_scr = input_data;

    uint32_t total_cycles = 0; //Variable para guardar el tiempo
    timer_cycles_init(); //Inicializamos el timer

    dma_init(NULL);					// Inicializar DMA
    printf("Init\n\r");
    // Configurar fuente
    tgt_src.ptr = (uint8_t *)RAM_scr;
    tgt_src.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src.trig = DMA_TRIG_MEMORY;
    tgt_src.type = DMA_DATA_TYPE_WORD;

    // Configurar destino
    tgt_dst.ptr = (uint8_t *)dst;
    tgt_dst.inc_d1_du = 1;
    tgt_dst.trig = DMA_TRIG_MEMORY;
    tgt_dst.type = DMA_DATA_TYPE_WORD;
    
    // Configurar la transacción
    trans.src = &tgt_src;
    trans.dst = &tgt_dst;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.hw_fifo_en = 1;
    trans.channel    = 0;
    trans.dim        = DMA_DIM_CONF_1D;
    trans.size_d1_du = INPUT_SIZE;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción

    if (dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error validando DMA\n\r");
        return -1;
    }
    dma_load_transaction(&trans);

    timer_start();
    dma_launch(&trans);
    while (!dma_is_ready(0));				//Espero bandera de que ha terminado
    total_cycles = timer_stop();
    printf("Ciclos de reloj: %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        printf("dst = %d\n\r", dst[i]);
    }
    
    return 0;
}
