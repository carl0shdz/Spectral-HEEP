#include <stdio.h>
#include <string.h> // Para memcpy si es necesario
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"
#include "input_data.h"
#include "timer_sdk.h"

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
uint32_t Block_R_u_2[OUTPUT_SIZE + INPUT_SIZE] __attribute__((aligned(4)));
uint32_t Cent_2[INPUT_SIZE] __attribute__((aligned(4))) = {0};

// Usamos la estructura mapeada brg en lugar del array crudo brg[(DATA_SIZE * 2) + 2]
brg_buffer_t brg_buf __attribute__((aligned(4)));
brg_buffer_t brg_buf_2 __attribute__((aligned(4)));
brg_buffer_t brg_buf_f __attribute__((aligned(4)));

uint32_t prosub[INPUT_SIZE] __attribute__((aligned(4))) = {0};
uint32_t prosub_2[INPUT_SIZE] __attribute__((aligned(4))) = {0};
uint32_t prosub_3[INPUT_SIZE] __attribute__((aligned(4))) = {0};

#define PMAX 3
unsigned int Pmax = PMAX;
//unsigned int Pmax = 3;

//stage 3
uint32_t Mk_in[OUTPUT_SIZE + INPUT_SIZE] __attribute__((aligned(4)));
uint32_t Cent_mk[INPUT_SIZE] __attribute__((aligned(4))) = {0};
uint32_t ProjSubj_stage3[INPUT_SIZE + (OUTPUT_SIZE * 2)] __attribute__((aligned(4))) = {0};

static dma_trans_t  dma_trans[NUM_CHANNELS];
static dma_target_t dma_src[NUM_CHANNELS];
static dma_target_t dma_dst[NUM_CHANNELS];

//uint32_t Vectores_Q_U[DATA_SIZE * 2][Pmax] __attribute__((aligned(4))) = {0};
uint32_t Vectores_Q_U[3][DATA_SIZE * 2] __attribute__((aligned(4))) = {0};
uint32_t index_tao;
uint32_t val_tao;

