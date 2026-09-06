#include <stdio.h>
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"
#include "input_data.h"
#include "timer_sdk.h"

#define DATA_SIZE     16
#define INPUT_SIZE    582*10
#define OUTPUT_SIZE   16
#define NUM_CHANNELS  4

typedef enum {
    CH_PIXELS = 0,
    CH_DUPLIC = 1,
    CH_BRIGHT = 2,
    CH_PROSUB = 3,
    CH_ROTACI = 4
} dma_channel_idx_t;

uint32_t src[INPUT_SIZE] __attribute__((aligned(4))) = {0};
uint32_t dst[INPUT_SIZE] __attribute__((aligned(4))) = {0};

static dma_trans_t  dma_trans[NUM_CHANNELS];
static dma_target_t dma_src[NUM_CHANNELS];
static dma_target_t dma_dst[NUM_CHANNELS];

static inline void dma_wait_for_channel(dma_channel_idx_t channel) {
    while (!dma_is_ready(channel)) {
        // Deshabilitar interrupciones globales temporalmente para evitar condiciones de carrera
        CSR_CLEAR_BITS(CSR_REG_MSTATUS, 0x8);   //0x8 es el bit MIE (Machine Interrupt Enable)
        if (!dma_is_ready(channel)) {
            wait_for_interrupt(); // Pone al core RISC-V en modo sleep hasta la interrupción del DMA
        }
        CSR_SET_BITS(CSR_REG_MSTATUS, 0x8); // Re-habilitar interrupciones
    }
}

void setup_dma_channel(dma_channel_idx_t ch, uint32_t *src_ptr, uint32_t *dst_ptr, uint32_t size) {
    dma_src[ch].ptr       = (uint8_t *)src_ptr;
    dma_src[ch].inc_d1_du = 1;
    dma_src[ch].trig      = DMA_TRIG_MEMORY;
    dma_src[ch].type      = DMA_DATA_TYPE_WORD;

    dma_dst[ch].ptr       = (uint8_t *)dst_ptr;
    dma_dst[ch].inc_d1_du = 1;
    dma_dst[ch].trig      = DMA_TRIG_MEMORY;
    dma_dst[ch].type      = DMA_DATA_TYPE_WORD;

    dma_trans[ch].src        = &dma_src[ch];
    dma_trans[ch].dst        = &dma_dst[ch];
    dma_trans[ch].mode       = DMA_TRANS_MODE_SINGLE;
    dma_trans[ch].hw_fifo_en = 1;
    dma_trans[ch].channel    = ch;
    dma_trans[ch].dim        = DMA_DIM_CONF_1D;
    dma_trans[ch].size_d1_du = size;
    dma_trans[ch].end        = DMA_TRANS_END_INTR; // Generará interrupción al terminar
}

int main() {
    uint32_t total_cycles = 0;
    timer_cycles_init();
    printf("S0\n\r");
    for (int i = 0; i < INPUT_SIZE; i++) {
        src[i] = 2;
        dst[i] = 0;
    }
    //printf("S1\n\r");
    dma_init(NULL);                 
    setup_dma_channel(CH_ROTACI, src, dst, INPUT_SIZE);
    if (dma_validate_transaction(&dma_trans[CH_ROTACI], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error validando DMA en canal %d\n\r", CH_ROTACI);
        return -1;
    }
    dma_load_transaction(&dma_trans[CH_ROTACI]);
    //printf("S3\n\r");
    timer_start();
    dma_launch(&dma_trans[CH_ROTACI]);
    dma_wait_for_channel(CH_ROTACI);
    total_cycles = timer_stop();
    //printf("S4\n\r");
    printf("T = %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));
    
    return 0;
}
