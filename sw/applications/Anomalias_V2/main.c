#include <stdio.h>
#include <string.h>
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"
#include "input_data.h"
#include "timer_sdk.h"

#define DATA_SIZE     16
#define INPUT_SIZE    1920
#define OUTPUT_SIZE   16
#define NUM_CHANNELS  4
#define PMAX          1

typedef enum {
    CH_PIXELS = 0,
    CH_DUPLIC = 1,
    CH_BRIGHT = 2,
    CH_PROSUB = 3
} dma_channel_idx_t;

// Estructura perfecta para los canales de Promedio y Centralizado
typedef struct __attribute__((aligned(4))) {
    uint32_t centroid[OUTPUT_SIZE];
    uint32_t image[INPUT_SIZE];
} Centered_Block_t;

// Estructura interna para Proyección y Sustracción
typedef struct __attribute__((aligned(4))) {
    uint32_t bright_data[DATA_SIZE * 2]; 
    uint32_t r_data[INPUT_SIZE];         
} Block_q_u_r_t;

// Estructura completa de salida del Brillo
typedef struct __attribute__((aligned(4))) {
    uint32_t index;
    uint32_t val;
    Block_q_u_r_t q_u_r;
} brg_buffer_t;

// ==========================================
// MAPA DE MEMORIA OPTIMIZADO (Zero-Copy)
// ==========================================
// Buffers Etapa 1
Centered_Block_t s1_input __attribute__((aligned(4)));
brg_buffer_t s1_ping __attribute__((aligned(4)));

// Buffers Etapa 2
Centered_Block_t s2_input __attribute__((aligned(4)));
brg_buffer_t s2_ping __attribute__((aligned(4)));
brg_buffer_t s2_pong __attribute__((aligned(4)));

// Buffers Etapa 3
Centered_Block_t s3_input __attribute__((aligned(4)));
Block_q_u_r_t s3_ping __attribute__((aligned(4)));
Block_q_u_r_t s3_pong __attribute__((aligned(4)));

// Buffer Etapa 4
brg_buffer_t s4_out __attribute__((aligned(4)));

// Variables Globales
uint32_t Vectores_Q_U[PMAX][DATA_SIZE * 2] __attribute__((aligned(4))) = {0};
uint32_t index_tao;
uint32_t val_tao;

static dma_trans_t  dma_trans[NUM_CHANNELS];
static dma_target_t dma_src[NUM_CHANNELS];
static dma_target_t dma_dst[NUM_CHANNELS];