static inline void dma_wait_for_channel(dma_channel_idx_t channel) {
    while (!dma_is_ready(channel)) {
        // Deshabilitar interrupciones globales temporalmente para evitar condiciones de carrera
        CSR_CLEAR_BITS(CSR_REG_MSTATUS, 0x8);   // 0x8 es el bit MIE (Machine Interrupt Enable)
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

    timer_start();
    /////////////////////////////////////////////Stage 1: extracción lina por linea/////////////////////////////////////////////////
    /////////////////////////////////////////////suponemos que nf = 1/////////////////////////////////////////////////
    //printf("Ini S1\n\r");
    // Canal 0: Promedio
    dma_launch(&dma_trans[CH_PIXELS]);
    dma_wait_for_channel(CH_PIXELS);

    // Canal 1: Centralizado
    dma_launch(&dma_trans[CH_DUPLIC]);
    dma_wait_for_channel(CH_DUPLIC);

    // Canal 2: Brillo
    dma_launch(&dma_trans[CH_BRIGHT]);
    dma_wait_for_channel(CH_BRIGHT);

    // Canal 3: ProSub
    dma_launch(&dma_trans[CH_PROSUB]);
    dma_wait_for_channel(CH_PROSUB);

    /////////////////////////////////////////////Stage 2: Estimación del subespacio/////////////////////////////////////////////////
    //printf("Ini S2\n\r");
     // Copia inicial
    memcpy(&Block_R_u_2[OUTPUT_SIZE], prosub, INPUT_SIZE * sizeof(uint32_t));
    
    // Inicialización del segmento fijo de r_data dentro de la estructura general
    memcpy(brg_buf_2.q_u_r.r_data, &Block_R_u_2[OUTPUT_SIZE], INPUT_SIZE * sizeof(uint32_t));

    uint32_t *Block_R_2 = prosub;

    setup_dma_channel(CH_PIXELS, Block_R_2,                     Block_R_u_2,            INPUT_SIZE);
    setup_dma_channel(CH_DUPLIC, Block_R_u_2,                   Cent_2,                 (INPUT_SIZE + OUTPUT_SIZE));
    setup_dma_channel(CH_BRIGHT, Cent_2,                        (uint32_t *)&brg_buf_2, INPUT_SIZE);
    setup_dma_channel(CH_PROSUB, (uint32_t *)&brg_buf_2.q_u_r,  prosub_2,(INPUT_SIZE + (OUTPUT_SIZE * 2)));

    for (int i = 0; i < NUM_CHANNELS; i++) {
        if (dma_validate_transaction(&dma_trans[i], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
            printf("Error validando DMA en canal %d\n\r", i);
            return -1;
        }
        dma_load_transaction(&dma_trans[i]);
    }

    //printf("S2 -> prom\n\r");
    // Canal 0: Promedio
    dma_launch(&dma_trans[CH_PIXELS]);
    dma_wait_for_channel(CH_PIXELS);
    //printf("S2 -> cent\n\r");
    // Canal 1: Centralizado
    dma_launch(&dma_trans[CH_DUPLIC]);
    dma_wait_for_channel(CH_DUPLIC);

    for (int i = 0; i < Pmax; i++) {
        if (i == 0) {
            //printf("S2 -> brig\n\r");
            // Canal 2: Brillo
            dma_launch(&dma_trans[CH_BRIGHT]);
            dma_wait_for_channel(CH_BRIGHT);

            memcpy(Vectores_Q_U[i], brg_buf_2.q_u_r.bright_data, (DATA_SIZE * 2) * sizeof(uint32_t));

            //printf("S2 -> PrSu\n\r");
            // Canal 3: q
            dma_launch(&dma_trans[CH_PROSUB]);
            dma_wait_for_channel(CH_PROSUB);
        }
        else {
            setup_dma_channel(CH_BRIGHT, prosub_2,                      (uint32_t *)&brg_buf_2, INPUT_SIZE);
            setup_dma_channel(CH_PROSUB, (uint32_t *)&brg_buf_2.q_u_r,  prosub_2,(INPUT_SIZE + (OUTPUT_SIZE * 2)));

            if (dma_validate_transaction(&dma_trans[2], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
                printf("Error validando DMA en canal %d\n\r", 2);
                return -1;
            }
            dma_load_transaction(&dma_trans[2]);

            if (dma_validate_transaction(&dma_trans[3], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
                printf("Error validando DMA en canal %d\n\r", 3);
                return -1;
            }
            dma_load_transaction(&dma_trans[3]);

            //printf("S2 -> brig\n\r");
            // Canal 2: Brillo
            dma_launch(&dma_trans[CH_BRIGHT]);
            dma_wait_for_channel(CH_BRIGHT);

            memcpy(Vectores_Q_U[i], brg_buf_2.q_u_r.bright_data, (DATA_SIZE * 2) * sizeof(uint32_t));
            index_tao = brg_buf_2.index;
            val_tao = brg_buf_2.val;

            //printf("S2 -> PrSu\n\r");
            // Canal 3: q
            dma_launch(&dma_trans[CH_PROSUB]);
            dma_wait_for_channel(CH_PROSUB);
        }
    }

    


    /////////////////////////////////////////////Stage 3: Calculo sub espacio ortogonal//////////////////////////////////////////////
    memcpy(&Mk_in[OUTPUT_SIZE], input_data, INPUT_SIZE * sizeof(uint32_t));
    memcpy(&Mk_in[0], &Block_R_u_2, OUTPUT_SIZE * sizeof(uint32_t));

    setup_dma_channel(CH_DUPLIC, Mk_in,                   Cent_mk,                 (INPUT_SIZE + OUTPUT_SIZE));
    if (dma_validate_transaction(&dma_trans[1], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error validando DMA en canal %d\n\r", 1);
        return -1;
    }
    dma_load_transaction(&dma_trans[1]);
    //printf("S3 -> cent\n\r");
    dma_launch(&dma_trans[CH_DUPLIC]);
    dma_wait_for_channel(CH_DUPLIC);

    for (int i = 0; i < Pmax; i++) {
        memcpy(&ProjSubj_stage3[0], Vectores_Q_U[i], (DATA_SIZE * 2) * sizeof(uint32_t));
        if (i == 0) {
            memcpy(&ProjSubj_stage3[OUTPUT_SIZE * 2], &Cent_mk, INPUT_SIZE * sizeof(uint32_t));
            setup_dma_channel(CH_PROSUB, ProjSubj_stage3,  prosub_3,(INPUT_SIZE + (OUTPUT_SIZE * 2)));
        }
        else {
            memcpy(&ProjSubj_stage3[0], Vectores_Q_U[i], (DATA_SIZE * 2) * sizeof(uint32_t));
            setup_dma_channel(CH_PROSUB, ProjSubj_stage3,  prosub_3,(INPUT_SIZE + (OUTPUT_SIZE * 2)));
        }
        if (dma_validate_transaction(&dma_trans[3], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
            printf("Error validando DMA en canal %d\n\r", 3);
            return -1;
        }
        dma_load_transaction(&dma_trans[3]);
        //printf("S3 -> PrSu\n\r");
        dma_launch(&dma_trans[CH_PROSUB]);
        dma_wait_for_channel(CH_PROSUB);
    }
    ///////////////////////////////////////////////stage 4: detección de anomalías////////////////////////////////////////////////////////

    setup_dma_channel(CH_BRIGHT, prosub_3,                       (uint32_t *)&brg_buf_f, INPUT_SIZE);
    if (dma_validate_transaction(&dma_trans[2], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error validando DMA en canal %d\n\r", 2);
        return -1;
    }
    dma_load_transaction(&dma_trans[2]);
    //printf("S4 -> brig\n\r");
    dma_launch(&dma_trans[CH_BRIGHT]);
    dma_wait_for_channel(CH_BRIGHT);

    total_cycles = timer_stop();

    if (brg_buf_f.val > (1.5*val_tao)) {
        printf("Anomalía detectada en el índice %d con valor %d\n\r", brg_buf_f.index, brg_buf_f.val);
    } else {
        printf("No se detectaron anomalías.\n\r");
    }

    printf("T = %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));

    return 0;
}