#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "core_v_mini_mcu.h"
#include "dma.h"
#include "input_data.h"
#include "timer_sdk.h"

#define PIXEL_DATAIN_OFFSET (EXT_SLAVE_START_ADDRESS + 0x0000)
#define PIXEL_RESULT_OFFSET (EXT_SLAVE_START_ADDRESS + 0x0004)

#define INPUT_SIZE 1920
#define OUTPUT_SIZE 16
#define DMA_CHANNEL 0

#define RAM2_BASE  ((uint32_t *)0x00010000)

uint32_t *RAM_scr = RAM2_BASE;
uint32_t *RAM_dst = RAM2_BASE + INPUT_SIZE;

//static uint32_t RAM_scr[INPUT_SIZE] __attribute__((aligned(4)));
//static uint32_t RAM_dst[OUTPUT_SIZE] __attribute__((aligned(4)));



int main(void) {
    dma_trans_t trans;
    dma_target_t tgt_src, tgt_dst;
    dma_config_flags_t rsp;
    
    uint32_t errors = 0;
    uint32_t total_cycles = 0; //Variable para guardar el tiempo
    timer_cycles_init(); //Inicializamos el timer
    
    RAM_scr = input_data;
    
    dma_init(NULL);  // inicializa el DMA interno
    
    // Configurar fuente
    tgt_src.ptr = (uint8_t *)RAM_scr;
    tgt_src.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src.trig = DMA_TRIG_MEMORY;
    tgt_src.type = DMA_DATA_TYPE_WORD;
    // Configuracion destino
    tgt_dst.ptr = (uint8_t *)PIXEL_DATAIN_OFFSET;
    tgt_dst.inc_d1_du = 0;				//No incrementa en el destino
    tgt_dst.trig = DMA_TRIG_MEMORY;
    tgt_dst.type = DMA_DATA_TYPE_WORD;
    
    trans.src = &tgt_src;
    trans.dst = &tgt_dst;
    trans.size_d1_du = INPUT_SIZE;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.win_du = 0;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción
    
    //printf("\n --> \n\r");
    rsp = dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    //printf("valid: %u \t\n\r", rsp);
    rsp = dma_load_transaction(&trans);
    //printf("load: %u \t\n\r", rsp);
    timer_start();					//estrategicamente se debe medir en este punto
    rsp = dma_launch(&trans);
    //printf("launch: %u \t\n\r", rsp);
    
    while (!dma_is_ready(DMA_CHANNEL))
    //printf("TX done\n\r");
    
    // Configurar fuente
    tgt_src.ptr = (uint8_t *)PIXEL_RESULT_OFFSET;
    tgt_src.inc_d1_du = 0;         			// Incrementar dirección en 1 por dato
    tgt_src.trig = DMA_TRIG_MEMORY;
    tgt_src.type = DMA_DATA_TYPE_WORD;
    // Configuracion destino
    tgt_dst.ptr = (uint8_t *)RAM_dst;
    tgt_dst.inc_d1_du = 1;				//No incrementa en el destino
    tgt_dst.trig = DMA_TRIG_MEMORY;
    tgt_dst.type = DMA_DATA_TYPE_WORD;

    trans.src = &tgt_src;
    trans.dst = &tgt_dst;
    trans.size_d1_du = OUTPUT_SIZE;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.win_du = 0;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción
    
    
    //printf("\n <--\n\r");
    rsp = dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    //printf("valid: %u \n\r", rsp);
    rsp = dma_load_transaction(&trans);
    //printf("load: %u \n\r", rsp);
    rsp = dma_launch(&trans);
    //printf("launch: %u \n\r", rsp);
    
    while (!dma_is_ready(DMA_CHANNEL));
    //printf("RX done\n\r");
    
    total_cycles = timer_stop();
    printf("Ciclos de reloj: %u cc\n\r", total_cycles);
    printf("Tiempo aprox:    %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
    	printf("R%d = %u\n\r", i, RAM_dst[i]);
    }
    return 0;
}


