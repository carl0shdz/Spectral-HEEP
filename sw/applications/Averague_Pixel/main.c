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
//#define INPUT_SIZE 1024*160
#define OUTPUT_SIZE 16
#define DMA_CHANNEL_tx 0
#define DMA_CHANNEL_rx 1

#define RAM2_BASE  ((uint32_t *)0x00010000)

uint32_t *RAM_scr = RAM2_BASE;
uint32_t *RAM_dst = RAM2_BASE + INPUT_SIZE;

int main(void) {
    dma_trans_t trans_w, trans_r;
    dma_target_t tgt_src_w, tgt_dst_w, tgt_src_r, tgt_dst_r;
    dma_config_flags_t rsp_w, rsp_r;
    
    uint32_t errors = 0;
    uint32_t total_cycles = 0; //Variable para guardar el tiempo
    timer_cycles_init(); //Inicializamos el timer
    
    RAM_scr = input_data;
    
    dma_init(NULL);  // inicializa el DMA interno
    
    //printf("Iniciando transferencia DMA Escritura en AveragePixel...\n\r");
    // Configurar fuente
    tgt_src_w.ptr = (uint8_t *)RAM_scr;
    tgt_src_w.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src_w.trig = DMA_TRIG_MEMORY;
    tgt_src_w.type = DMA_DATA_TYPE_WORD;
    // Configuracion destino
    tgt_dst_w.ptr = (uint8_t *)PIXEL_DATAIN_OFFSET;
    tgt_dst_w.inc_d1_du = 0;				    //No incrementa en el destino
    tgt_dst_w.trig = DMA_TRIG_MEMORY;
    tgt_dst_w.type = DMA_DATA_TYPE_WORD;
    
    trans_w.src = &tgt_src_w;
    trans_w.dst = &tgt_dst_w;
    trans_w.channel = DMA_CHANNEL_tx;
    trans_w.size_d1_du = INPUT_SIZE;
    trans_w.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans_w.win_du = 0;
    trans_w.end = DMA_TRANS_END_INTR;       	// Notificar con interrupción
    
    rsp_w = dma_validate_transaction(&trans_w, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    //printf("valid W: %u", rsp_w);
    rsp_w = dma_load_transaction(&trans_w);
    //printf("load W: %u \t\n\r", rsp_w);

    //printf("Iniciando transferencia DMA Lectura en AveragePixel...\n\r");
    // Configurar fuente
    tgt_src_r.ptr = (uint8_t *)PIXEL_RESULT_OFFSET;
    tgt_src_r.inc_d1_du = 0;         			// Incrementar dirección en 1 por dato
    tgt_src_r.trig = DMA_TRIG_MEMORY;
    tgt_src_r.type = DMA_DATA_TYPE_WORD;
    // Configuracion destino
    tgt_dst_r.ptr = (uint8_t *)RAM_dst;
    tgt_dst_r.inc_d1_du = 1;				    //No incrementa en el destino
    tgt_dst_r.trig = DMA_TRIG_MEMORY;
    tgt_dst_r.type = DMA_DATA_TYPE_WORD;

    trans_r.src = &tgt_src_r;
    trans_r.dst = &tgt_dst_r;
    trans_r.channel = DMA_CHANNEL_rx;
    trans_r.size_d1_du = OUTPUT_SIZE;
    trans_r.mode = DMA_TRANS_MODE_SINGLE;  		
    trans_r.win_du = 0;
    trans_r.end = DMA_TRANS_END_INTR;       	// Notificar con interrupción

    rsp_r = dma_validate_transaction(&trans_r, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    //printf("valid R: %u \n\r", rsp_r);
    rsp_r = dma_load_transaction(&trans_r);
    //printf("load R: %u \n\r", rsp_r);
    
    timer_start();				
    rsp_w = dma_launch(&trans_w);
    while (!dma_is_ready(DMA_CHANNEL_tx)){};

    rsp_r = dma_launch(&trans_r);
    while (!dma_is_ready(DMA_CHANNEL_rx)){};
    
    //timer_start();
    total_cycles = timer_stop();
    printf("Ciclos de reloj: %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
    	printf(" %u\n\r", RAM_dst[i]);
    }
    return 0;
}
