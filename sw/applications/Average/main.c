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
#define channel_Bright 2
#define channel_ProSub 3
uint32_t memory_pool[OUTPUT_SIZE + INPUT_SIZE] __attribute__((aligned(4)));     //bloque de entrada con centroide al incio
uint32_t dst[INPUT_SIZE] __attribute__((aligned(4))) = {0};                     //bloque centralizado
uint32_t brg[(DATA_SIZE * 2) + 2] __attribute__((aligned(4))) = {0};            //vector con los vectores Q y U concatenados, con el bloque centralizado al inicio
uint32_t dma4_pool[32 + INPUT_SIZE] __attribute__((aligned(4))) = {0};          // 32 valores de brg + 1920 de la imagen = 1952 palabras
uint32_t prosub[INPUT_SIZE] __attribute__((aligned(4))) = {0};                  //de salida interloop

int main() {
    dma_trans_t trans;
    dma_target_t tgt_src, tgt_dst;

    dma_trans_t trans_2;
    dma_target_t tgt_src_2, tgt_dst_2;

    dma_trans_t trans_3;
    dma_target_t tgt_src_3, tgt_dst_3;

    dma_trans_t trans_4;
    dma_target_t tgt_src_4, tgt_dst_4;

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

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    tgt_src_3.ptr = (uint8_t *)dst;
    tgt_src_3.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src_3.trig = DMA_TRIG_MEMORY;
    tgt_src_3.type = DMA_DATA_TYPE_WORD;

    // Configurar destino
    tgt_dst_3.ptr = (uint8_t *)brg;   // Destino en memoria
    tgt_dst_3.inc_d1_du = 1;
    tgt_dst_3.trig = DMA_TRIG_MEMORY;
    tgt_dst_3.type = DMA_DATA_TYPE_WORD;
    
    // Configurar la transacción
    trans_3.src = &tgt_src_3;
    trans_3.dst = &tgt_dst_3;
    trans_3.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans_3.hw_fifo_en = 1;
    trans_3.channel    = channel_Bright;
    trans_3.dim        = DMA_DIM_CONF_1D;
    trans_3.size_d1_du = (INPUT_SIZE);
    trans_3.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    tgt_src_4.ptr = (uint8_t *)dma4_pool;
    tgt_src_4.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src_4.trig = DMA_TRIG_MEMORY;
    tgt_src_4.type = DMA_DATA_TYPE_WORD;

    // Configurar destino
    tgt_dst_4.ptr = (uint8_t *)prosub;   // Destino en memoria
    tgt_dst_4.inc_d1_du = 1;
    tgt_dst_4.trig = DMA_TRIG_MEMORY;
    tgt_dst_4.type = DMA_DATA_TYPE_WORD;
    
    // Configurar la transacción
    trans_4.src = &tgt_src_4;
    trans_4.dst = &tgt_dst_4;
    trans_4.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans_4.hw_fifo_en = 1;
    trans_4.channel    = channel_ProSub;
    trans_4.dim        = DMA_DIM_CONF_1D;
    trans_4.size_d1_du = (INPUT_SIZE+(OUTPUT_SIZE*2));
    trans_4.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción


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

    if (dma_validate_transaction(&trans_3, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error3 validando DMA\n\r");
        return -1;
    }
    dma_load_transaction(&trans_3);

    if (dma_validate_transaction(&trans_4, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error4 validando DMA\n\r");
        return -1;
    }
    dma_load_transaction(&trans_4);


    timer_start();
    dma_launch(&trans);
    while (!dma_is_ready(channel_Pixels));				//Espero bandera de que ha terminado

    printf("P1\n\r");

    dma_launch(&trans_2);
    while (!dma_is_ready(channel_Duplic));

    printf("P2\n\r");

    dma_launch(&trans_3);
    while (!dma_is_ready(channel_Bright));
    
    printf("P3\n\r");

    for (int j = 0; j < (DATA_SIZE*2); j++) {
        dma4_pool[j] = brg[j + 2];
    }
    for (int i = 0; i < INPUT_SIZE; i++) {
        dma4_pool[(DATA_SIZE*2) + i] = memory_pool[OUTPUT_SIZE + i]; // OUTPUT_SIZE es 16
    }

    //for (int k = 0; k < 3; k++) {
    //    printf("d4 = %d\n\r", dma4_pool[k]);
    //}

    dma_launch(&trans_4);
    while (!dma_is_ready(channel_ProSub));

    total_cycles = timer_stop();

    printf("P4\n\r");

    //printf("T%u cc\n\r", total_cycles);
    //printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));
    //for (int i = 0; i < OUTPUT_SIZE-12; i++) {
    //    printf("Da_B = %d\n\r", dst[i]);
    //}

    //for (int i = 0; i < OUTPUT_SIZE-12; i++) {
    //    printf("Da_E = %d\n\r", dst[INPUT_SIZE - 1 - i]);
    //}

    /*for (int i = 0; i < ((OUTPUT_SIZE*2) + 2); i++) {
        if (i < OUTPUT_SIZE + 2 && i >= 2) {
            printf("qV = %d\n\r", brg[i]);
        }
        else if (i >= OUTPUT_SIZE + 2 && i < (OUTPUT_SIZE * 2 + 2)) {
            printf("uV = %d\n\r", brg[i]);
        }
        else {
            printf("Bg = %x\n\r", brg[i]);
        }
    }
    */
    for (int l = 0; l < (OUTPUT_SIZE); l++) {
        printf("IL = %d\n\r", prosub[l]);
    }

    return 0;
}