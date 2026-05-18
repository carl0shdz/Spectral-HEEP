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
#define channel_Pixels 0
#define channel_Duplic 1
uint32_t memory_pool[OUTPUT_SIZE + INPUT_SIZE] __attribute__((aligned(4)));
uint32_t dst[DATA_SIZE] __attribute__((aligned(4))) = {0};

int main() {
    dma_trans_t trans;
    dma_target_t tgt_src, tgt_dst;

    dma_trans_t trans_2;
    dma_target_t tgt_src_2, tgt_dst_2;

    uint32_t total_cycles = 0;
    timer_cycles_init(); //Inicializamos el timer

    for(int i = 0; i < INPUT_SIZE; i++){
        memory_pool[OUTPUT_SIZE + i] = input_data[i];
    }
    uint32_t *RAM_scr = &memory_pool[OUTPUT_SIZE];
    RAM_scr = input_data;

    dma_init(NULL);					// Inicializar DMA
    
    // Configurar fuente
    tgt_src.ptr = (uint8_t *)RAM_scr;
    tgt_src.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src.trig = DMA_TRIG_MEMORY;
    tgt_src.type = DMA_DATA_TYPE_WORD;

    // Configurar destino
    tgt_dst.ptr = (uint8_t *)memory_pool;   // Destino en memoria
    tgt_dst.inc_d1_du = 1;
    tgt_dst.trig = DMA_TRIG_MEMORY;
    tgt_dst.type = DMA_DATA_TYPE_WORD;
    
    // Configurar la transacción
    trans.src = &tgt_src;
    trans.dst = &tgt_dst;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.hw_fifo_en = 1;
    trans.channel    = channel_Pixels;
    trans.dim        = DMA_DIM_CONF_1D;
    trans.size_d1_du = INPUT_SIZE;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    tgt_src_2.ptr = (uint8_t *)memory_pool;
    tgt_src_2.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src_2.trig = DMA_TRIG_MEMORY;
    tgt_src_2.type = DMA_DATA_TYPE_WORD;

    // Configurar destino
    tgt_dst_2.ptr = (uint8_t *)dst;   // Destino en memoria
    tgt_dst_2.inc_d1_du = 1;
    tgt_dst_2.trig = DMA_TRIG_MEMORY;
    tgt_dst_2.type = DMA_DATA_TYPE_WORD;
    
    // Configurar la transacción
    trans_2.src = &tgt_src_2;
    trans_2.dst = &tgt_dst_2;
    trans_2.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans_2.hw_fifo_en = 1;
    trans_2.channel    = channel_Duplic;
    trans_2.dim        = DMA_DIM_CONF_1D;
    trans_2.size_d1_du = (INPUT_SIZE+OUTPUT_SIZE);
    trans_2.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción


    if (dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error1 validando DMA\n\r");
        return -1;
    }
    dma_load_transaction(&trans);

    if (dma_validate_transaction(&trans_2, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error2 validando DMA\n\r");
        return -1;
    }
    dma_load_transaction(&trans_2);

    timer_start();
    dma_launch(&trans);
    while (!dma_is_ready(channel_Pixels));				//Espero bandera de que ha terminado

    dma_launch(&trans_2);
    while (!dma_is_ready(channel_Duplic));

    total_cycles = timer_stop();
    printf("Ciclos de reloj: %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        printf("Da_B = %d\n\r", dst[i]);
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        printf("Da_E = %d\n\r", dst[INPUT_SIZE - 1 - i]);
    }

    return 0;
}