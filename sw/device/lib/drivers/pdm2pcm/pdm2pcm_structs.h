/*
                              *******************
******************************* H SOURCE FILE *******************************
**                            *******************                          **
**                                                                         **
** project  : x-heep                                                       **
** filename : pdm2pcm_structs.h                                 **
** date     : 10/04/2026                                                      **
**                                                                         **
*****************************************************************************
**                                                                         **
**                                                                         **
*****************************************************************************

*/

/**
* @file   pdm2pcm_structs.h
* @date   10/04/2026
* @brief  Contains structs for every register
*
* This file contains the structs of the registes of the peripheral.
* Each structure has the various bit fields that can be accessed
* independently.
* 
*/

#ifndef _PDM2PCM_STRUCTS_H
#define PDM2PCM_STRUCTS

/****************************************************************************/
/**                                                                        **/
/**                            MODULES USED                                **/
/**                                                                        **/
/****************************************************************************/

#include <inttypes.h>
#include "core_v_mini_mcu.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/

#define pdm2pcm_peri ((volatile pdm2pcm *) PDM2PCM_START_ADDRESS)

/****************************************************************************/
/**                                                                        **/
/**                       TYPEDEFS AND STRUCTURES                          **/
/**                                                                        **/
/****************************************************************************/



typedef struct {

  uint32_t CLKDIVIDX;                             /*!< Decimation ratio from the sys_clk to the sampling frequency*/

  uint32_t CONTROL;                               /*!< Control register*/

  uint32_t STATUS;                                /*!< Status register*/

  uint32_t cic_activated_stages;                  /*!< Thermometric value of the activated stages (The 1s should be contiguous and right-aligned)*/

  uint32_t cic_delay_comb;                        /*!< delay in each comb block (D)*/

  uint32_t DECIMCIC;                              /*!< Samples count after which to decimate in the CIC filter.*/

  uint32_t RXDATA;                                /*!< Filtered output data*/

} pdm2pcm;

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED VARIABLES                            **/
/**                                                                        **/
/****************************************************************************/

#ifndef _PDM2PCM_STRUCTS_C_SRC



#endif  /* _PDM2PCM_STRUCTS_C_SRC */

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/**                                                                        **/
/**                          INLINE FUNCTIONS                              **/
/**                                                                        **/
/****************************************************************************/



#endif /* _PDM2PCM_STRUCTS_H */
/****************************************************************************/
/**                                                                        **/
/**                                EOF                                     **/
/**                                                                        **/
/****************************************************************************/
