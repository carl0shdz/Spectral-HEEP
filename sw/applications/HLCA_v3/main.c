#include <stdio.h>
#include <stdbool.h>
#include <string.h> // Para memcpy si es necesario
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"
#include "input_data.h"
#include "timer_sdk.h"
#include "fast_intr_ctrl.h"


#define DATA_SIZE     16
//#define DATA_SIZE     180
#define INPUT_SIZE    1920
//#define INPUT_SIZE    1024*180
#define OUTPUT_SIZE   16
//#define OUTPUT_SIZE   180
#define NUM_CHANNELS  4

typedef enum {
    CH_PIXELS = 0,
    CH_DUPLIC = 1,
    CH_BRIGHT = 2,
    CH_PROSUB = 3
} dma_channel_idx_t;


typedef struct __attribute__((aligned(4))) {
    uint32_t bright_data[DATA_SIZE * 2]; // Equivale a los (DATA_SIZE * 2) elementos
    uint32_t r_data[INPUT_SIZE];         
} Block_q_u_r_t;


typedef struct __attribute__((aligned(4))) {
    uint32_t index;
    uint32_t val;
    Block_q_u_r_t q_u_r;
} brg_buffer_t;

// Alineación de datos en memoria
uint32_t Block_R_u[OUTPUT_SIZE + INPUT_SIZE] __attribute__((aligned(4)));
uint32_t Cent[INPUT_SIZE] __attribute__((aligned(4))) = {0};

// Usamos la estructura mapeada brg en lugar del array crudo brg[(DATA_SIZE * 2) + 2]
brg_buffer_t brg_buf __attribute__((aligned(4)));

uint32_t prosub[INPUT_SIZE] __attribute__((aligned(4))) = {0};

unsigned int Pmax = 5;
unsigned char Segm = 0;
static dma_trans_t  dma_trans[NUM_CHANNELS];
static dma_target_t dma_src[NUM_CHANNELS];
static dma_target_t dma_dst[NUM_CHANNELS];


/*static inline void dma_wait_for_channel(dma_channel_idx_t channel) {
    while (!dma_is_ready(channel)) {
        // Deshabilitar interrupciones globales temporalmente para evitar condiciones de carrera
        CSR_CLEAR_BITS(CSR_REG_MSTATUS, 0x8);   // 0x8 es el bit MIE (Machine Interrupt Enable)
        if (!dma_is_ready(channel)) {
            wait_for_interrupt(); // Pone al core RISC-V en modo sleep hasta la interrupción del DMA
        }
        CSR_SET_BITS(CSR_REG_MSTATUS, 0x8); // Re-habilitar interrupciones
    }
}*/

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
    Segm++;
    //printf("canal: %d\n\r", channel);
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

    // Copia inicial
    memcpy(&Block_R_u[OUTPUT_SIZE], input_data, INPUT_SIZE * sizeof(uint32_t));
    
    // Inicialización del segmento fijo de r_data dentro de la estructura general
    memcpy(brg_buf.q_u_r.r_data, &Block_R_u[OUTPUT_SIZE], INPUT_SIZE * sizeof(uint32_t));

    uint32_t *Block_R = input_data;

    dma_init(NULL);                 
    
    setup_dma_channel(CH_PIXELS, Block_R,                    Block_R_u,            INPUT_SIZE);
    setup_dma_channel(CH_DUPLIC, Block_R_u,                  Cent,                 (INPUT_SIZE + OUTPUT_SIZE));
    setup_dma_channel(CH_BRIGHT, Cent,                       (uint32_t *)&brg_buf, INPUT_SIZE);
    setup_dma_channel(CH_PROSUB, (uint32_t *)&brg_buf.q_u_r, prosub,               (INPUT_SIZE + (OUTPUT_SIZE * 2)));

    for (int i = 0; i < NUM_CHANNELS; i++) {
        if (dma_validate_transaction(&dma_trans[i], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
            printf("Error validando DMA en canal %d\n\r", i);
            return -1;
        }
        dma_load_transaction(&dma_trans[i]);
    }

    // 1. Registrar tu función callback: guarda la dirección de tu función en una variable global para que el manejador de interrupciones pueda invocarla.
    dma_register_callback(mi_callback_dma);

    // 2. Habilitar la línea en el Fast Interrupt Controller (FIC)
    enable_fast_interrupt(kDma_done_fic_e, true);

    dma_finished = false;
    timer_start();

    // Canal 0: Pixels
    dma_launch(&dma_trans[CH_PIXELS]);
    //dma_wait_for_channel(CH_PIXELS);
    while (!dma_finished) {
        wait_for_interrupt();
    }

    // Canal 1: Duplic
    dma_finished = false;
    dma_launch(&dma_trans[CH_DUPLIC]);
    //dma_wait_for_channel(CH_DUPLIC);
    while (!dma_finished) {
        wait_for_interrupt();
    }

    for (int i = 0; i < Pmax; i++) {

        if (i > 0) {
            setup_dma_channel(CH_BRIGHT, prosub, (uint32_t *)&brg_buf, INPUT_SIZE);
            if (dma_validate_transaction(&dma_trans[CH_BRIGHT], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
                printf("Error validando DMA en canal %d\n\r", i);
                return -1;
            }
            dma_load_transaction(&dma_trans[CH_BRIGHT]);
        }                                

        // Canal 2: Bright
        dma_finished = false;
        dma_launch(&dma_trans[CH_BRIGHT]);
        while (!dma_finished) {
            wait_for_interrupt();
        }
        //dma_wait_for_channel(CH_BRIGHT); 
        //printf("P3\n\r");
        //printf("B_i = %d\n\r", brg_buf.index);
        //printf("B_v = %d\n\r",   brg_buf.val);



        if (i > 0) {
            setup_dma_channel(CH_PROSUB, (uint32_t *)&brg_buf.q_u_r, prosub, (INPUT_SIZE + (OUTPUT_SIZE * 2)));
            if (dma_validate_transaction(&dma_trans[CH_PROSUB], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
                printf("Error validando DMA en canal %d\n\r", i);
                return -1;
            }
            dma_load_transaction(&dma_trans[CH_PROSUB]);
        }  

        // Canal 3: ProSub
        dma_finished = false;
        dma_launch(&dma_trans[CH_PROSUB]);
        while (!dma_finished) {
            wait_for_interrupt();
        }
        //dma_wait_for_channel(CH_PROSUB);
        //printf("P4\n\r");
        //printf("ite %d\n\r", i);
    }
    
    total_cycles = timer_stop();
    printf("T = %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));

    return 0;
}