static inline void dma_wait_for_channel(dma_channel_idx_t channel) {
    while (!dma_is_ready(channel)) {
        CSR_CLEAR_BITS(CSR_REG_MSTATUS, 0x8);   
        if (!dma_is_ready(channel)) {
            wait_for_interrupt(); 
        }
        CSR_SET_BITS(CSR_REG_MSTATUS, 0x8); 
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
    dma_trans[ch].end        = DMA_TRANS_END_INTR; 
}

// Función auxiliar rápida para configurar, validar y lanzar dentro de bucles
static inline void dma_reload_and_launch(dma_channel_idx_t ch, uint32_t *src, uint32_t *dst, uint32_t size) {
    setup_dma_channel(ch, src, dst, size);
    dma_validate_transaction(&dma_trans[ch], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
    dma_load_transaction(&dma_trans[ch]);
    dma_launch(&dma_trans[ch]);
}

int main() {
    uint32_t total_cycles = 0;
    timer_cycles_init();
    dma_init(NULL);

    // ==========================================
    // PREPARACIÓN FUERA DE LA MEDICIÓN
    // ==========================================
    // Cargamos la imagen simulada en el buffer de la Etapa 1
    memcpy(s1_input.image, input_data, INPUT_SIZE * sizeof(uint32_t));
    
    // Cargamos la hipotética nueva imagen en el buffer de la Etapa 3
    memcpy(s3_input.image, input_data, INPUT_SIZE * sizeof(uint32_t)); 

    // Preconfiguramos los DMA de la Etapa 1 (Estáticos)
    setup_dma_channel(CH_PIXELS, s1_input.image, s1_input.centroid, INPUT_SIZE);
    setup_dma_channel(CH_DUPLIC, (uint32_t *)&s1_input, s1_ping.q_u_r.r_data, (INPUT_SIZE + OUTPUT_SIZE));
    setup_dma_channel(CH_BRIGHT, s1_ping.q_u_r.r_data, (uint32_t *)&s1_ping, INPUT_SIZE);
    
    // ATENCIÓN: El ProSub de la S1 escribe su salida directamente en la entrada de la S2. ¡Zero Copy!
    setup_dma_channel(CH_PROSUB, (uint32_t *)&s1_ping.q_u_r, s2_input.image, (INPUT_SIZE + (OUTPUT_SIZE * 2)));

    for (int i = 0; i < NUM_CHANNELS; i++) {
        dma_validate_transaction(&dma_trans[i], DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY);
        dma_load_transaction(&dma_trans[i]);
    }

    // ==========================================
    // INICIO DE MEDICIÓN PURA DE HARDWARE
    // ==========================================
    timer_start();

    // --- STAGE 1: Extracción ---
    dma_launch(&dma_trans[CH_PIXELS]); dma_wait_for_channel(CH_PIXELS);
    dma_launch(&dma_trans[CH_DUPLIC]); dma_wait_for_channel(CH_DUPLIC);
    dma_launch(&dma_trans[CH_BRIGHT]); dma_wait_for_channel(CH_BRIGHT);
    dma_launch(&dma_trans[CH_PROSUB]); dma_wait_for_channel(CH_PROSUB);


    // --- STAGE 2: Estimación del subespacio ---
    dma_reload_and_launch(CH_PIXELS, s2_input.image, s2_input.centroid, INPUT_SIZE);
    dma_wait_for_channel(CH_PIXELS);
    
    dma_reload_and_launch(CH_DUPLIC, (uint32_t *)&s2_input, s2_ping.q_u_r.r_data, (INPUT_SIZE + OUTPUT_SIZE));
    dma_wait_for_channel(CH_DUPLIC);

    // Bucle Ping-Pong (Sin memcpys masivos)
    for (int i = 0; i < PMAX; i++) {
        // Seleccionamos los buffers intercambiables según si la iteración es par o impar
        brg_buffer_t *src_buf = (i % 2 == 0) ? &s2_ping : &s2_pong;
        brg_buffer_t *dst_buf = (i % 2 == 0) ? &s2_pong : &s2_ping;

        dma_reload_and_launch(CH_BRIGHT, src_buf->q_u_r.r_data, (uint32_t *)src_buf, INPUT_SIZE);
        dma_wait_for_channel(CH_BRIGHT);

        // Copia microscópica (solo 32 words)
        memcpy(Vectores_Q_U[i], src_buf->q_u_r.bright_data, (DATA_SIZE * 2) * sizeof(uint32_t));
        index_tao = src_buf->index;
        val_tao = src_buf->val;

        dma_reload_and_launch(CH_PROSUB, (uint32_t *)&src_buf->q_u_r, dst_buf->q_u_r.r_data, (INPUT_SIZE + (OUTPUT_SIZE * 2)));
        dma_wait_for_channel(CH_PROSUB);
    }


    // --- STAGE 3: Cálculo del subespacio ortogonal ---
    // Copia microscópica (solo 16 words del centroide de fondo)
    memcpy(s3_input.centroid, s2_input.centroid, OUTPUT_SIZE * sizeof(uint32_t));

    dma_reload_and_launch(CH_DUPLIC, (uint32_t *)&s3_input, s3_ping.r_data, (INPUT_SIZE + OUTPUT_SIZE));
    dma_wait_for_channel(CH_DUPLIC);

    // Bucle Ping-Pong
    for (int i = 0; i < PMAX; i++) {
        Block_q_u_r_t *src_buf = (i % 2 == 0) ? &s3_ping : &s3_pong;
        Block_q_u_r_t *dst_buf = (i % 2 == 0) ? &s3_pong : &s3_ping;

        // Copia microscópica (solo 32 words de Q y U para esta iteración)
        memcpy(src_buf->bright_data, Vectores_Q_U[i], (DATA_SIZE * 2) * sizeof(uint32_t));

        dma_reload_and_launch(CH_PROSUB, (uint32_t *)src_buf, dst_buf->r_data, (INPUT_SIZE + (OUTPUT_SIZE * 2)));
        dma_wait_for_channel(CH_PROSUB);
    }


    // --- STAGE 4: Detección de Anomalías ---
    // Determinamos automáticamente dónde quedó el residual final según si PMAX es par o impar
    uint32_t *final_residual = (PMAX % 2 == 0) ? s3_ping.r_data : s3_pong.r_data;
    
    dma_reload_and_launch(CH_BRIGHT, final_residual, (uint32_t *)&s4_out, INPUT_SIZE);
    dma_wait_for_channel(CH_BRIGHT);

    // ==========================================
    // FIN DE MEDICIÓN PURA DE HARDWARE
    // ==========================================
    total_cycles = timer_stop();

    // Resultados (Impresiones fuera de la zona temporizada)
    if (s4_out.val > (1.5 * val_tao)) {
        printf("Anomalia detectada en el indice %u con valor %u\n\r", s4_out.index, s4_out.val);
    } else {
        printf("No se detectaron anomalias.\n\r");
    }

    printf("T = %u cc\n\r", total_cycles);
    printf("Tiempo: %u us\n\r", (uint32_t)get_time_from_cycles(total_cycles));

    return 0;
}