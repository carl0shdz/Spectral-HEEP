#include <stdio.h>
#include "dma.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"


#define DATA_SIZE 16

int main() {
    dma_trans_t trans;
    dma_target_t tgt_src, tgt_dst;

    uint32_t src[DATA_SIZE] __attribute__((aligned(4))) = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint32_t dst[DATA_SIZE] __attribute__((aligned(4))) = {0};

    
    dma_init(NULL);					// Inicializar DMA

    // Configurar fuente
    tgt_src.ptr = (uint8_t *)src;
    tgt_src.inc_d1_du = 1;         			// Incrementar dirección en 1 por dato
    tgt_src.trig = DMA_TRIG_MEMORY;
    tgt_src.type = DMA_DATA_TYPE_WORD;

    // Configurar destino
    tgt_dst.ptr = (uint8_t *)dst;
    tgt_dst.inc_d1_du = 1;
    tgt_dst.trig = DMA_TRIG_MEMORY;
    tgt_dst.type = DMA_DATA_TYPE_WORD;

    printf("addr src: %p\n", (void*)src);
    printf("addr dst: %p\n", (void*)dst);
    
    for (int i = 0; i < DATA_SIZE/4; i++) {
        printf("dst T0 [%d] = 0x%08x\n", i, dst[i]);
    }
    
    // Configurar la transacción
    trans.src = &tgt_src;
    trans.dst = &tgt_dst;
    trans.size_d1_du = DATA_SIZE;
    trans.src_type = DMA_DATA_TYPE_WORD;
    trans.dst_type = DMA_DATA_TYPE_WORD;
    trans.mode = DMA_TRANS_MODE_SINGLE;  		// Modo simple
    trans.win_du = 0;
    trans.sign_ext = 0;
    trans.end = DMA_TRANS_END_INTR;       		// Notificar con interrupción

    // Validar, cargar y lanzar
    if (dma_validate_transaction(&trans, DMA_ENABLE_REALIGN, DMA_PERFORM_CHECKS_INTEGRITY) != DMA_CONFIG_OK) {
        printf("Error validando DMA\n");
        return -1;
    }
    dma_load_transaction(&trans);
    dma_launch(&trans);

    while (!dma_is_ready(0));				//Espero bandera de que ha terminado
    
    // Verificar resultado
    int errors = 0;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (src[i] != dst[i]) {
            printf("Error en indice %d: %x != %x\n", i, src[i], dst[i]);
            errors++;
        }
    }
    
    for (int i = 0; i < DATA_SIZE/4; i++) {
        printf("dst T1 [%d] = 0x%08x\n", i, dst[i]);
    }
    
    if (errors == 0)
        printf("DMA transferencia exitosa!\n");
    else
        printf("DMA transferencia con errores\n");

    return errors;
}
