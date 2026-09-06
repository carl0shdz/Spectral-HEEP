#include <stdio.h>
#include <stdbool.h>
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"
#include "input_data.h"
#include "timer_sdk.h"
#include "fast_intr_ctrl.h"



#define DATA_SIZE     16
#define INPUT_SIZE    120*16
#define OUTPUT_SIZE   16
#define NUM_CHANNELS  5

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

//=======================================================
typedef void (*dma_channel_callback_t)(uint8_t channel);

static volatile bool dma_finished = false;
static dma_channel_callback_t user_callback = NULL;

void dma_register_callback(dma_channel_callback_t cb) {
    user_callback = cb;
}

/**
 * Sobrescribe la función weak 'fic_irq_dma_done' de fast_intr_ctrl.c.
 * Esta función es invocada inmediatamente después de que el FIC limpia el flag pendiente.
 */
void dma_intr_handler_trans_done(uint8_t channel) {
    if (user_callback != NULL) {
        user_callback(channel);
    }
    dma_finished = true;
}

void mi_callback_dma(uint8_t channel) {
    printf(">> [CALLBACK] Fin de transferencia en el canal %d por interrupcion pura.\n\r", channel);
}

//=======================================================

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

    printf("S1\n\r");
    dma_init(NULL);                 
    setup_dma_channel(CH_PIXELS, src, dst, INPUT_SIZE);

    if (dma_validate_transaction(&dma_trans[CH_PIXELS], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error validando DMA en canal %d\n\r", CH_PIXELS);
        return -1;
    }
    dma_load_transaction(&dma_trans[CH_PIXELS]);

    // 1. Registrar tu función callback: guarda la dirección de tu función en una variable global para que el manejador de interrupciones pueda invocarla.
    dma_register_callback(mi_callback_dma);

    // 2. Habilitar la línea en el Fast Interrupt Controller (FIC)
    enable_fast_interrupt(kDma_done_fic_e, true);

    printf("S2: Lanzando DMA...\n\r");
    dma_finished = false;
    timer_start();

    // 3. Iniciar la transferencia por hardware
    dma_launch(&dma_trans[CH_PIXELS]);

    // 4. Espera pasiva de bajo consumo (WFI):
    // La CPU se duerme. Cuando el DMA termina, salta inmediatamente a
    // `handler_irq_fast_dma_done`, ejecuta `mi_callback_dma` y despierta al core.
    while (!dma_finished) {
        wait_for_interrupt();
    }

    total_cycles = timer_stop();
    printf("S3\n\r");
    printf("T = %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));

    // Comprobación de integridad
    bool ok = true;
    for (int i = 0; i < 2; i++) {
        printf("dst[%d] = %u\n\r", i, dst[i]);
    }
    
    return 0;
}
