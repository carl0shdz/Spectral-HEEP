#include <stdio.h>
#include <stdint.h>
#include "core_v_mini_mcu.h"
#include "dma.h"

#define INOUT_BASE_ADDR         (EXT_SLAVE_START_ADDRESS)         // 0xF0000000
#define INOUT_DATAIN_OFFSET     0x0000
#define INOUT_DATAOUT_OFFSET    0x0004

#define INOUT_DATAIN_ADDR       (INOUT_BASE_ADDR + INOUT_DATAIN_OFFSET)
#define INOUT_DATAOUT_ADDR      (INOUT_BASE_ADDR + INOUT_DATAOUT_OFFSET)

#define DATA_SIZE	1
#define DMA_CHANNEL_TX  0
#define DMA_CHANNEL_RX  1

//static uint32_t RAM_scr[DATA_SIZE] __attribute__((aligned(4))) = {0xAB, 0xAC, 0xAD, 0xAF, 0xA0, 0xA1, 0xA2, 0xA3};
//static uint32_t RAM_scr[DATA_SIZE] __attribute__((aligned(4))) = {0xAB, 0xAC, 0xAD, 0xAF};
static uint32_t RAM_scr[DATA_SIZE] __attribute__((aligned(4))) = {0xAB};
static uint32_t RAM_dst[DATA_SIZE] __attribute__((aligned(4))) = {0};

int main(void) {
    dma_trans_t trans;
    dma_target_t tgt_src, tgt_dst;
    
    dma_config_flags_t rsp;
    uint32_t errors = 0;

    dma_init(NULL);  // inicializa el DMA interno
    
    printf("A src = 0x%04x\n", INOUT_DATAIN_ADDR);
    printf("A des = 0x%04x\n", INOUT_DATAOUT_ADDR);
    
    // Configurar fuente
    tgt_src.ptr = (uint8_t *)RAM_scr;
    tgt_src.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src.trig = DMA_TRIG_MEMORY;
    tgt_src.type = DMA_DATA_TYPE_WORD;
    // Configuracion destino
    tgt_dst.ptr = (uint8_t *)INOUT_DATAIN_ADDR;
    tgt_dst.inc_d1_du = 0;				//No incrementa en el destino
    tgt_dst.trig = DMA_TRIG_MEMORY;
    tgt_dst.type = DMA_DATA_TYPE_WORD;
    
    trans.src = &tgt_src;
    trans.dst = &tgt_dst;
    trans.size_d1_du = DATA_SIZE;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.win_du = 0;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción
    
    printf("\n-- TX --\n\r");
    rsp = dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    //printf("vld: %u \t\n\r", rsp);
    rsp = dma_load_transaction(&trans);
    //printf("load: %u \t\n\r", rsp);
    rsp = dma_launch(&trans);
    //printf("launch: %u \t\n\r", rsp);
    
    while (!dma_is_ready(DMA_CHANNEL_TX))
    printf("TX done\n\r");
    
    
    // Configurar fuente
    tgt_src.ptr = (uint8_t *)INOUT_DATAOUT_ADDR;
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
    trans.size_d1_du = DATA_SIZE;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.win_du = 0;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción
    
    
    printf("\n-- RX --\n\r");
    rsp = dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    //printf("validate: %u \n\r", rsp);
    rsp = dma_load_transaction(&trans);
    //printf("load: %u \n\r", rsp);
    rsp = dma_launch(&trans);
    //printf("launch: %u \n\r", rsp);
    
    while (!dma_is_ready(DMA_CHANNEL_TX));
    printf("RX done\n\r");
    printf("Result T0 = 0x%04x\n", RAM_dst[0]);
    return 0;
}
